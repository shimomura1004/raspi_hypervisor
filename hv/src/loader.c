#include <inttypes.h>
#include "loader.h"
#include "fat32.h"
#include "mm.h"
#include "utils.h"
#include "debug.h"
#include "elf.h"
#include "arm/mmu.h"
#include "spinlock.h"
#include "cpu_core.h"

// todo: 初期化処理を呼んで初期化するようにする
static struct spinlock loader_lock = {0, "loader", -1};

// 指定された EL2 のメモリ上のプログラムコードを VM のメモリにロードする
// ハイパーバイザに埋め込まれた EL1 コードを VM にコピーするために使う
void copy_code_to_memory(struct vm_struct2 *vm, unsigned long va, unsigned long from, unsigned long size) {
    unsigned long current_va = va & PAGE_MASK;

    while (size > 0) {
        uint8_t *buf = (uint8_t *)allocate_vm_page(vm, current_va);
        int readsize = MIN(PAGE_SIZE, size);
        memcpy(buf, (void*)from, readsize);

        size -= readsize;
        from += readsize;
        current_va += PAGE_SIZE;
    }
}

int load_file_to_memory(struct vm_struct2 *vm, const char *name, unsigned long va) {
    // todo: ロックの単位が大きいのでもっと細分化する
    acquire_lock(&loader_lock);

    struct fat32_fs hfat;
    if (fat32_get_handle(&hfat) < 0) {
        WARN("failed to find fat32 file system");
        return -1;
    }

    struct fat32_file file;
    if (fat32_lookup(&hfat, name, &file) < 0) {
        WARN("requested file (%s) is not found", name);
        return -1;
    }

    int remain = fat32_file_size(&file);
    int offset = 0;
    unsigned long current_va = va & PAGE_MASK;

    while (remain > 0) {
        uint8_t *buf = (uint8_t *)allocate_vm_page(vm, current_va);
        int readsize = MIN(PAGE_SIZE, remain);
        int actualsize = fat32_read(&file, buf, offset, readsize);

        if (readsize != actualsize) {
            // todo: エラーで戻る前に既に確保したメモリを解放する
            WARN("failed to read raw file");
            return -1;
        }

        remain -= readsize;
        offset += readsize;
        current_va += PAGE_SIZE;
    }

    release_lock(&loader_lock);
    return 0;
}

// todo: 丸ごと elf.c に移す？
// todo: 関数が長いのでリファクタリングしたい
int elf_binary_loader(void *args, unsigned long *pc, unsigned long *sp, struct vm_struct2 *vm) {
    struct loader_args *loader_args = (struct loader_args *)args;

    INFO("Loading requested file(%s)", loader_args->filename);

    struct fat32_fs hfat;
    if (fat32_get_handle(&hfat) < 0) {
        WARN("failed to find fat32 file system");
        return -1;
    }

    struct fat32_file file;
    if (fat32_lookup(&hfat, loader_args->filename, &file) < 0) {
        WARN("requested file (%s) is not found", loader_args->filename);
        return -1;
    }

    // ハイパーバイザのメモリ空間に ELF ヘッダ分を読み込む(1ページで十分)
    uint8_t *buf = (uint8_t *)allocate_page();
    int readsize = MIN(PAGE_SIZE, sizeof(struct elf_header));
    int actualsize = fat32_read(&file, buf, 0, readsize);

    if (readsize != actualsize) {
        WARN("failed to read elf file");
        return -1;
    }

    // ELF ヘッダのチェック
    struct elf_header *header = (struct elf_header *)buf;
    if (elf_check(header) < 0) {
        WARN("wrong ELF format");
        free_page(buf);
        return -1;
    }

    // ELF ヘッダを格納したメモリは別用途で使ってしまうので、必要な情報を退避する
    uint16_t program_header_num = header->program_header_num;
    uint64_t program_header_offset = header->program_header_offset;
    uint16_t program_header_size = header->program_header_size;
    *pc = header->entry_point & 0xffffffffffff;

    // セグメントを順番にロード
    for (int i = 0; i < program_header_num; i++) {
        // ハイパーバイザのメモリ空間にプログラムヘッダを読み込む(1ページで十分)
        // todo: ここを関数として抜き出し
        int readsize = MIN(PAGE_SIZE, program_header_size);
        int actualsize = fat32_read(&file, buf, program_header_offset + program_header_size * i, readsize);

        if (readsize != actualsize) {
            WARN("failed to read file (program header)");
            return -1;
        }

        struct elf_program_header *phdr = (struct elf_program_header *)buf;

        // ロード可能なセグメントかを確認
        if (phdr->type != 1) {
            INFO("skipping unloadable segment %d", i);
            continue;
        }
        INFO("loading segment %d", i);

        // プログラムヘッダを格納したメモリは別用途で使ってしまうので、必要な情報を退避する
        uint64_t offset = phdr->offset;
        uint64_t virtual_addr = phdr->virtual_addr;
        // uint64_t physical_addr = phdr->physical_addr;
        uint64_t file_size = phdr->file_size;
        uint64_t memory_size = phdr->memory_size;
        INFO("file_size/memory_size: 0x%lx/0x%lx", file_size, memory_size);

        // 指定されたアドレスにセグメントをコピーする(ページ単位のコピーをループする)
        // todo: 関数化
        while (memory_size > 0) {
            // ページテーブルのインデックスを計算
            unsigned long ipa_page = virtual_addr & PAGE_MASK;
            // ページ内オフセットを計算
            unsigned long page_offset = virtual_addr & ~PAGE_MASK;
            // 今回書き込むバイト数
            unsigned long write_size = MIN(PAGE_SIZE - page_offset, memory_size);

            // 書き込み先の IPA が既に確保・マップされているか調べる
            unsigned long pa = ipa_to_pa(vm, ipa_page);
            uint8_t *vm_buf;
            if (pa) {
                // 既に確保されている
                vm_buf = (uint8_t *)P2V(pa);
            } else {
                // 未確保の場合は新規に確保し、stage2 テーブルに登録
                // allocate_vm_page の中の map_stage2_page で stage2 テーブルを更新している
                vm_buf = (uint8_t *)allocate_vm_page(vm, ipa_page);
            }

            // コピー元のデータをハイパーバイザのメモリ空間に読み込む
            if (file_size > 0) {
                // 今回のループで書き込めるサイズとファイルの残りサイズを比較して小さい方を選ぶ
                int read_size = MIN(write_size, file_size);
                // ページの途中からの場合でも正しくロードできるようオフセットを加味する
                //   リンカスクリプトの指示(ALIGN(0x1000))は無視される可能性がある
                int actualsize = fat32_read(&file, vm_buf + page_offset, offset, read_size);
                if (actualsize != read_size) {
                    WARN("failed to read file (segment data)");
                    return -1;
                }
                // もし残りの書き込みサイズが file_size を超える場合、その分は 0 で埋める
                if (write_size > read_size) {
                    memzero(vm_buf + page_offset + read_size, write_size - read_size);
                }
                file_size -= read_size;
                offset += read_size;
            } else {
                // ファイルからの読み込みが既に終わっている場合は 0 埋め
                memzero(vm_buf + page_offset, write_size);
            }

            memory_size -= write_size;
            virtual_addr += write_size;
        }
    }

    *sp = loader_args->sp;
    INFO("pc: 0x%lx in 48bit, sp(default): 0x%lx(0x%lx in 48bit)", *pc & 0xffffffffffff, *sp, *sp & 0xffffffffffff);

    free_page(buf);
    return 0;
}

int raw_binary_loader(void *args, unsigned long *pc, unsigned long *sp, struct vm_struct2 *vm) {
    struct loader_args *loader_args = (struct loader_args *)args;

    if (load_file_to_memory(vm, loader_args->filename, loader_args->loader_addr) < 0) {
        return -1;
    }

    *pc = loader_args->entry_point;
    *sp = loader_args->sp;

    return 0;
}

// 元々(raspberry-pi-os)は
//   カーネルの仮想メモリ空間(VA)と物理メモリ(PA)がリニアマッピング(boot.S で設定)
//   ユーザプロセスの仮想メモリ空間(VA)と物理メモリ(PA)は任意のマッピング(適宜設定)
// ハイパーバイザ化により
//   ハイパーバイザの仮想メモリ空間(IPA)と物理メモリ(PA)がリニアマッピング(boot.S で設定)
//   VM の仮想メモリ空間(VA)とハイパーバイザのメモリ空間(IPA)は任意のマッピング(適宜設定)
//   これとは別に、ホストの自身の VA も PA にリニアマッピングされる

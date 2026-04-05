#include <stdint.h>
#include "virtio_blk.h"
#include "mm.h"
#include "block.h"
#include "debug.h"

#define SECTOR_SIZE                 512
#define VIRTQ_ENTRY_NUM             16
#define VIRTIO_DEVICE_BLK           2
#define VIRTIO_MMIO_VIRTQ_ALIGN     512
// todo: 決め打ちになっているが、本来はデバイスツリーから探すべき
#define VIRTIO_BLK_PADDR            (VIRTIO_BLK_BASE + 0x3e00)

// virtio-blk デバイスの MMIO レジスタのオフセット
#define VIRTIO_REG_MAGIC            0x00
#define VIRTIO_REG_VERSION          0x04
#define VIRTIO_REG_DEVICE_ID        0x08
#define VIRTIO_REG_PAGE_SIZE        0x28
#define VIRTIO_REG_QUEUE_SEL        0x30
#define VIRTIO_REG_QUEUE_NUM_MAX    0x34
#define VIRTIO_REG_QUEUE_NUM        0x38
#define VIRTIO_REG_QUEUE_PFN        0x40
#define VIRTIO_REG_QUEUE_ALIGN      0x3C
#define VIRTIO_REG_QUEUE_READY      0x44
#define VIRTIO_REG_QUEUE_NOTIFY     0x50
#define VIRTIO_REG_DEVICE_STATUS    0x70
#define VIRTIO_REG_DEVICE_CONFIG    0x100

// virti-blk デバイスのステータスビット
#define VIRTIO_STATUS_ACK           1
#define VIRTIO_STATUS_DRIVER        2
#define VIRTIO_STATUS_DRIVER_OK     4
#define VIRTIO_STATUS_FEATURES_OK   8
#define VIRTQ_DESC_F_NEXT           1
#define VIRTQ_DESC_F_WRITE          2
#define VIRTQ_AVAIL_F_NO_INTERRUPT  1
#define VIRTIO_BLK_T_IN             0
#define VIRTIO_BLK_T_OUT            1

struct virtq_desc {
    uint64_t addr;
    uint32_t len;
    uint16_t flags;
    uint16_t next;
} __attribute__((packed));

struct virtq_avail {
    uint16_t flags;
    uint16_t index;
    uint16_t ring[VIRTQ_ENTRY_NUM];
} __attribute__((packed));

struct virtq_used_elem {
    uint32_t id;
    uint32_t len;
} __attribute__((packed));

struct virtq_used {
    uint16_t flags;
    uint16_t index;
    struct virtq_used_elem ring[VIRTQ_ENTRY_NUM];
} __attribute__((packed));

struct virtio_virtq {
    struct virtq_desc descs[VIRTQ_ENTRY_NUM];
    struct virtq_avail avail;
    // used ring の位置は VIRTIO_REG_PAGE_SIZE のアラインメントに合わせる必要がある
    //   通常のページサイズ(4KB)を使うと virtio_virtq のために連続した2ページが必要になるので
    //   VIRTIO_REG_PAGE_SIZE を 512B にしている
    struct virtq_used used __attribute__((aligned(VIRTIO_MMIO_VIRTQ_ALIGN)));
    int queue_index;
    volatile uint16_t *used_index;
    uint16_t last_used_index;
} __attribute__((packed));

struct virtio_blk_req {
    uint32_t type;
    uint32_t reserved;
    uint64_t sector;
    uint8_t data[512];
    uint8_t status;
} __attribute__((packed));

struct virtio_virtq *blk_request_vq;
struct virtio_blk_req *blk_req;
uint64_t blk_req_paddr;
uint64_t blk_capacity;

static uint32_t virtio_reg_read32(unsigned offset) {
    return *((volatile uint32_t *) (P2V(VIRTIO_BLK_PADDR) + offset));
}

static void virtio_reg_write32(unsigned offset, uint32_t value) {
    *((volatile uint32_t *) (P2V(VIRTIO_BLK_PADDR) + offset)) = value;
}

static uint64_t virtio_reg_read64(unsigned offset) {
    return *((volatile uint64_t *) (P2V(VIRTIO_BLK_PADDR) + offset));
}

static struct virtio_virtq *virtq_init(unsigned index) {
    // 1ページ分(4KB)を確保
    // ハイパーバイザでは全メモリ領域がリニアマッピングされているため、ページテーブルの追加設定は不要
    unsigned long virtq_paddr = get_free_page();
    if (virtq_paddr == 0) {
        return NULL;
    }

    struct virtio_virtq *vq = (struct virtio_virtq *) P2V(virtq_paddr);
    vq->queue_index = index;
    vq->used_index = (volatile uint16_t *) &vq->used.index;

    // キューを選択
    virtio_reg_write32(VIRTIO_REG_QUEUE_SEL, index);

    // キューサイズ(使用するディスクリプタの数)を指定
    virtio_reg_write32(VIRTIO_REG_QUEUE_NUM, VIRTQ_ENTRY_NUM);

    // キューのアライメントを指定
    virtio_reg_write32(VIRTIO_REG_QUEUE_ALIGN, VIRTIO_MMIO_VIRTQ_ALIGN);

    // キューのページフレーム番号(PFN)を書き込む
    //   PFN は物理アドレスをページサイズで割った値
    // VIRTIO_REG_PAGE_SIZE で指定したアライメント単位で割る必要がある
    virtio_reg_write32(VIRTIO_REG_QUEUE_PFN, virtq_paddr / VIRTIO_MMIO_VIRTQ_ALIGN);

    // デバイスにキューの準備ができたことを伝える (Modern の場合は READY レジスタ、Legacy は PFN 書き込みで有効化)
    virtio_reg_write32(VIRTIO_REG_QUEUE_READY, 1);

    return vq;
}

int virtio_blk_init(void) {
    const uint32_t magic = virtio_reg_read32(VIRTIO_REG_MAGIC);
    const uint32_t version = virtio_reg_read32(VIRTIO_REG_VERSION);
    const uint32_t device_id = virtio_reg_read32(VIRTIO_REG_DEVICE_ID);

    if (magic != 0x74726976) {
        WARN("virtio: invalid magic value: %x", magic);
        return BLOCK_ERROR;
    }
    if (version != 1) {
        WARN("virtio: invalid version: %d", version);
        return BLOCK_ERROR;
    }
    if (device_id != VIRTIO_DEVICE_BLK) {
        WARN("virtio: invalid device id: %d", device_id);
        return BLOCK_ERROR;
    }

    // virtio-blk の仕様に従って virtio-blk デバイスを初期化する
    // デバイスをリセット
    virtio_reg_write32(VIRTIO_REG_DEVICE_STATUS, 0);

    // ACKNOWLEDGE ステータスビットを設定
    uint32_t status = virtio_reg_read32(VIRTIO_REG_DEVICE_STATUS);
    virtio_reg_write32(VIRTIO_REG_DEVICE_STATUS, status | VIRTIO_STATUS_ACK);

    // DRIVER ステータスビットを設定
    status = virtio_reg_read32(VIRTIO_REG_DEVICE_STATUS);
    virtio_reg_write32(VIRTIO_REG_DEVICE_STATUS, status | VIRTIO_STATUS_DRIVER);

    // 機能ネゴシエーション完了 (本来は機能の読み書きが必要だが、簡易実装のためそのまま OK を出す)
    status = virtio_reg_read32(VIRTIO_REG_DEVICE_STATUS);
    virtio_reg_write32(VIRTIO_REG_DEVICE_STATUS, status | VIRTIO_STATUS_FEATURES_OK);

    // このドライバで使うページサイズをデバイス側に伝える
    // 通常のページサイズは 4KB だが、ここではあえて小さい 512B を指定している
    virtio_reg_write32(VIRTIO_REG_PAGE_SIZE, VIRTIO_MMIO_VIRTQ_ALIGN);

    // ディスク読み書き用のキューを初期化
    blk_request_vq = virtq_init(0);

    // DRIVER_OK ステータスビットを設定
    virtio_reg_write32(VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_DRIVER_OK);

    // ディスクの容量を取得
    blk_capacity = virtio_reg_read64(VIRTIO_REG_DEVICE_CONFIG + 0) * SECTOR_SIZE;
    INFO("virtio-blk: capacity is %lu bytes", blk_capacity);

    // デバイスへの処理要求を格納する領域を確保
    blk_req_paddr = get_free_page();
    if (blk_req_paddr == 0) {
        return BLOCK_ERROR;
    }
    blk_req = (struct virtio_blk_req *) P2V(blk_req_paddr);

    return BLOCK_OK;
}

static int virtq_is_busy(struct virtio_virtq *vq) {
    // メモリ同期: デバイスが書いた used_index を確実に読み込む
    __asm__ volatile("dsb sy" ::: "memory");
    // ドライバが最後に確認した位置と、デバイスが更新した位置が同じなら、まだ未処理（ビジー）
    return vq->last_used_index == *vq->used_index;
}

static void virtq_kick(struct virtio_virtq *vq, int desc_index) {
    vq->avail.ring[vq->avail.index % VIRTQ_ENTRY_NUM] = desc_index;

    // インデックスを増やす前に、リングの内容がメモリに書き込まれていることを保証
    __asm__ volatile("dsb sy" ::: "memory");
    vq->avail.index++;

    // インデックスの更新をデバイスに見えるようにしてから Notify する
    __asm__ volatile("dsb sy" ::: "memory");

    virtio_reg_write32(VIRTIO_REG_QUEUE_NOTIFY, vq->queue_index);
}

// `lba` のセクタから `num` セクタ読んで `buf` に書き込む
int virtio_blk_read(unsigned int lba, unsigned char *buf, unsigned int num) {
    if (lba + num > blk_capacity / SECTOR_SIZE) {
        WARN("virtio: out of bounds read: sector=%u, num=%u, capacity=%llu",
             lba, num, blk_capacity / SECTOR_SIZE);
        return 0;
    }

    for (unsigned int i = 0; i < num; i++) {
        // virtio_blk デバイスに対する読み込み要求を準備
        blk_req->sector = lba + i;
        blk_req->type = VIRTIO_BLK_T_IN;
        // ステータスを初期化(デバイスが 0 以外に書き込むことで完了を知らせる)
        blk_req->status = 0xFF;

        struct virtio_virtq *vq = blk_request_vq;
        
        // ヘッダ部 (type, reserved, sector)
        vq->descs[0].addr = blk_req_paddr;
        vq->descs[0].len = 16; 
        vq->descs[0].flags = VIRTQ_DESC_F_NEXT;
        vq->descs[0].next = 1;

        // データ部
        vq->descs[1].addr = blk_req_paddr + offsetof(struct virtio_blk_req, data);
        vq->descs[1].len = SECTOR_SIZE;
        vq->descs[1].flags = VIRTQ_DESC_F_NEXT | VIRTQ_DESC_F_WRITE;
        vq->descs[1].next = 2;

        // ステータス部
        vq->descs[2].addr = blk_req_paddr + offsetof(struct virtio_blk_req, status);
        vq->descs[2].len = 1;
        vq->descs[2].flags = VIRTQ_DESC_F_WRITE;

        // デバイス側に要求を通知し、完了を待つ
        virtq_kick(vq, 0);
        while (virtq_is_busy(vq))
            ;
        
        // デバイスが処理を終えたので、ドライバ側のインデックスを進める
        vq->last_used_index++;

        if (blk_req->status != 0) {
            WARN("virtio: read error at sector %u, status=%d", lba + i, blk_req->status);
            return i * SECTOR_SIZE; // 読み込みに成功したバイト数までを返す
        }

        memcpy(buf + (i * SECTOR_SIZE), blk_req->data, SECTOR_SIZE);
    }

    return num * SECTOR_SIZE;
}

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
#define VIRTIO_REG_QUEUE_READY      0x44
#define VIRTIO_REG_QUEUE_NOTIFY     0x50
#define VIRTIO_REG_DEVICE_STATUS    0x70
#define VIRTIO_REG_DEVICE_CONFIG    0x100

// virti-blk デバイスのステータスビット
#define VIRTIO_STATUS_ACK           1
#define VIRTIO_STATUS_DRIVER        2
#define VIRTIO_STATUS_DRIVER_OK     4
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

    // キューのページフレーム番号(PFN)を書き込む
    //   PFN は物理アドレスをページサイズで割った値
    // VIRTIO_REG_PAGE_SIZE で指定したアライメント単位で割る必要がある
    virtio_reg_write32(VIRTIO_REG_QUEUE_PFN, virtq_paddr / VIRTIO_MMIO_VIRTQ_ALIGN);
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

    // このドライバで使うページサイズをデバイス側に伝える
    // 通常のページサイズは 4KB だが、ここではあえて小さい 512B を指定している
    virtio_reg_write32(VIRTIO_REG_PAGE_SIZE, VIRTIO_MMIO_VIRTQ_ALIGN);

    // ディスク読み書き用のキューを初期化
    blk_request_vq = virtq_init(0);

    // DRIVER_OK ステータスビットを設定
    virtio_reg_write32(VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_DRIVER_OK);

    // ディスクの容量を取得
    blk_capacity = virtio_reg_read64(VIRTIO_REG_DEVICE_CONFIG + 0) * SECTOR_SIZE;
    INFO("virtio-blk: capacity is %llu bytes", blk_capacity);

    // デバイスへの処理要求を格納する領域を確保
    blk_req_paddr = get_free_page();
    if (blk_req_paddr == 0) {
        return BLOCK_ERROR;
    }
    blk_req = (struct virtio_blk_req *) P2V(blk_req_paddr);

    return BLOCK_OK;
}

int virtio_blk_read(unsigned int lba, unsigned char *buffer, unsigned int num) {
    // todo: Implement VirtIO read logic
    WARN("virtio_blk_read: Not implemented yet");
    return 0; // 0 bytes read (error)
}

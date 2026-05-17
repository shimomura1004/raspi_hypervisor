#include "board_config.h"
#include "common_utils.h"

// これは BCM2837 に搭載されるペリフェラルである system timer のドライバ
// RPi3 には 1tick ごとにカウントアップするタイマが搭載されていて
// 合計4個の比較用レジスタがあり、カウンタの値が一致すると対応する割込み線を発火させる

static unsigned long base_address;
static unsigned int interval;

void systimer_init(unsigned long base_virt_address, unsigned long systimer_interval) {
    base_address = base_virt_address;
    interval = systimer_interval;

    // 今のカウンタ値から interval tick 後に発火するように2個目のレジスタに値をセットする
    unsigned int next = get32(base_address + SYSTIMER_CLO_OFFSET) + interval;
    put32(base_address + SYSTIMER_C1_OFFSET, next);
}

// システムタイマのレジスタ CLO/CHI を読み、合わせて64ビット値として返す
// 32ビットずつ別々に読まないといけないので、読んでいる間に値が更新される可能性がある
// 上位32ビットを2回読んで、更新されていたら読み直す
// この対応がないと最大 2^32 us(約72分)ずれる
unsigned long get_physical_systimer_count() {
    unsigned long chi = get32(base_address + SYSTIMER_CHI_OFFSET);
    unsigned long clo = get32(base_address + SYSTIMER_CLO_OFFSET);
    if (chi != get32(base_address + SYSTIMER_CHI_OFFSET)) {
        chi = get32(base_address + SYSTIMER_CHI_OFFSET);
        clo = get32(base_address + SYSTIMER_CLO_OFFSET);
    }
    return clo | (chi << 32);
}

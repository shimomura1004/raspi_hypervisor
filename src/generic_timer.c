#include "generic_timer.h"
#include "sched.h"
#include "utils.h"

// タスクスケジューリングの guranularity は 20ms
const static unsigned int interval_ms = 20;

// CNTFRQ_EL0: Counter-timer Frequency register
// https://developer.arm.com/documentation/111107/2025-12/AArch64-Registers/CNTFRQ-EL0--Counter-timer-Frequency-Register
// [31:0] ClockFreq: indicate the effective frequency of the system counter, in Hz.

// CNTP_CTL_EL0: Counter-timer Physical Timer Control register
// Control register for the EL1 physical timer.
// https://developer.arm.com/documentation/ddi0601/2025-12/AArch64-Registers/CNTP-CTL-EL0--Counter-timer-Physical-Timer-Control-Register
// [2] ISTATUS: The status of the timer.
//   0b0: Timer condition is not met.
//   0b1: Timer condition is met.
// [1] IMASK: Timer interrupt mask bit.
//   0b0: Timer interrupt is not masked by the IMASK bit.
//   0b1: Timer interrupt is masked by the IMASK bit.
// [0] ENABLE: Enables the timer.
//   0b0 Timer disabled.
//   0b1 Timer enabled.

// CNTP_TVAL_EL0: Counter-timer Physical Timer TimerValue register
// Holds the timer value for the EL1 physical timer.
// https://developer.arm.com/documentation/ddi0595/2021-09/AArch64-Registers/CNTP-TVAL-EL0--Counter-timer-Physical-Timer-TimerValue-register?lang=en
// [31:0] TimerValue
//   If CNTP_CTL_EL0.ENABLE is 0, the value returned is UNKNOWN.
//   If CNTP_CTL_EL0.ENABLE is 1, the value returned is (CNTP_CVAL_EL0 - CNTPCT_EL0).
//     つまりタイマが有効なときにこの値を読むと、比較値までの残り時間(tick数)が返ってくる
// このレジスタに書き込むと、現在のカウント値に書いた値を加えた値が CNTP_CVAL_EL0 にセットされる

// CNTP_CVAL_EL0: Counter-timer Physical Timer CompareValue register
// Holds the compare value for the EL1 physical timer.
// https://developer.arm.com/documentation/ddi0595/2021-09/AArch64-Registers/CNTP-CVAL-EL0--Counter-timer-Physical-Timer-CompareValue-register?lang=en
// [63:0] Compare value
//   The timer condition is met when (CNTPCT_EL0 - CompareValue) is greater than or equal to zero.

// CNTPCT_EL0: Counter-timer Physical Count register
// Holds the 64-bit physical count value.
// https://developer.arm.com/documentation/ddi0601/2020-12/AArch64-Registers/CNTPCT-EL0--Counter-timer-Physical-Count-register?lang=en
// [63:0] Pyhisical count value
//   Reads of CNTPCT_EL0 from EL0 or EL1 return (PCount<63:0> - CNTPOFF_EL2<63:0>)

void generic_timer_init() {
    // 周波数は可変かもしれないので毎回読む
	unsigned long frequency;
    // 周波数の読み取り
	asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
    // interval で指定したミリ秒数に何 tick 必要か計算する
	unsigned long ticks = (frequency / 1000) * interval_ms;
    // タイマの初期値セット(ここで指定した tick 数後に最初のタイマ割込みが発生する)
	asm volatile("msr cntp_tval_el0, %0" : : "r"(ticks));
    // タイマの有効化
	asm volatile("msr cntp_ctl_el0, %0" : : "r"(1));
}

void handle_generic_timer_irq() {
	unsigned long frequency;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
	unsigned long ticks = (frequency / 1000) * interval_ms;

    // 次のタイマ割込みが発生するよう値を再セット
	asm volatile("msr cntp_tval_el0, %0" : : "r"(ticks));

    // この pCPU 上で動く vCPU を切り替える
    timer_tick();
}

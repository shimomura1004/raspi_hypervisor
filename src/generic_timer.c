#include "generic_timer.h"
#include "sched.h"
#include "utils.h"

// タスクスケジューリングの guranularity は 20ms
const static unsigned int interval_ms = 20;

// CNTFRQ_EL0: Counter-timer Frequency register
// https://developer.arm.com/documentation/111107/2025-12/AArch64-Registers/CNTFRQ-EL0--Counter-timer-Frequency-Register
// [31:0] ClockFreq: indicate the effective frequency of the system counter, in Hz.

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

// CNTHCTL_EL2: Counter-timer Hypervisor Control Register
// Controls the generation of an event stream from the physical counter,
//   and access from EL1 to the physical counter and the EL1 physical timer.
// https://developer.arm.com/documentation/111179/2025-09_ASL1/AArch64-Registers/CNTHCTL-EL2--Counter-timer-Hypervisor-Control-Register
// When VHE is implemented:
//   [19] CNTPMASK:
//   [18] CNTVMASK:
//   [17] EVNTIS:
//   [16] EL1NVVCT:
//   [15] EL1NVPCT:
//   [14] EL1TVCT:
//   [13] EL1TVT:
//   [12] ECV:
//   [11] EL1PTEN:
//   [10] EL1PCTEN: 
//   [9] EL0PTEN:
//   [8] EL0VTEN:
//   [7:4] EVNTI:
//   [3] EVNTDIR:
//   [2] EVNTEN:
//   [1] EL0VCTEN:
//   [0] EL0PCTEN:
// When VHE is not implemented:
//   [19] CNTPMASK:
//   [18] CNTVMASK:
//   [17] EVNTIS:
//   [16] EL1NVVCT:
//   [15] EL1NVPCT:
//   [14] EL1TVCT:
//   [13] EL1TVT:
//   [12] ECV:
//   [11:8] Reserved
//   [7:4] EVNTI:
//   [3] EVNTDIR:
//   [2] EVNTEN:
//   [1] EL1PCEN: EL0/EL1 からの physical timer register へのアクセスを EL2 でトラップする
//     0b0: EL0 and EL1 accesses to the specified registers are trapped to EL2.
//     0b1: This control does not cause any instructions to be trapped.
//   [0] EL1PCTEN: EL0/EL1 からの physical counter register へのアクセスを EL2 でトラップする
//     0b0: EL0 and EL1 accesses to the specified registers are trapped to EL2.
//     0b1: This control does not cause any instructions to be trapped.

// CNTVOFF_EL2: Counter-timer Virtual Offset Register
// Holds the 64-bit virtual offset. This is the offset for the AArch64 virtual timers and counters.
// https://developer.arm.com/documentation/111107/2025-09/AArch64-Registers/CNTVOFF-EL2--Counter-timer-Virtual-Offset-Register
// [63:0] VOffset: Virtual offset

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

	// Generic Timer の仮想化の設定
	// EL1 から CNTPCT_EL0(physical counter) へのアクセスを禁止ラップ
	unsigned long cnthctl;
	asm volatile("mrs %0, cnthctl_el2" : "=r"(cnthctl));
    // EL1PCTEN ビットのみ 0 にクリア
	cnthctl &= ~1; // EL1PCTEN
	asm volatile("msr cnthctl_el2, %0" : : "r"(cnthctl));

	// Virtual Timer のオフセットを 0 に設定 (ゲスト向け仮想時間は物理時間と常に同じ)
    // todo: ゲストごとにオフセットを変える場合は VM の復帰ごとに設定し直す必要がある
	asm volatile("msr cntvoff_el2, %0" : : "r"(0));
}

// ハイパーバイザ用の Generic Timer の割込みハンドラ
void handle_generic_timer_irq() {
	unsigned long frequency;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
	unsigned long ticks = (frequency / 1000) * interval_ms;

    // 次のタイマ割込みが発生するよう値を再セット
	asm volatile("msr cntp_tval_el0, %0" : : "r"(ticks));

    // この pCPU 上で動く vCPU を切り替える
    timer_tick();
}

#include <stddef.h>
#include <stdint.h>

#include "printf.h"
#include "utils.h"
#include "generic_timer.h"
#include "irq.h"
#include "peripherals/irq.h"
#include "vm.h"
#include "sched.h"
#include "mini_uart.h"
#include "mm.h"
#include "sd.h"
#include "loader.h"
#include "cpu_core.h"
#include "debug.h"

// boot.S で初期化が終わるまでコアを止めるのに使うフラグ
// .bss ではなく .data に配置することで、リセット時に確実に 0 に初期化されるようにする
//   本来は .bss はブートローダやスタートアップコードでゼロクリアする必要があるが、
//   簡易実装の場合はこの処理が漏れる可能性がある
volatile unsigned long initialized_flag __attribute__((section(".data"))) = 0;

static struct loader_args vmm_elf_args = {
	.loader_addr = 0x0,
	.entry_point = 0x0,
	.sp = 0xffff000000100000,
	.filename = "VMM.ELF",
	.vcpu_num = 1,
};

// 各 pCPU で必要な初期化処理
static void initialize_pcpu(unsigned long cpuid) {
	// CPU コア構造体の初期化
	init_pcpu_struct(cpuid);

	// VBAR_EL2 レジスタに割込みベクタのアドレスを設定する
	// 各 CPU コアで呼び出す必要がある
	irq_vector_init();

	// システムタイマ(Generic Timer)の初期化
	generic_timer_init();

	// 割込みコントローラの有効化
	disable_irq();
	enable_interrupt_controller(cpuid);
}

// 全コア共通で一度だけ実施する初期化処理
static void initialize_hypervisor() {
	mm_init();
	uart_init();
	init_printf(NULL, putc);

	// todo: セキュアモニタ経由で起動された場合はここでセキュアモニタのログを出力

	// // システムタイマは全コアで共有されるのでここで初期化
	// systimer_init();

	// SD カードの初期化
	if (sd_init() < 0) {
		PANIC("sd_init() failed");
	}
}

static void prepare_vmm() {
	if (create_vm_with_loader(elf_binary_loader, &vmm_elf_args) < 0) {
		WARN("error while starting VMM");
	}
}

// hypervisor としてのスタート地点
void hypervisor_main(unsigned long cpuid)
{
	// 実行中の CPU コアを初期化
	initialize_pcpu(cpuid);

	// CPU 0 がハイパーバイザの初期化を実施
	if (cpuid == 0) {
		// ハイパーバイザの初期化とゲストのロードを実施
		initialize_hypervisor();
		INFO("Raspvisor initialized");

		// pCPU の数と同じ数の vCPU を持った idle VM を準備
		create_idle_vm();
		INFO("Idle VM and idle vCPUs are created");

		prepare_vmm();
		INFO("Guest VMs are prepared");

		initialized_flag = 1;
		asm volatile ("dc civac, %0" : : "r" (&initialized_flag) : "memory");
		asm volatile ("sev");
	}

	INFO("pCPU%d start running", cpuid);

	scheduler(cpuid);
}

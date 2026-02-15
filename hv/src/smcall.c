#include "debug.h"

// todo: smc 命令の引数の即値(smc_nr)で判定しているがよいか？ x8 などを見るべきでは？
void smcall(unsigned long smc_nr, unsigned long a0, unsigned long a1, unsigned long a2, unsigned long a3) {
    struct pt_regs *regs = vcpu_pt_regs(current_pcpu()->current_vcpu);

    INFO("%u, %d, %d, %d, %d", smc_nr, a0, a1, a2, a3);

    switch (smc_nr) {
	case 0: {
		WARN("SMC #%lu(%lu)", smc_nr, a0);
		break;
    }
    }

	// todo: Secure monitor に通知するだけの仮実装になっている
    //       一部はハイパーバイザでエミュレート、一部はセキュアモニタに再度通知
    asm volatile ("smc #0");
}

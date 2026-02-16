#include "utils.h"
#include "debug.h"

void smcall(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long x4) {
    // SMCCC に従い、x0 を Function ID として使用する
    // unsigned long id = x0;

    INFO("SMC: %lu, %lu, %lu, %lu, %lu", x0, x1, x2, x3, x4);

	// todo: Secure monitor に通知するだけの仮実装になっている
    //       一部はハイパーバイザでエミュレート、一部はセキュアモニタに再度通知
    issue_smc(x0, x1, x2, x3, x4);
}

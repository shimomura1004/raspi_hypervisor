#include "utils.h"
#include "debug.h"

void smcall(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3) {
    // SMCCC に従い、x0 を Function ID として使用する
    // unsigned long id = x0;

    INFO("SMC: %lu, %lu, %lu, %lu", x0, x1, x2, x3);

    // todo: Secure monitor に通知するだけの仮実装になっている
    //       一部はハイパーバイザでエミュレート、一部はセキュアモニタに再度通知
    issue_smc(x0, x1, x2, x3);
}

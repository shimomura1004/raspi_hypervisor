#ifndef _UTILS_H
#define _UTILS_H

#include "common_utils.h"

struct cpu_sysregs;

extern uint64_t translate_el1(uint64_t);
extern uint64_t translate_el12(uint64_t);
extern uint64_t get_ttbr0_el1();
extern uint64_t get_ttbr1_el1();
extern uint64_t get_ttbr0_el2();
extern uint64_t get_vttbr_el2();
extern uint64_t get_sp();
extern void set_vmpidr_el2(uint64_t);
extern void issue_smc(uint64_t, uint64_t, uint64_t, uint64_t);

// Stage2 変換テーブルをセットしてアドレス空間(VTTBR_EL2)を切り替え、つまり IPA -> PA の変換テーブルを切り替える
//   テーブル自体の準備は VM がロードされた初期化時やメモリアボート時に行う
// VM ごとにアドレスの上位8ビットが異なるようになっている
extern void set_stage2_pgd(uint64_t pgd, uint64_t vmid);
// x0 が指すメモリアドレスに保存された値を各システムレジスタに復元する
extern void restore_sysregs(struct cpu_sysregs *);
// 各システムレジスタの値を取り出し、x0 が指すメモリアドレスに保存する
extern void save_sysregs(struct cpu_sysregs *);
// すべての各システムレジスタの値を取り出し、x0 が指すメモリアドレスに保存する
extern void get_all_sysregs(struct cpu_sysregs *);

extern void assert_vfiq(void);
extern void assert_virq(void);
extern void assert_vserror(void);
extern void clear_vfiq(void);
extern void clear_virq(void);
extern void clear_vserror(void);

#endif  /*_UTILS_H */

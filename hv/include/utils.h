#ifndef	_UTILS_H
#define	_UTILS_H

#include "common_utils.h"

struct cpu_sysregs;

extern unsigned long translate_el1(unsigned long);
extern unsigned long translate_el12(unsigned long);
extern unsigned long get_ttbr0_el1();
extern unsigned long get_ttbr1_el1();
extern unsigned long get_ttbr0_el2();
extern unsigned long get_vttbr_el2();
extern unsigned long get_cpuid();
extern unsigned long get_sp();
extern void set_vmpidr_el2(unsigned long);
extern void issue_smc(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);

// Stage2 変換テーブルをセットしてアドレス空間(VTTBR_EL2)を切り替え、つまり IPA -> PA の変換テーブルを切り替える
//   テーブル自体の準備は VM がロードされた初期化時やメモリアボート時に行う
// VM ごとにアドレスの上位8ビットが異なるようになっている
extern void set_stage2_pgd(unsigned long pgd, unsigned long vmid);
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

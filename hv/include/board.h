#ifndef _BOARD_H
#define _BOARD_H

struct vm_struct2;
struct vcpu_struct;

#define HAVE_FUNC(ops, func) ((ops) && (ops)->func)

struct board_ops {
    void (*initialize)(struct vm_struct2 *vm);
    unsigned long (*mmio_read)(struct vcpu_struct *vcpu, unsigned long addr);
    void (*mmio_write)(struct vcpu_struct *vcpu, unsigned long addr, unsigned long val);
    void (*entering_vm)(struct vcpu_struct *vcpu);
    void (*leaving_vm)(struct vcpu_struct *vcpu);
    int (*is_irq_asserted)(struct vcpu_struct *vcpu);
    int (*is_fiq_asserted)(struct vcpu_struct *vcpu);
    void (*debug)(struct vcpu_struct *vcpu);
};

const struct board_ops *get_default_board_ops(void);

#endif /* _BOARD_H */

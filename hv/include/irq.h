#ifndef	_IRQ_H
#define	_IRQ_H

void irq_vector_init( void );
void enable_irq( void );
void disable_irq( void );
void enable_local_interrupt_controller(unsigned long cpuid);
void enable_legacy_interrupt_controller();
int is_interrupt_enabled( void );

void enable_virtual_timer_irq(void);
void disable_virtual_timer_irq(void);

#endif  /*_IRQ_H */

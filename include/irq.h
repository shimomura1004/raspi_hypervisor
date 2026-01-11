#ifndef	_IRQ_H
#define	_IRQ_H

void irq_vector_init( void );
void enable_irq( void );
void disable_irq( void );
void enable_interrupt_controller(unsigned long cpuid);
int is_interrupt_enabled( void );

#endif  /*_IRQ_H */

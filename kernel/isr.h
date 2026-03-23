/* kernel/isr.h */
#ifndef ISR_H
#define ISR_H

typedef void (*irq_handler_t)(void);
void irq_install_handler(int irq, irq_handler_t handler);
void irq_handler(int irq);

#endif

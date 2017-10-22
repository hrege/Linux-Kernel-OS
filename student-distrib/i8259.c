/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
	cli();											//irq_save??
	//Start initialization sequence with 0x11
	outb(MASTER_8259_PORT, ICW1);

	//Write ICW2
	outb(MASTER_8259_PORT + 1, ICW2_MASTER);
	outb(SLAVE_8259_PORT + 1, ICW2_SLAVE);

	//Write ICW3
	if((ICW1 & 0x01) == 0) {
		outb(MASTER_8259_PORT, ICW3_MASTER);
		outb(SLAVE_8259_PORT, ICW3_SLAVE);
	}

	outb(MASTER_8259_PORT + 1, ICW4);
	outb(SLAVE_8259_PORT + 1, ICW4);

	sti();
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
	if(irq_num > 7) {
		outb(SLAVE_8259_PORT, EOI);
	}
	outb(MASTER_8259_PORT, EOI);
}

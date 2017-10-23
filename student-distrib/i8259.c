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

	outb(0xFF, MASTER_DATA_PORT);
	outb(0xFF, SLAVE_DATA_PORT);


	//Start initialization sequence with 0x11
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW1, SLAVE_8259_PORT);

	//Write ICW2
	outb(ICW2_MASTER, MASTER_DATA_PORT);
	outb(ICW2_SLAVE, SLAVE_DATA_PORT);

	//Write ICW3
	// if((ICW1 & 0x02) == 0) {
		outb(ICW3_MASTER, MASTER_DATA_PORT);
		outb(ICW3_SLAVE, SLAVE_DATA_PORT);
	// }


	outb(ICW4, MASTER_DATA_PORT);
	outb(ICW4, SLAVE_DATA_PORT);

}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
	uint16_t port;
	uint8_t value;

	if(irq_num < 8) {
		port = MASTER_DATA_PORT;
	}
	else {
		port = SLAVE_DATA_PORT;
		irq_num -= 8;
	}
	value = inb(port) & ~(1 << irq_num);
	outb(port, value);
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
	uint16_t port;
	uint8_t value;

	if(irq_num < 8) {
		port = MASTER_DATA_PORT;
	}
	else {
		port = SLAVE_DATA_PORT;
		irq_num -= 8;
	}

	value = inb(port) | (1 << irq_num);
	outb(port, value);
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
	if(irq_num >= 8) {
		outb(EOI, SLAVE_8259_PORT);
	}
	outb(EOI, MASTER_8259_PORT);
}

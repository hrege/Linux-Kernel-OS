/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* i8259_init
*		Description: Initialize the 8259 PIC
*		Author: Hershel
*		Input: none
*		Output: none
*		Return: none
*		Side effects: pic is enabled
*/
void i8259_init(void) {
	unsigned long flags;

	cli_and_save(flags);

	outb(0xFF, MASTER_DATA_PORT);
	outb(0xFF, SLAVE_DATA_PORT);

	//Start initialization sequence with 0x11
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW2_MASTER, MASTER_DATA_PORT);
	outb(ICW3_MASTER, MASTER_DATA_PORT);
	outb(ICW4, MASTER_DATA_PORT);

	outb(ICW1, SLAVE_8259_PORT);
	outb(ICW2_SLAVE, SLAVE_DATA_PORT);
	outb(ICW3_SLAVE, SLAVE_DATA_PORT);
	outb(ICW4, SLAVE_DATA_PORT);

	master_mask &= ~(0x4);
	outb(master_mask, MASTER_DATA_PORT);
	outb(slave_mask, SLAVE_DATA_PORT);

	restore_flags(flags);
	sti();

}

/*	enable_irq
*		Description: Enable (unmask) the specified IRQ 
*		Author: Hershel
*		Input: irq number
*		Outputs: none
*		Returns: none
*		Side effect: enable the given irq
*/
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
	if(irq_num < 8) {
		master_mask = value;
	}
	else {
		slave_mask = value;
	}
	outb(value, port);
}

/*	disable_irq
*		Description: Disable (mask) the specified IRQ 
*		Author: Hershel
*		Input: the IRQ to mask
*		Output: none
*		Returns: none
*		Side effects: given irq is masked
*/
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
	if(irq_num < 8) {
		master_mask = value;
	}
	else {
		slave_mask = value;
	}
	outb(value, port);
}

/* send_eoi
*		Description: Send end-of-interrupt signal for the specified IRQ 
*		Author: Hershel
*		Input: The irq_number to send the eoi to
*		Output: none
*		Returns: none
*		Side-effect: EOI wirtten to the device
*/
void send_eoi(uint32_t irq_num) {
	if(irq_num >= 8) {
		outb(EOI | (irq_num-8), SLAVE_8259_PORT);
		outb(EOI | ICW3_SLAVE, MASTER_8259_PORT);
	}
	else {
		outb(EOI | irq_num, MASTER_8259_PORT);
	}
}

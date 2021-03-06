/* i8259.c - Functions to interact with the 8259 interrupt controller
* vim:ts=4 noexpandtab
*/

#include "i8259.h"
#include "lib.h"
#include "rtc.h"

#define FULL_MASK				0xFF
#define IGNORE_SLAVE			0x4
#define MAX_IRQ_LINES			8
#define TOTAL_IRQ_LINES 		16

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */
int i;
/* i8259_init
*		Description: Initialize the 8259 PIC
*		Author: Hershel
*		Input: none
*		Output: none
*		Return: none
*		Side effects: pic is enabled
*/
void i8259_init(void) {
	/* Write control word sequence for master PIC. */
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW2_MASTER, MASTER_DATA_PORT);
	outb(ICW3_MASTER, MASTER_DATA_PORT);
	outb(ICW4, MASTER_DATA_PORT);

	/* Write control word sequence for Slave PIC. */
	outb(ICW1, SLAVE_8259_PORT);
	outb(ICW2_SLAVE, SLAVE_DATA_PORT);
	outb(ICW3_SLAVE, SLAVE_DATA_PORT);
	outb(ICW4, SLAVE_DATA_PORT);

	/* Set master mask to mask every IRQ line except IRQ2, which is mapped for Slave PIC. */
	master_mask &= ~(IGNORE_SLAVE);

	/* Re-mask all IRQ lines. */
	outb(master_mask, MASTER_DATA_PORT);
	outb(slave_mask, SLAVE_DATA_PORT);

	/* Disable all potential interrupts from occuring before they are mapped properly. */
	for(i = 0; i < TOTAL_IRQ_LINES; i++) {
		disable_irq(i);
	}

	enable_irq(2); //enable pic
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
	uint16_t port;				/* Store which PIC port interrupt is coming from */
	uint8_t value;				/* Hold data at corresponding IRQ line */

	/* Set port to Master or Slave depending on IRQ line. */
	if(irq_num < MAX_IRQ_LINES) {
		port = MASTER_DATA_PORT;
		outb(0xA, MASTER_8259_PORT);
	}
	else {
		port = SLAVE_DATA_PORT;
		irq_num -= MAX_IRQ_LINES;
		outb(0xB, SLAVE_8259_PORT);
	}
	//Write control word to master from PIC data sheet to master_8259 port
	/* Set specified IRQ line as active low. */

	value = inb(port) & ~(1 << irq_num);

	/* Set mask according to IRQ line. */
	if(irq_num < MAX_IRQ_LINES) {
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
	uint16_t port;				/* Store which PIC port interrupt is coming from */
	uint8_t value;				/* Hold data at corresponding IRQ line */

	/* Set port to Master or Slave depending on IRQ line. */
	if(irq_num < MAX_IRQ_LINES) {
		port = MASTER_DATA_PORT;
		outb(0xA, MASTER_8259_PORT);
	}
	else {
		port = SLAVE_DATA_PORT;
		irq_num -= MAX_IRQ_LINES;
		outb(0xB, SLAVE_8259_PORT);
	}
	//Write control word to master from PIC data sheet to master_8259 port
	/* Set specified IRQ line as active low. */
	value = inb(port) | (1 << irq_num);

	/* Set mask according to IRQ line. */
	if(irq_num < MAX_IRQ_LINES) {
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
	/* For Slave PIC, send EOI  */
	if(irq_num >= MAX_IRQ_LINES) {
		outb(EOI | (irq_num - MAX_IRQ_LINES), SLAVE_8259_PORT);
		outb(EOI | ICW3_SLAVE, MASTER_8259_PORT);
	}
	else {
		outb(EOI | irq_num, MASTER_8259_PORT);
	}
}

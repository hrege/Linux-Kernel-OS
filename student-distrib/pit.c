#include "lib.h"
#include "pit.h"
#include "scheduler.h"
#include "i8259.h"
#include "sys_call.h"

/* Reference: https://en.wikibooks.org/wiki/X86_Assembly/Programmable_Interval_Timer */

/* 
*	pit_init
*		Author: Jonathan
*		Description: Initializtation for the pic. Currently only channel 0 is being used (timer)
*					Channel 0 is initialized to a rate based on the scheduling quantum
*		Input/Return: None
*		Output: Commands to PIT ports
*		Side-effect: PIT IRQ is enabled
*/
extern void pit_init(){
	uint32_t rate = 1000 / (uint32_t)QUANTUM;
	uint16_t x_val = (uint16_t)(CRYSTAL_FREQ / rate);
	uint8_t low = (uint8_t)(0x00FF & x_val);
	uint8_t high = (uint8_t)(x_val >> 8);

	outb(PIT_CONTROL_WORD, PIT_COMMAND); //send control word for channel 0 to PIT
	outb(low, PIT_CHAN_ZERO);
	outb(high, PIT_CHAN_ZERO);
	enable_irq(PIT_IRQ);
}

/* 
*	pit_handler
*		Author: Jonathan
*		Description: Handles pit interrupt by checking the exe_flag, and
					 performing a process switch if it is raised.
*		Input/Return: None
*		Output: Sends EOI or performs process switch
*		Side-effect: Performs process switch
*/
extern void pit_handler(){
	if(!exe_flag){
		send_eoi(PIT_IRQ);
		return;
	}
	process_switch();
}


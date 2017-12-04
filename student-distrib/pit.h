#ifndef PIT_H
#define PIT_H

#define PIT_CHAN_ZERO	0x40
#define PIT_CHAN_ONE	0x41
#define	PIT_CHAN_TWO	0x42
#define PIT_COMMAND		0x43
#define CRYSTAL_FREQ	11931820
#define PIT_CONTROL_WORD	0x36
#define PIT_IRQ			0


/* Function to initialize RTC clock. */
extern void pit_init();
/* Interrupt handler for RTC. */
extern void pit_handler();

#endif

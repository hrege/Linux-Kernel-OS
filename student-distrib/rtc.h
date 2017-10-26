/*
*  Author: Hershel Rege, Group 31 victoriOS Secret
*  Code modified from osDev Internet source.
*/

#ifndef RTC_H
#define RTC_H

#define SLAVE_IRQ       0x02
#define RTC_PORT	      0x70
#define RTC_PORT_CMOS	  (RTC_PORT + 1)
#define	RTC_PIC_IRQ	    8	//RTC is line 8 on the PIC
#define BIT_SIX_ENABLE  0x40

/* RTC Registers */
#define RTC_A_REG	    0x0A
#define	RTC_B_REG	    0x0B
#define RTC_C_REG	    0x0C



/* Function to initialize RTC clock. */
extern void rtc_init();
/* Interrupt handler for RTC. */
extern void rtc_handler();

#endif

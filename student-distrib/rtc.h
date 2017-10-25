/*
*  Author: Hershel Rege, Group 31 victoriOS Secret
*  Code modified from osDev Internet source.
*/

#ifndef RTC_H
#define RTC_H

#define RTC_PORT	0x70
#define RTC_PORT_CMOS	(RTC_PORT + 1)
#define	RTC_PIC_IRQ	8	//RTC is line 8 on the PIC 

/* RTC Registers */
#define RTC_A_REG	x8A
#define	RTC_B_REG	x8B



/* Function to initialize RTC clock. */
extern void rtc_init();

#endif

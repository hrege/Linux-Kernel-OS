/*
*  Author: Hershel Rege, Group 31 victoriOS Secret
*  Code modified from osDev Internet source.
*/

#ifndef RTC_H
#define RTC_H

#define RTC_PORT	0x70
#define RTC_PORT_TWO	(RTC_PORT + 1)

/* Function to initialize RTC clock. */
extern void rtc_init();

#endif

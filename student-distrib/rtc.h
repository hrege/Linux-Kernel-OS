/*
*  Author: Hershel Rege & Jonathan Mullen, Group 31 victoriOS Secret
*/

#ifndef RTC_H
#define RTC_H

#define SLAVE_IRQ       0x02
#define RTC_PORT	      0x70
#define RTC_PORT_CMOS	  (RTC_PORT + 1)
#define	RTC_PIC_IRQ	    8	//RTC is line 8 on the PIC
#define BIT_SIX_ENABLE  0x40

#define MIN_FREQ        2
#define MAX_FREQ        1024

/* RTC Registers */
#define RTC_A_REG	    0x0A
#define	RTC_B_REG	    0x0B
#define RTC_C_REG	    0x0C

/* Function to initialize RTC clock. */
extern void rtc_init();
/* Interrupt handler for RTC. */
extern void rtc_handler();

/* RTC Driver Open Function */
extern int32_t rtc_open(const uint8_t* filename);

/* RTC Driver Read Function */
extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);

/* RTC Driver Write Function */
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t frequency);

/*RTC Write Helpers */
/*Sends info to rtc */
void rtc_output(int8_t i);
/*Handles rate increase variable changes*/
void rate_increase(int new_rate);
/*Handles rate decrease variable changes*/
void rate_decrease(int new_rate);


/* RTC Driver Close Function */
extern int32_t rtc_close(int32_t fd);

#endif

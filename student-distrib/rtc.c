/*
*  Author: Hershel Rege, Group 31 victoriOS Secret
*  Code modified from osDev Internet source.
*/

#include "lib.h"
#include "rtc.h"
#include "i8259.h"

/*
  Description: Initialize RTC clock interrupt.
  Author: Hershel & Jonathan
  Input: none
  Output: Initializes RTC clock registers.
  Return: none
  Side Effects: RTC is initialized.
 */

//filescope variables:
int occurred = 0;  //indicator that the interrupt occurred for read

void rtc_init() {
  printf("Initializing RTC\n");

  /* Initialize registers used for RTC and CMOS. */
  outb(RTC_A_REG, RTC_PORT);  //disable NMI

  /*
     Turn on periodic interrupts through IRQ8 by reading
     from Register B, then set index in RTC Port. re-write
     old value with bit 6 turned on.
   */
  outb(RTC_B_REG, RTC_PORT);
  char prev = inb(RTC_PORT_CMOS);
  outb(RTC_B_REG, RTC_PORT);
  outb((prev | BIT_SIX_ENABLE), RTC_PORT_CMOS);

  /* Enable IRQ line on Slave PIC for RTC. */
  enable_irq(RTC_PIC_IRQ);
}

/*
  Description: Interrupt handler for the RTC.
  Author: Hershel & Jonathan
  Input: none
  Output: Sends End-of-Interrupt signal to both RTC PIC IRQ line
          and Slave IRQ line on the Master PIC.
  Return: none
  Side Effects: Reads/writes from RTC hardware ports.
 */

void rtc_handler() {
  /* Write to C Register of RTC. */
  outb(RTC_C_REG, RTC_PORT);

  /* Read from modified 6-bit CMOS register. */
  inb(RTC_PORT_CMOS);

  /* Send EOI signals to both Slave PIC and Master PIC. */
  send_eoi(RTC_PIC_IRQ);
  send_eoi(SLAVE_IRQ);
  //mark that the interrupt happened
  occurred = 1;
  printf("Hi");
}



///////////////////////////
/*  RTC DRIVER FUNCTIONS */
///////////////////////////

/*
*	rtc_open
*		Author: Jonathan
*		Description: This is the open function for the RTC driver
*						Initializes the RTC frequency to 2Hz
*						Does so by calling RTC write
*		Input: filename (not used - exists to meet open sys call format)
*		Outputs: none
*		Returns: 0 always
*		Side Effects: RTC running at 2Hz
*/
int32_t rtc_open(const uint8_t* filename){
	rtc_write(0, NULL, 2);
	return 0;
}

/*
*	rtc_read
*		Author: Jonathan
*		Description: This is the read function for the RTC driver
*						Blocks until the next RTC interrupt
*		Inputs: fd, buf, nbytes (not used - exists to meet read sys call format)
*		Outputs: none
*		Returns: 0 (always)
*		Side effects: blocks
*/
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
	occurred = 0;
	while(!occurred);
	return 0;

}

/*
*	rtc_write
*		Author: Jonathan
*		Description: This is the write function for the RTC driver
*						Changes the frequency of the RTC
*		Inputs: fd, buf, frequency (only frequency used ... others to meet the sys call format)
*		Outputs: Frequency to RTC
*		Returns: -1 if given invalid frequency
*				 0 else
*		Side Effects: RTC running at new frequency
*/
int32_t rtc_write(int32_t fd, const void* buf, int32_t frequency){
	/* Perform checks to verify the input frequency is in range...
	*	Must be power of 2 from min. 2 to max (allowed to user) of 1024 */
	if(frequency >= 2 && frequency <= 1024){
		int32_t f = 2; //frequency to check against
		int8_t i = 0;	   //power count (minus 1)
		do{
			if(frequency == f){
				/*if valid frequency then write to RTC
				* write to A register of CMOS bottom 4
				* value is !(power-1)  */
				sti();
				outb(RTC_A_REG, RTC_PORT);		//Select A Register
				char prev = inb(RTC_PORT_CMOS); //get old to keep top 4
				outb(RTC_A_REG, RTC_PORT);
				outb((prev & 0xF0)|(!i & 0x0F)	,RTC_PORT_CMOS);
				cli();
				return 0; //return success
			}
			f = f*2;
			i += 1;
		}while(f<=1024);
	}
	return -1; //return fail
}

/*
*	rtc_close
*		Author: Jonathan
*		Description: This is the close function for the RTC driver.
*						Since we are not currently virtualizing it just returns 0
*		Inputs: trash (not used - exixts to meet close sys call format)
*		Outputs: none
*		Returns: 0 always
*		Side effects: none
*/
int32_t rtc_close(int32_t trash){
	return 0;
}


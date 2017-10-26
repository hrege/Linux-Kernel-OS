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

void rtc_init() {
 // cli(); //currently called inside cli
  printf("Initializing RTC\n");
  /* Initialize registers used for RTC and CMOS. */
  outb(RTC_A_REG, RTC_PORT);  //disable NMI

  //outb(0x20, RTC_PORT_CMOS); // Can't write to it before reading it?

  /* Turn on periodic interrupts through IRQ8. */
  outb(RTC_B_REG, RTC_PORT);        //select reg B
  char prev = inb(RTC_PORT_CMOS);   // Read reg B
  outb(RTC_B_REG, RTC_PORT);        //set index
  outb((prev | 0x40), RTC_PORT_CMOS); //re-write old value with bit 6 turned on

  //enable the IRQ address for the rtcuser
  enable_irq(8);
  //sti();
}

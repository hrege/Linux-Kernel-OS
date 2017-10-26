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
}

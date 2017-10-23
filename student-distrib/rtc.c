/*
  Author: Hershel Rege, Group 31 victoriOS Secret
  Code modified from osDev Internet source.
*/

#include "lib.h"
#include "rtc.h"

void rtc_init() {
  /* Initialize registers used for RTC and CMOS. */
  cli();
  outb(RTC_PORT, 0x8A);
  outb(RTC_PORT_TWO, 0x20);

  /* Turn on periodic interrupts through IRQ8. */
  outb(RTC_PORT, 0x8B);
  char prev = inb(RTC_PORT_TWO);
  outb(RTC_PORT, 0x8B);
  outb(RTC_PORT_TWO, prev | 0x40);
  sti();
}

/*
  Author: Hershel Rege, Group 31 victoriOS Secret
  Code modified from osDev Internet source.
*/

#include "lib.h"
#include "rtc.h"

void rtc_init() {
  /* Initialize registers used for RTC and CMOS. */
  cli();
  outb(0x8A, RTC_PORT);
  outb(0x20, RTC_PORT_TWO);

  /* Turn on periodic interrupts through IRQ8. */
  outb(0x8B, RTC_PORT);
  char prev = inb(RTC_PORT_TWO);
  outb(0x8B, RTC_PORT);
  outb((prev | 0x40), RTC_PORT_TWO);
  sti();
}

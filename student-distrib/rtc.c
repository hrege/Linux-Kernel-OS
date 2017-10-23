/*
  Author: Hershel Rege, Group 31 victoriOS Secret
  Code modified from osDev Internet source.
*/

#include "lib.h"

void rtc_init() {
  /* Initialize registers used for RTC and CMOS. */
  cli();
  outb(0x70, 0x8A);
  outb(0x71, 0x20);

  /* Turn on periodic interrupts through IRQ8. */
  outb(0x70, 0x8B);
  char prev = inb(0x71);
  outb(0x70, 0x8B);
  outb(0x71, prev | 0x40);
  sti();
}

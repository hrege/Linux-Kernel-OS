#ifndef KEYBOARD_H
#define KEYBOARD_H

#define NUM_SCANCODES       128
#define NUM_CASES           4
#define KEYBOARD_IRQ        1
#define KEYBOARD_DATA_PORT  0x60

void keyboard_init();
void keyboard_handler();
char getScancode(char input);

#endif

#ifndef KEYBOARD_H
#define KEYBOARD_H

#define NUM_SCANCODES   128
#define NUM_CASES       4

void keyboard_init();
void keyboard_handler();
char getScancode(char input);

#endif

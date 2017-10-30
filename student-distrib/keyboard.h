#ifndef KEYBOARD_H
#define KEYBOARD_H

//#include <stdint.h>

#define NUM_SCANCODES       128
#define NUM_CASES           4
#define KEYBOARD_IRQ        1
#define KEYBOARD_DATA_PORT  0x60
#define max_buffer_size     128
#define MAX_SCANCODE		0x3A

#define BACKSPACE_SCAN     	0xE
#define ENTER         		0x0A
#define CTRL_SCAN    		0x1D
#define LSHIFT_ON_SCAN		0x2A
#define RSHIFT_ON_SCAN		0x36
#define LSHIFT_OFF_SCAN		0xAA
#define RSHIFT_OFF_SCAN		0xB6
#define CAPS_LOCK			0x3A
#define CTRL_RELEASE_SCAN	0x9D		
#define VIDEO       		0xB8000
#define NUM_COLS    		80
#define NUM_ROWS    		25

extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t terminal_open(const uint8_t* filename);
extern int32_t terminal_close(int32_t fd);

void keyboard_init();
void keyboard_handler();
char getScancode(char input);
void update_cursor(int x, int y);


#endif

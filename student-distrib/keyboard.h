#ifndef KEYBOARD_H
#define KEYBOARD_H

//#include <stdint.h>

#define NUM_SCANCODES       128		//for size of scan code array
#define NUM_CASES           4		//cases to handle caps/shift
#define KEYBOARD_IRQ        1		//Keyboard is on IRQ1
#define KEYBOARD_DATA_PORT  0x60	//Port to read keyboard data
#define max_buffer_size     128		//Maximum size of terminal buffer
#define MAX_SCANCODE		0x3A	//Maximum scancode
#define BAD_SCAN_COMMA		0xB3
#define BAD_SCAN_F2 		0xBA
#define BAD_SCAN_CAPS 		0xBC
#define BAD_SCAN_F   		0xA1

//specially handled scancodes
#define BACKSPACE_SCAN     	0xE
#define ENTER         		0x1C
#define L_SCAN 				0x26
#define CTRL_SCAN    		0x1D
#define ALT_SCAN 			0x38
#define LSHIFT_ON_SCAN		0x2A
#define RSHIFT_ON_SCAN		0x36
#define LSHIFT_OFF_SCAN		0xAA
#define RSHIFT_OFF_SCAN		0xB6
#define CAPS_LOCK			0x3A
#define CTRL_RELEASE_SCAN	0x9D
#define ALT_RELEASE_SCAN	0xB8
#define F1_PRESS			0x3B
#define F2_PRESS			0x3C
#define F3_PRESS			0x3D
#define F3_RELEASE			0xBD

//define screen size
#define NUM_COLS    		80
#define NUM_ROWS    		25
#define NUM_TERMS			3

extern int line_start[NUM_TERMS];

extern volatile uint8_t line_buffer[NUM_TERMS][max_buffer_size];
extern volatile int buffer_length[NUM_TERMS];


/* //// external drivers functions \\\\\ */
/* Read for terminal driver */
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
/* Write for terminal driver */
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
/* Open for terminal driver */
extern int32_t terminal_open(const uint8_t* filename);
/* close for terminal driver */
extern int32_t terminal_close(int32_t fd);

/* initializes the keyboard */
void keyboard_init();
/* handles keyboard interrupts */
void keyboard_handler();
/* gets a scancode from the table */
char getScancode(char input);

/* Switches task frame to run new scheduled process. */
void terminal_switch(uint32_t* stored_esp, uint32_t* stored_ebp);

/* Modifies paging to switch to new terminal on keypress. */
void display_new_terminal(int new_terminal);

#endif

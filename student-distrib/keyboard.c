/*
 *Terminal driver and expanded keyboard handler written by Sam Morris 
 */


#include "i8259.h"
#include "lib.h"
#include "keyboard.h"


static volatile uint8_t line_buffer[max_buffer_size];
static volatile int buffer_length;
static volatile int flag; //0 to sleep termianl_read() positive number for bytes to copy 
static int rshift_flag;
static int lshift_flag;
static int caps_flag;
static int ctrl_flag;


/*
   Scancode array to convert all scancode values
   given by keyboard hardware into alphanumeric
   and special keys. Each Scan Code has 4 cases:
   0. Only that key - 1: Shift is pressed
   2: Caps is on  	- 3: CAPS on & Shift pressed
 */
char scancode_map[NUM_SCANCODES][NUM_CASES] = {
      { 0x00, 0x00, 0x00, 0x00 }, /* Nothing */
      { 0x00, 0x00, 0x00, 0x00 }, /* ESC */ // 0x1B to restore

      /* Line 1 on keyboard: Numbers 1-9, 0, -, +/=, Backspace */
      { 0x31, 0x21, 0x31, 0x21 }, /* 1 */
      { 0x32, 0x40, 0x32, 0x40 }, /* 2 */
      { 0x33, 0x23, 0x33, 0x23 }, /* 3 */
      { 0x34, 0x24, 0x34, 0x24 }, /* 4 */
      { 0x35, 0x25, 0x35, 0x25 }, /* 5 */
      { 0x36, 0x5E, 0x36, 0x5E }, /* 6 */
      { 0x37, 0x26, 0x37, 0x26 }, /* 7 */
      { 0x38, 0x2A, 0x38, 0x2A }, /* 8 */
      { 0x39, 0x28, 0x39, 0x28 }, /* 9 */
      { 0x30, 0x29, 0x30, 0x29 }, /* 0 */
      { 0x2D, 0x5F, 0x2D, 0x5F }, /* - */
      { 0x3D, 0x2B, 0x3D, 0x2B }, /* +/= */
      { 0x08, 0x08, 0x08, 0x08 }, /* Backspace */

      /* Line 2 on keyboard: Tab, QWERTYUIOP, [] */
      { 0x00, 0x00, 0x00, 0x00 }, /* Tab */ // 0x09 if we want to restore
      { 0x71, 0x51, 0x51, 0x71 }, /* Q */
      { 0x77, 0x57, 0x57, 0x77 }, /* W */
      { 0x65, 0x45, 0x45, 0x65 }, /* E */
      { 0x72, 0x52, 0x52, 0x72 }, /* R */
      { 0x74, 0x54, 0x54, 0x74 }, /* T */
      { 0x79, 0x59, 0x59, 0x79 }, /* Y */
      { 0x75, 0x55, 0x55, 0x75 }, /* U */
      { 0x69, 0x49, 0x49, 0x69 }, /* I */
      { 0x6F, 0x4F, 0x4F, 0x6F }, /* O */
      { 0x70, 0x50, 0x50, 0x70 }, /* P */
      { 0x5B, 0x7B, 0x5B, 0x7B }, /* [ */
      { 0x5D, 0x7D, 0x5D, 0x7D }, /* ] */

      /* Enter and Control */
      { 0x0A, 0x0A, 0x0A, 0x0A }, /* ENTER */
      { 0x00, 0x00, 0x00, 0x00 }, /* LCTRL */

      /* Line 3 on keyboard: ASDFGHJKL;' and `(tilde) */
      { 0x61, 0x41, 0x41, 0x61 }, /* A */
      { 0x73, 0x53, 0x53, 0x73 }, /* S */
      { 0x64, 0x44, 0x44, 0x64 }, /* D */
      { 0x66, 0x46, 0x46, 0x66 }, /* F */
      { 0x67, 0x47, 0x47, 0x67 }, /* G */
      { 0x68, 0x48, 0x48, 0x68 }, /* H */
      { 0x6A, 0x4A, 0x4A, 0x6A }, /* J */
      { 0x6B, 0x4B, 0x4B, 0x6B }, /* K */
      { 0x6C, 0x4C, 0x4C, 0x6C }, /* L */
      { 0x3B, 0x3A, 0x3B, 0x3A }, /* ; */
      { 0x27, 0x22, 0x27, 0x22 }, /* ' */
      { 0x60, 0x7E, 0x60, 0x7E }, /* ` */

      /* Line 4 on keyboard: Shift, \, ZXCVBNM,./ and Shift */
      { 0x00, 0x00, 0x00, 0x00 }, /* LSHIFT */
      { 0x5C, 0x7C, 0x5C, 0x7C }, /* \ */
      { 0x7A, 0x5A, 0x5A, 0x7A }, /* Z */
      { 0x78, 0x58, 0x58, 0x78 }, /* X */
      { 0x63, 0x43, 0x43, 0x63 }, /* C */
      { 0x76, 0x56, 0x56, 0x76 }, /* V */
      { 0x62, 0x42, 0x42, 0x62 }, /* B */
      { 0x6E, 0x4E, 0x4E, 0x6E }, /* N */
      { 0x6D, 0x4D, 0x4D, 0x6D }, /* M */
      { 0x2C, 0x3C, 0x2C, 0x3C }, /* , */
      { 0x2E, 0x3E, 0x2E, 0x3E }, /* . */
      { 0x2F, 0x3F, 0x2F, 0x3F }, /* / */
      { 0x00, 0x00, 0x00, 0x00 }, /* RSHIFT */

      /* Special Keys: Print Screen, Alt, Space, Caps */
      { 0x00, 0x00, 0x00, 0x00 }, /* PRTSC */
      { 0x00, 0x00, 0x00, 0x00 }, /* LALT */
      { 0x20, 0x20, 0x20, 0x20 }, /* SPACE */
      { 0x00, 0x00, 0x00, 0x00 }, /* CAPS */

      /* Function keys (1-10), NumLock, ScrollLock, NumberPad() all set to 0 */
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },

      /* The rest of the scancodes are not used */
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00 }

};

/*
    Description: Initializes keyboard interrupts on Master Pic IRQ1.
    Author: Hershel
    Inputs: none
    Outputs: none
    Return Value: none
    Side Effects: none
 */
void keyboard_init() {
  terminal_open((uint8_t*)1);
  enable_irq(KEYBOARD_IRQ);


}

/*
    Description: Interrupt handler to deal with keypresses.
    Author: Hershel & Sam & Jonathan
    Inputs: none
    Outputs: Prints the character typed by the keyboard.
    Return Value: none
    Side Effects: Sends EOI signal to Master PIC.
 */
void keyboard_handler() {
  uint8_t keyboard_input = 0;
  char char_out = 0;
  /* Read keypress from keyboard data port. */
  keyboard_input = inb(KEYBOARD_DATA_PORT);
  char_out = getScancode(keyboard_input);

 if(keyboard_input == LSHIFT_ON_SCAN){
        lshift_flag = 1;
  }
 else if(keyboard_input == RSHIFT_ON_SCAN){
        rshift_flag = 1;
  }
  else if(keyboard_input == LSHIFT_OFF_SCAN){
        lshift_flag = 0;
  }
  else if(keyboard_input == RSHIFT_OFF_SCAN){
        rshift_flag = 0;
  }
  else if(keyboard_input == CAPS_LOCK){
        caps_flag = (!caps_flag) & 0x1;
  }
  else if(keyboard_input == CTRL_SCAN){
    //Clear screen and start printing from top
      ctrl_flag++;

  }
  else if(keyboard_input == CTRL_RELEASE_SCAN){
    //Clear screen and start printing from top
      ctrl_flag--;

  }
  else if(keyboard_input == BACKSPACE_SCAN){
      if(buffer_length > 0){
        buffer_length--;
      }
      else{
        set_screen_x(0);
        send_eoi(KEYBOARD_IRQ);
        return;
      }
      /*Go back to previous video memory*/
      if(get_screen_x() == 0 && buffer_length > 0){
            set_screen_y(get_screen_y() - 1);
            set_screen_x(NUM_COLS - 1);
      }
      else{
          set_screen_x(get_screen_x() - 1);
      }
      /*Previous character erased*/
      putc(' ');
      /*Reset screen position to write next character over space*/
      if(get_screen_x() == 0 && buffer_length > 0){
        set_screen_y(get_screen_y() - 1);
        set_screen_x(NUM_COLS - 1);
        update_cursor(get_screen_x(), get_screen_y());
      }
      else{
          set_screen_x(get_screen_x() - 1);
          update_cursor(get_screen_x(), get_screen_y());
      }


      //backspace
  }
  /*Clear screen*/
  else if(char_out == 'l' && ctrl_flag > 0){
      clear();
      set_screen_x(0);
      set_screen_y(0);
      update_cursor(get_screen_x(), get_screen_y());
      buffer_length = 0;

  }

  else if(char_out == ENTER){
    line_buffer[buffer_length] = '\n';
    putc('\n');
    flag = buffer_length;
    update_cursor(get_screen_x(), get_screen_y());
    buffer_length = 0;


  }



  /* Print scancode-converted character to terminal. */
  else if(keyboard_input < MAX_SCANCODE && buffer_length < max_buffer_size && char_out != 0) {
    putc(char_out);
    line_buffer[buffer_length] = char_out;
    buffer_length++;
    update_cursor(get_screen_x(), get_screen_y());
  }

  /*Vertical Scrolling condition*/
  if (get_screen_y() == NUM_ROWS)
  {
     memcpy(get_video_mem(), get_video_mem() + (NUM_COLS << 1), (((NUM_ROWS-1)*NUM_COLS) << 1));
     set_screen_y(NUM_ROWS - 1);
     clear_line();
     update_cursor(get_screen_x(), get_screen_y());
   }

  


  /* Send End-of-Interrupt signal to Master PIC. */
  send_eoi(KEYBOARD_IRQ);
}

/*
    Description: Convert scancode to character to print to screen.
    Author: Hershel
    Inputs: input - scancode delivered by keyboard hardware.
    Outputs: none
    Return Value: Converted alphanumeric character to print to screen.
    Side Effects: Accesses scancode array to determine which character is being printed.
 */
char getScancode(char input) {


  return scancode_map[(int)input][(lshift_flag | rshift_flag | (caps_flag << 1))];
}

/*
*	terminal_read
*		Author: Sam
*		Description: Read function for terminal driver. Reads the specified nbytes from the buffer
*		Input: standard read inputs: fd, buf, nbytes
*		Output: nada
*		Returns: the number of bytes read if pass; -1 if fail
*/


int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
  int i;//loop variable

  //Check input validity
  if(nbytes < 1){
    return -1;
  } 
  if(buf == NULL){
    return -1;
  }
  if(nbytes > max_buffer_size){
    nbytes = max_buffer_size;
  }

  //wait for flag
  while(flag == 0);
  /*Check size of buffer*/
  if(nbytes < flag){
    flag = nbytes;
  }
  /* read */
  for(i = 0; i < flag; i++){
      ((char *)buf)[i] = line_buffer[i];
  }
  ((char *)buf)[flag] = '\0';
  nbytes = flag;	//set return value
  /* reset flag */
  flag = 0;

  return nbytes;
}

/*
*	terminal_write
*		Author: Sam
*		Description: writes to the terminal
*		Inputs: Standard write - fd, buf, nbytes
*		Outpus: buf contents to screen
*		Returns: 0 = Pass; -1 = Fail
*/
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    int i;//loop vairable
    /* Validate input */
    if(nbytes < 0){
      return -1;
    }
    if(buf == NULL){
        return -1;
    }
    if(nbytes > max_buffer_size){
      nbytes = max_buffer_size;
    }

    /* read from buf */
    for(i = 0; i < nbytes; i++){
      if(((char*)buf)[i] == '\0'){
        return 0;		//return 0 if end of string
      }
      if(((char *)buf)[i] == ENTER){
         putc('\n');	//enter is newline
      }
      else if(((char *)buf)[i] != 0) {
        putc(((char *)buf)[i]);	//otherwise print the char and put it in line buffer
        line_buffer[buffer_length] = ((char *)buf)[i];
      }  

      /*Take care of scrolling when screen_y reaches maximum*/
      if (get_screen_y() == NUM_ROWS){
         memcpy(get_video_mem(), get_video_mem() + (NUM_COLS << 1), (((NUM_ROWS-1)*NUM_COLS) << 1));
         set_screen_y(NUM_ROWS - 1);
         clear_line();
         buffer_length = 0;
        }
     
    }
    update_cursor(get_screen_x(), get_screen_y());

    return 0;
}

/* 
*	terminal_open
*		Author: Sam
*		Description: Doesn't do much at this point since multiple terminals isn't enabled
*						-just initializes some vars
*		Inputs: filename (not used)as is standard for open
*		Outputs: Nada
*		Returns: 0 always atm
*/
int32_t terminal_open(const uint8_t* filename){
  buffer_length = 0;
  set_screen_x(0);
  set_screen_y(0);
  clear();
  update_cursor(get_screen_x(), get_screen_y());
  /*Initialize all flags*/
  lshift_flag = 0;
  rshift_flag = 0;
  caps_flag = 0;
  ctrl_flag = 0;
  return 0;
}

/* 
*	terminal_close
*		Author: collaborative project between all 4 team members
*		Description: does nothing now since multiple terminals are not implemented
*		Inputs: fd as is required for close. Not used
*		Outputs: none
*		Returns: 0 always 
*/
int32_t terminal_close(int32_t fd){
//Do Nothing
  return 0;
}










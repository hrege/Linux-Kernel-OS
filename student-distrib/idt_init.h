//Definisitons
#ifndef IDT_INIT
#define IDT_INIT

#ifndef ASM


#define IDT_SIZE                  256
#define NUM_EXCEPTIONS            20
#define SYSTEM_CALL_IDT_ENTRY     0x80
#define PIT_IDT_ENTRY			  0x20
#define KEYBOARD_IDT_ENTRY        0x21
#define RTC_IDT_ENTRY             0x28
#define USER_PROTECTION           3




/* Main function to initialize the idt */
extern void idt_init();

/* Handler for sys calls at x80 */
void sys_call();

/* Keyboard interrupt handler */
void get_char();

/* RTC Handler */
void rtc_int();

/*PIT Handling*/
void pit_hlp();

#endif
#endif /* IDT_INIT */

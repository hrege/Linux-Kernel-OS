//Definisitons
#ifndef IDT_INIT
#define IDT_INIT

#ifndef ASM

#define IDT_SIZE                  256
#define NUM_EXCEPTIONS            20
#define SYSTEM_CALL_IDT_ENTRY     0x80
#define KEYBOARD_IDT_ENTRY        0x21
#define RTC_IDT_ENTRY             0x28
#define USER_PROTECTION           3

/* Main function to initialize the idt */
extern void idt_init();

/* The exception handlers. Currently all just print what they are
 *  i.e. "This is double fault" */
void divide_by_zero();
void debug();
void nmi_interrupt();
void breakpoint();
void overflow();
void bound_range_exceeded();
void invalid_opcode();
void device_not_available();
void double_fault();
void coprocessor_segment_overrun();
void invalid_tss();
void segment_not_present();
void stack_segment();
void general_protection();
void page_fault();
void assertion_fail();
void fpu_floating_point_exception();
void alignment_check();
void machine_check();
void simd_floating_point_exception();

/* Handler for sys calls at x80 */
void sys_call();

/* Keyboard interrupt handler */
void get_char();

/* RTC Handler */
void rtc_int();

#endif
#endif /* IDT_INIT */

##ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

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
void stack_segement_fault();
void general_protection();
void page_fault();
void fpu_floating_point_exception();
void alignment_check();
void machine_check();
void simd_floating_point_exception();

#endif
#ifndef SCHEDULER_H
#define SCHEDULER_H

#define SHELL_INIT_DONE 2   // End state for shell check variables
#define QUANTUM         35  // Milliseconds for rate initialization

extern int8_t visible_process;

//scheduling algorithm
void get_next_process();

//extern void schedule_init();
extern void schedule_init();
extern void process_switch();
#endif

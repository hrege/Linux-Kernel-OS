#ifndef SCHEDULER_H
#define SCHEDULER_H

#define SHELL_INIT_DONE 2   // End state for shell check variables

extern int8_t visible_process;

//extern void schedule_init();
extern void schedule_init();
extern void process_switch();
#endif

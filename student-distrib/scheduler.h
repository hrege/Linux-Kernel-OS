#ifndef SCHEDULER_H
#define SCHEDULER_H


extern int8_t visible_process;

//extern void schedule_init();
extern void schedule_init();
extern void process_switch(int curr_process);
#endif

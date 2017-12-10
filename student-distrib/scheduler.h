#ifndef SCHEDULER_H
#define SCHEDULER_H


extern int8_t active_process;
extern int8_t visible_process;

extern void schedule_init();

extern void scheduler();

uint8_t next_process(int8_t current);

#endif

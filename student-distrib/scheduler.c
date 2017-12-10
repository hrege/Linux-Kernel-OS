#include "lib.h"
#include "scheduler.h"
#include "x86_desc.h"
#include "sys_call.h"
#include "pit.h"
#include "i8259.h"
#include "keyboard.h"
#include "paging.h"


int8_t visible_process;

/* The schedule works as a round robin scheduler meaning if you had three 
processes open then whole time you'd do 1 -> 2 -> 3 -> 1 -> 2 -> 3 -> 1 ...
The scheduler allocated a certain length of time before switching.
Each time the PIT interrupt is triggered:
1st store needed data about where you are coming from. 
2nd run the scheduling "algorithm" incase things have been opened or closed or one task was closed and returned you to another etc. 
3rd switch tasks */

void schedule_init(){
	visible_process = 0;
  active_term = 0;
}


void process_switch(int curr_process) {
      uint8_t* ptr = (uint8_t*)("shell");

      PCB_t* curr_pcb;
      PCB_t* dest_pcb;
      curr_pcb = get_pcb();

	  active_term = (uint8_t)((active_term + 1) % 3);

      dest_pcb = (PCB_t*)((uint32_t)(EIGHT_MB - STACK_ROW_SIZE - (EIGHT_KB * active_term)) & 0xFFFFE000);

      while(dest_pcb->child_process){
        dest_pcb = dest_pcb->child_process;
      }

      paging_switch(128, 4 * (dest_pcb->process_id + 2));

      asm volatile("movl %%esp, %0;"
        "movl %%ebp, %1;"
        : "=m"(curr_pcb->kern_esp_context), "=m"(curr_pcb->kern_ebp_context)
        :
        : "eax"
      );

      if((active_term == 1) && (shell_2 < 2)) {
	      shell_2++;
      }
      if((active_term == 2) && (shell_3 < 2)) {
      	shell_3++;
      }

      if(shell_2 == 1){ 
        shell_2++;
        tss.esp0 = ((uint32_t)(EIGHT_MB - STACK_ROW_SIZE - (EIGHT_KB)));
        tss.ss0 = KERNEL_DS;

        asm volatile("movl %%esp, %0;"
          "movl %%ebp, %1;"
          : "=m"(curr_pcb->kern_esp_context), "=m"(curr_pcb->kern_ebp_context)
          :
          : "eax"
        );

        send_eoi(PIT_IRQ);
        clear();
        sys_execute(ptr);
        return;
      }

      if(shell_3 == 1){ 
        shell_3++;
        tss.esp0 = ((uint32_t)(EIGHT_MB - STACK_ROW_SIZE - (EIGHT_KB*2)));
        tss.ss0 = KERNEL_DS;

        asm volatile("movl %%esp, %0;"
          "movl %%ebp, %1;"
          : "=m"(curr_pcb->kern_esp_context), "=m"(curr_pcb->kern_ebp_context)
          :
          : "eax"
        );

        send_eoi(PIT_IRQ);
        clear();
        sys_execute(ptr);
        return;
      }

      send_eoi(PIT_IRQ);
      terminal_switch(dest_pcb->kern_esp_context, dest_pcb->kern_ebp_context);
}





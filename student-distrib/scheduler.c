#include "lib.h"
#include "scheduler.h"
#include "x86_desc.h"
#include "sys_call.h"
#include "pit.h"
#include "i8259.h"
#include "keyboard.h"
#include "paging.h"


int8_t visible_process;

/*
 *	process_switch
 *		Description: Initializes the external visible_process 
                   and active_term variables.
 *		Author: Sam, Jonathan, Hershel, Austin
 *		Inputs: None
 *		Outputs: None
 *		Side effect: visible_process and active_term are set to 0.
 */
void schedule_init(){
	visible_process = 0;
  active_term = 0;
}

/* get_next_process
*		Author: Jonathan
*		Description: the scheduling algorithm - modified round robin
*					-only switches to a terminal if it is running something other than a shell,
*						it is visibile, or in the case it isn't open yet - then it opens it
*		Input: none
*		Output: none
*		Returns: none
*		Side-effect: active_term changed
*/				   

void get_next_process(){
	int8_t term = active_term;
	do{
		term = (term + 1) % 3;
		//switch to process if it is not open or it is running something other than shell or it is visible
		if((!check_pid(term)) || (non_shell(term)) || visible_process == term){
			active_term = term;
			return;
		}
	}while(term != active_term);
	return;
}

/*
 *	process_switch
 *		Description: The function called by pit_handler to perform an active terminal switch
                   when the PIT interrupt is triggered.
                   Active terminal is scheduled round-robin, so it goes 1 -> 2 -> 3 -> 1 -> ...
                   First, data about previous terminal is stored. Then the scheduler
                   checks if the second and third shells for terminals 2 and 3 are executed.
                   Finally, performs the task switch.
 *		Author: Sam, Jonathan, Hershel
 *		Inputs: None
 *		Outputs: None
 *		Side effect: Performs a paging switch and terminal (context) switch
 */
void process_switch() {
      uint8_t* ptr = (uint8_t*)("shell");

      PCB_t* curr_pcb;
      PCB_t* dest_pcb;
      curr_pcb = get_pcb();

      get_next_process();

      dest_pcb = (PCB_t*)((uint32_t)(EIGHT_MB - STACK_ROW_SIZE - (EIGHT_KB * active_term)) & 0xFFFFE000);

      while(dest_pcb->child_process){
        dest_pcb = dest_pcb->child_process;
      }

      //paging set_up. Arg 2 - phys address - calculated by adding two mult *4. 
      //Mult by 4 because need 4 mb for each. Add two so id 0 is at 8 which is first available locations
      paging_switch(USER_PROG_VM, 4 * (dest_pcb->process_id + 2));

      asm volatile("movl %%esp, %0;"
        "movl %%ebp, %1;"
        : "=m"(curr_pcb->kern_esp_context), "=m"(curr_pcb->kern_ebp_context)
        :
        : "eax"
      );

      if((active_term == 1) && (shell_2 < SHELL_INIT_DONE)) {
	      shell_2++;
      }
      if((active_term == 2) && (shell_3 < SHELL_INIT_DONE)) {
      	shell_3++;
      }

      if(shell_2 == 1 && active_term == 1){ 
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

      if(shell_3 == 1 && active_term == 2){ 
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





#include "lib.h"
#include "scheduler.h"
#include "pit.h"


/* The schedule works as a round robin scheduler meaning if you had three 
processes open then whole time you'd do 1 -> 2 -> 3 -> 1 -> 2 -> 3 -> 1 ...
The scheduler allocated a certian length of time before switching.
Each time the PIT interrupt is triggered 1st store needed data abotu where
you are coming from. 2nd run the scheduling "algorithm" incase things have
been opened or close or one task was closed and returned you to another etc. 
Third switch tasks */

void scheduler(){
	//get current process number
	int8_t process = 5;
	//if not valid (i.e. if gets called while first process not yet started) return
	if(process < 0 || process > 2){
		return;
	}
	//get next process
	process = next_process(process);
	//return if no other processes to run
	if(process == -1){
		return;
	}

}

/*
*	next_process
*		Author: Jonathan
*		Description: Gets the next process to run
*		Input: Current process(terminal) number
*		Returns: next process(terminal) number 
*				of -1 if no other processes to run
*/
uint8_t next_process(int8_t current){
	int8_t next = (current + 1) % 3;
	do{
		if(next - is active){
			return next;
		}
		next = (next + 1) % 3;
	}while(next != current);
	return -1;
}

void process_switch(){

	//should we handle video memory here?


	//update tss as approriate for where we are moving to

	//could just call context switch?



}





/*
*  Author: Hershel Rege & Jonathan Mullen, Group 31 victoriOS Secret
*    Some code inspired by OSDev internet source and modified to our use case
*/

#include "lib.h"
#include "rtc.h"
#include "i8259.h"
#include "filesystem.h"
#include "paging.h"
#include "x86_desc.h"

//filescope variables:


//rates for each terminal
int rate[] = {0,0,0};
//inverse power rate for each (see rtc write for math)
int8_t inverse[] = {0,0,0};
//rate it is set at
int set_rate = 0; 
//inverse i used to set
int8_t set_i = 0;
//counter for each terminal
uint32_t count[] = {0,0,0};
//counter target value for each
uint32_t target[] = {0,0,0}; 


/* rtc_init
*   Description: Initialize RTC clock interrupt.
*   Author: Hershel & Jonathan
*   Input: none
*   Output: Initializes RTC clock registers.
*   Return: none
*   Side Effects: RTC is initialized.
 */
void rtc_init() {
  printf("Initializing RTC\n");

  /* Initialize registers used for RTC and CMOS. */
  outb(RTC_A_REG, RTC_PORT);  //disable NMI

  /*
     Turn on periodic interrupts through IRQ8 by reading
     from Register B, then set index in RTC Port. re-write
     old value with bit 6 turned on.
   */
  outb(RTC_B_REG, RTC_PORT);
  char prev = inb(RTC_PORT_CMOS);
  outb(RTC_B_REG, RTC_PORT);
  outb((prev | BIT_SIX_ENABLE), RTC_PORT_CMOS);
  /* Enable IRQ line for RTC. */
  enable_irq(RTC_PIC_IRQ);

}

/*
* rtc_handler
*    Description: Interrupt handler for the RTC.
*    Author: Hershel & Jonathan
*    Input: none
*    Output: Sends End-of-Interrupt signal to both RTC PIC IRQ line
*            and Slave IRQ line on the Master PIC.
*    Return: none
*    Side Effects: Reads/writes from RTC hardware ports.
 */

void rtc_handler() {
  /* Write to C Register of RTC. */
  outb(RTC_C_REG, RTC_PORT);

  /* Read from modified 6-bit CMOS register. */
  inb(RTC_PORT_CMOS);

  /* Send EOI signals to both Slave PIC and Master PIC. */
  send_eoi(RTC_PIC_IRQ);
  //increment counters
  int i;
  for(i=0; i<NUM_TERMS; i++){
  	count[i]++;
  }

  return;
}



///////////////////////////
/*  RTC DRIVER FUNCTIONS */
///////////////////////////

/*
*	rtc_open
*		Author: Jonathan
*		Description: This is the open function for the RTC driver
*						Initializes the RTC frequency to 2Hz
*						Does so by calling RTC write
*		Input: filename (not used - exists to meet open sys call format)
*		Outputs: none
*		Returns: 0 always
*		Side Effects: RTC running at 2Hz
*/
int32_t rtc_open(const uint8_t* filename){

	int freq = 2;
	rtc_write(0, &freq, 0);  //call rtc_write with freq of 2Hz
	rate[active_term] = freq;
	return 0;
}

/*
*	rtc_read
*		Author: Jonathan
*		Description: This is the read function for the RTC driver
*						Blocks until the next RTC interrupt
*		Inputs: fd, buf, nbytes (not used - exists to meet read sys call format)
*		Outputs: none
*		Returns: 0 (always)
*		Side effects: blocks
*/
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){ 
	sti();
	while(count[active_term]<target[active_term]);  //wait for interrupt to occur
	cli();
	count[active_term] = 0;
	return 0;

}

/*
*	rtc_write
*		Author: Jonathan
*		Description: This is the write function for the RTC driver
*						Changes the frequency of the RTC
*		Inputs: fd, buf, nbytes (only buf used ... others to meet the sys call format)
*		Outputs: Frequency to RTC
*		Returns: -1 if given invalid frequency
*				 0 else
*		Side Effects: RTC running at new frequency
*/
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
	/* Perform checks to verify the input frequency is in range...
	Must be power of 2 from min. 2 to max (allowed to user) of 1024 */
	if(buf == NULL){
		return -1;
	}
	int32_t frequency = *((uint32_t *)buf);
	if((frequency >= MIN_FREQ) && frequency <= MAX_FREQ){
		int f = 2; //frequency to check against
		int8_t i = 14;	   //inverse power counter (minus 1)
		do{
			//if called with 
			if(frequency == f){
				/*if valid frequency input then write maximum
				* frequency to rtc if not already at that rate
				* write to A register of CMOS bottom 4
				* value is !(power-1)  */
				rate[active_term] = f;
				inverse[active_term] = i;

				if(f > set_rate){
					rate_increase(f);
					rtc_output(i);
				}
				else{
					//initialize counter and target
					count[active_term] = 0;
					target[active_term] = set_rate / f;
				}
				return 0; //return success
			}
			f = f*2;
			i = i - 1;
		}while(f<=1024);
	}
	return -1; //return fail
}

/* rtc_output
*		Author: Jonathan
*		Description: does the output to the rtc
*		Inputs:  inverse power for setting corresponding to rate
*		Outputs: to RTC ports
*		Return: none
*		Side-effect: RTC rate changed
*/
void rtc_output(int8_t i){
	set_i = i;
	cli();//to be safe
	outb(RTC_A_REG, RTC_PORT);		//Select A Register
	char prev = inb(RTC_PORT_CMOS); //get old to keep top 4
	outb(RTC_A_REG, RTC_PORT);
	outb((prev & 0xF0)|(i & 0x0F), RTC_PORT_CMOS); //keep same upper 4 put freq value into lower 4
	return;
}

/* rate_increase
*		Author: Jonathan
*		Description: updates the rate and the counter values to match if new_rate is larger (from write)
*		Inputs: rate switching to
*		Outputs: none
*		Return: none
*		Side-effect: RTC counters & targets adjusted
*/
void rate_increase(int new_rate){
	int i;
	//case where all closed
	if(set_rate != 0){
		int mult = new_rate / set_rate;  
		for(i = 0; i<NUM_TERMS; i++){
			if(i!=active_term){
				count[i] *= mult;
				target[i] *= mult;
			}
		}
	}
	count[active_term] = 0;
	target[active_term] = 1;
	set_rate = new_rate;
	return;
}
/* rate_decrease
*		Author: Jonathan
*		Description: updates the rate and the counter values to match when new_rate is smaller (from close)
*		Inputs: rate switching to
*		Outputs: none
*		Return: none
*		Side-effect: RTC counters & targets adjusted
*/
void rate_decrease(int new_rate){
	int i;
	int div = set_rate / new_rate;
	for(i = 0; i<NUM_TERMS; i++){
		if(i!=active_term){
			count[i] /= div;
			target[i] /= div;
		}
	}
	set_rate = new_rate;
	return;
}
/*
*	rtc_close
*		Author: Jonathan
*		Description: This is the close function for the RTC driver.
*						Since we are not currently virtualizing it just returns 0
*		Inputs: trash (not used - exixts to meet close sys call format)
*		Outputs: none
*		Returns: 0 always
*		Side effects: none
*/
int32_t rtc_close(int32_t fd){
	rate[active_term] = 0;
	inverse[active_term] = 0;
	int max_rate = 0;
	int8_t i = 0;
	int j;
	for(j=0; j<NUM_TERMS; j++){
		if(rate[j]>max_rate){
			max_rate = rate[j];
			i = inverse[j];
		}
	}
	
	//if all closed set rtc to slowest rate to reduce interrupts
	if(max_rate == 0){
		int freq = 2;
		rtc_write(0, &freq, 0);  //call rtc_write with freq of 2Hz
	}
	//if the rate is changing
	else if(max_rate != set_rate){
		rate_decrease(max_rate);
		rtc_output(i);
	}

  PCB_t* curr_pcb = get_pcb();
  curr_pcb->file_array[fd].flags = 0;
  curr_pcb->file_array[fd].file_position = 0;

	return 0;
}


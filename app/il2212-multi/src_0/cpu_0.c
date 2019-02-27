//#include <stdio.h>
//#include "system.h"
#include "altera_avalon_pio_regs.h"
//#include "io.h"
#include "sys/alt_stdio.h"
#include <altera_avalon_mutex.h>

extern void delay (int millisec);
char count = 0;




int main()
{
	alt_putstr("Hello cpu_0!\n");

	alt_mutex_dev* mutex_0 = altera_avalon_mutex_open( "/dev/mutex_0" );
	alt_mutex_dev* mutex_1 = altera_avalon_mutex_open( "/dev/mutex_1" );
	alt_mutex_dev* mutex_2 = altera_avalon_mutex_open( "/dev/mutex_2" );
	alt_mutex_dev* mutex_3 = altera_avalon_mutex_open( "/dev/mutex_3" );
	alt_mutex_dev* mutex_4 = altera_avalon_mutex_open( "/dev/mutex_4" );
	
	
	altera_avalon_mutex_trylock(mutex_0, 1);
	altera_avalon_mutex_trylock(mutex_1, 1);
	altera_avalon_mutex_trylock(mutex_2, 1);
	altera_avalon_mutex_trylock(mutex_3, 1);
	altera_avalon_mutex_trylock(mutex_4, 1);
	
	
	while (1) {
		
		
		/*
		 * TODO: cropping function
		 * */
		alt_putstr("cpu_0 cropping\n");
		delay(100);	//simulating cropping time
		
		
		
		/*
		 * TODO: write image to shared memory
		 * */
		alt_putstr("cpu_0 writing image to memory\n");
		delay(500);	//simulating writing time
		
		
		
		/*
		 * schedule reading from memory
		 * */
		alt_putstr("cpu_0 scheduling reading\n");
		
		if(altera_avalon_mutex_is_mine(mutex_1)) {
			alt_putstr("cpu_0 giving control to cpu 1\n");
			altera_avalon_mutex_unlock(mutex_1);
			delay(1);
			altera_avalon_mutex_lock(mutex_1, 1);
		} else {
			alt_putstr("cpu_0 cant open mutex 1 because does not own mutex 1\n");
		}
		
		if(altera_avalon_mutex_is_mine(mutex_2)) {
			alt_putstr("cpu_0 giving control to cpu 2\n");
			altera_avalon_mutex_unlock(mutex_2);
			delay(1);
			altera_avalon_mutex_lock(mutex_2, 1);
		} else {
			alt_putstr("cpu_0 cant open mutex 2 because does not own mutex 2\n");
		}
		
		if(altera_avalon_mutex_is_mine(mutex_3)) {
			alt_putstr("cpu_0 giving control to cpu 3\n");
			altera_avalon_mutex_unlock(mutex_3);
			delay(1);
			altera_avalon_mutex_lock(mutex_3, 1);
		} else {
			alt_putstr("cpu_0 cant open mutex 3 because does not own mutex 3\n");
		}
		
		if(altera_avalon_mutex_is_mine(mutex_4)) {
			alt_putstr("cpu_0 giving control to cpu 4\n");
			altera_avalon_mutex_unlock(mutex_4);
			delay(1);
			altera_avalon_mutex_lock(mutex_4, 1);
		} else {
			alt_putstr("cpu_0 cant open mutex 4 because does not own mutex 4\n");
		}
		
		
		/*
		 * let all cpus start computation at the same time.
		 * it is okay to have cpus wait for others to finish reading first
		 * because cpu0 will still need to synchronise 
		 * with all the results later anyways.
		 * */
		alt_putstr("cpu_0 grants permission to compute to all cpus\n");
		altera_avalon_mutex_unlock(mutex_1);
		altera_avalon_mutex_unlock(mutex_2);
		altera_avalon_mutex_unlock(mutex_3);
		altera_avalon_mutex_unlock(mutex_4);
		
		
		/*
		 * wait until other cpu's finish computing
		 * */
		alt_putstr("cpu_0 waiting for other cpus to finish computing\n");
		
		while(
			!altera_avalon_mutex_is_mine(mutex_1) ||
			!altera_avalon_mutex_is_mine(mutex_2) ||
			!altera_avalon_mutex_is_mine(mutex_3) ||
			!altera_avalon_mutex_is_mine(mutex_4)
			)
			{
				altera_avalon_mutex_trylock(mutex_1, 1);
				altera_avalon_mutex_trylock(mutex_2, 1);
				altera_avalon_mutex_trylock(mutex_3, 1);
				altera_avalon_mutex_trylock(mutex_4, 1);
			}
		alt_putstr("other cpu's have finished computing\n");
		
		
		/*
		 * schedule writing result to memory
		 * */
		alt_putstr("cpu_0 scheduling result writing from other cpu's\n");
		
		if(altera_avalon_mutex_is_mine(mutex_1)) {
			alt_putstr("cpu_0 giving control to cpu 1\n");
			altera_avalon_mutex_unlock(mutex_1);
			delay(1);
			altera_avalon_mutex_lock(mutex_1, 1);
		} else {
			alt_putstr("cpu_0 cant open mutex 1 because does not own mutex 1\n");
		}
		
		if(altera_avalon_mutex_is_mine(mutex_2)) {
			alt_putstr("cpu_0 giving control to cpu 2\n");
			altera_avalon_mutex_unlock(mutex_2);
			delay(1);
			altera_avalon_mutex_lock(mutex_2, 1);
		} else {
			alt_putstr("cpu_0 cant open mutex 2 because does not own mutex 2\n");
		}
		
		if(altera_avalon_mutex_is_mine(mutex_3)) {
			alt_putstr("cpu_0 giving control to cpu 3\n");
			altera_avalon_mutex_unlock(mutex_3);
			delay(1);
			altera_avalon_mutex_lock(mutex_3, 1);
		} else {
			alt_putstr("cpu_0 cant open mutex 3 because does not own mutex 3\n");
		}
		
		if(altera_avalon_mutex_is_mine(mutex_4)) {
			alt_putstr("cpu_0 giving control to cpu 4\n");
			altera_avalon_mutex_unlock(mutex_4);
			delay(1);
			altera_avalon_mutex_lock(mutex_4, 1);
		} else {
			alt_putstr("cpu_0 cant open mutex 4 because does not own mutex 4\n");
		}
		
		
		alt_putstr("results written\n");
		delay(1);	
		
		/*
		 * TODO: 
		 * */
		alt_putstr("cpu_0 reading results from memory\n");
		delay(100);	//simulating memory access

		
		/*
		 * TODO: compare results 
		 * */
		alt_putstr("cpu_0 comparing results\n");
		delay(10);	//simulating comparison time
		
		
		/*
		 * TODO: calculate coordinates
		 * */
		alt_putstr("cpu_0 calculating coordinates\n");
		delay(10); //simulating coordinates time
		
		count ++;
		alt_printf("image number %x processed\n", count);
		delay(500);	//simulating writing time
		
  }
  return 0;
}
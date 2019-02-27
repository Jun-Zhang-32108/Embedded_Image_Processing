//#include <stdio.h>
#include "system.h"
//#include "io.h"
#include "sys/alt_stdio.h"
#include <altera_avalon_mutex.h>

extern void delay (int millisec);
char count = 0;

unsigned int* shared = (unsigned int*) SHARED_ONCHIP_BASE;

int main()
{
	alt_putstr("Hello cpu_2!\n");
	alt_mutex_dev* mutex_2 = altera_avalon_mutex_open( "/dev/mutex_2" );
	if(mutex_2) {
		alt_putstr("cpu_2 found mutex_2 handle\n");
	}
		
	while (1) {
		if(!altera_avalon_mutex_is_mine(mutex_2)) {
			altera_avalon_mutex_lock(mutex_2, 1);
			alt_putstr("cpu_2 reading image\n");
			  
			/*
			* TODO: read image from shared memory here
			* */
			delay(100); 	//simulating read time
			alt_printf("cpu_2 read %x\n", *shared);
			
			//signal read completion
			altera_avalon_mutex_unlock(mutex_2);
			delay(1);
			altera_avalon_mutex_lock(mutex_2, 1);
			
			/*
			* TODO: put grayscaling here
			* */
			alt_putstr("cpu_2 grayscaling\n");
			delay(100); 	//simulating read time
						
			/*
			* TODO: xcorr2 here
			* */
			alt_putstr("cpu_2 xcorr2'ing image\n");
			delay(100); 	//simulate xcorr time
			
			/*
			* TODO: offsetting here
			* */
			alt_putstr("cpu_2 offsetting\n");
			delay(10);	//simulating offsetting time
			
			/*
			 * TODO: calculate position
			 * */
			alt_putstr("cpu_2 calculating position\n");
			delay(10);	//simulating position time
			
			
			
			//signal computation completion
			altera_avalon_mutex_unlock(mutex_2);  
			delay(1);
			//wait until granted access to write to shared memory
			altera_avalon_mutex_lock(mutex_2, 1);
			
			
			
			
			/*
			* TODO: write result to shared memory here
			* */
			alt_putstr("cpu_2 writing result\n");
			delay(100); 	//simulating write time
//			*shared = 22;
			
			//signal write completion
			altera_avalon_mutex_unlock(mutex_2);
		} else {
			alt_putstr("ERROR DETECTED: cpu_2 not supposed to own mutex at begin of loop\n");
			altera_avalon_mutex_unlock(mutex_2);
		}
		count++;
		alt_printf("cpu_2 count = %x\n", count);
	}
	return 0;
}

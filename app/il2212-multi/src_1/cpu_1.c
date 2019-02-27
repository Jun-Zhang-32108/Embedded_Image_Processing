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
	alt_putstr("Hello cpu_1!\n");
	alt_mutex_dev* mutex_1 = altera_avalon_mutex_open( "/dev/mutex_1" );
	if(mutex_1) {
		alt_putstr("cpu_1 found mutex_1 handle\n");
	}
	
	while (1) {
		if(!altera_avalon_mutex_is_mine(mutex_1)) {
			altera_avalon_mutex_lock(mutex_1, 1);
			alt_putstr("cpu_1 reading image\n");
			
			/*
			* TODO: read image from shared memory here
			* */
			delay(100); 	//simulating read time
			alt_printf("cpu_1 read %x\n", *shared);
			
			//signal read completion
			altera_avalon_mutex_unlock(mutex_1);
			delay(1);
			altera_avalon_mutex_lock(mutex_1, 1);
			
			/*
			* TODO: put grayscaling here
			* */
			alt_putstr("cpu_1 grayscaling\n");
			delay(100); 	//simulating read time
			
			/*
			* TODO: xcorr2 here
			* */
			alt_putstr("cpu_1 xcorr2'ing image\n");
			delay(100); 	//simulate xcorr time
			
			/*
			* TODO: offsetting here
			* */
			alt_putstr("cpu_1 offsetting\n");
			delay(10);	//simulating offsetting time
			
			/*
			 * TODO: calculate position
			 * */
			alt_putstr("cpu_1 calculating position\n");
			delay(10);	//simulating position time
			
			
			
			
			//signal computation completion
			altera_avalon_mutex_unlock(mutex_1);  
			delay(1);
			//wait until granted access to write to shared memory
			altera_avalon_mutex_lock(mutex_1, 1);
			
			
			
			
			/*
			* TODO: write result to shared memory here
			* */
			alt_putstr("cpu_1 writing result\n");
			delay(100); 	//simulating write time
//			*shared = 21;
			
			//signal write completion
			altera_avalon_mutex_unlock(mutex_1);
		} else {
			alt_putstr("ERROR DETECTED: cpu_1 not supposed to own mutex at begin of loop\n");
			altera_avalon_mutex_unlock(mutex_1);
		}
		count++;
		alt_printf("cpu_1 count = %x\n", count);
	}
	return 0;
}

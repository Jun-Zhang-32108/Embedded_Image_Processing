#include <stdio.h>
#include "system.h"
#include "io.h"
#include "sys/alt_stdio.h"
#include <altera_avalon_mutex.h>

extern void delay (int millisec);
char count = 0;

int main()
{
	alt_putstr("Hello cpu_3!\n");
	alt_mutex_dev* mutex_3 = altera_avalon_mutex_open( "/dev/mutex_3" );
	if(mutex_3) {
		alt_putstr("cpu_3 found mutex_3 handle\n");
	}
		
	while (1) {
		if(!altera_avalon_mutex_is_mine(mutex_3)) {
			altera_avalon_mutex_lock(mutex_3, 1);
			alt_putstr("cpu_3 reading image\n");
			
			
			/*
			* TODO: read image from shared memory here
			* */
			delay(100); 	//simulating read time
			
			//signal read completion
			altera_avalon_mutex_unlock(mutex_3);
			delay(1);
			altera_avalon_mutex_lock(mutex_3, 1);
			
			/*
			* TODO: put grayscaling here
			* */
			alt_putstr("cpu_3 grayscaling\n");
			delay(100); 	//simulating read time
			
			/*
			* TODO: xcorr2 here
			* */
			alt_putstr("cpu_3 xcorr2'ing image\n");
			delay(100); 	//simulate xcorr time
			
			/*
			* TODO: offsetting here
			* */
			alt_putstr("cpu_3 offsetting\n");
			delay(10);	//simulating offsetting time
			
			/*
			* TODO: calculate position
			* */
			alt_putstr("cpu_3 calculating position\n");
			delay(10);	//simulating position time
		  
			
			
			
		  
			//signal computation completion
			altera_avalon_mutex_unlock(mutex_3);  
			delay(1);
			//wait until granted access to write to shared memory
			altera_avalon_mutex_lock(mutex_3, 1);
			
			
			
			
			/*
			* TODO: write result to shared memory here
			* */
			alt_putstr("cpu_3 writing result\n");
			delay(100); 	//simulating read time
			
			//signal write completion
			altera_avalon_mutex_unlock(mutex_3);  
		} else {
			alt_putstr("ERROR DETECTED: cpu_3 not supposed to own mutex at begin of loop\n");
			altera_avalon_mutex_unlock(mutex_3);
		}
		count++;
		alt_printf("cpu_3 count = %x\n", count);
	}
	return 0;
}

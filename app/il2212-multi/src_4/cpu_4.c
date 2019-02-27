//#include <stdio.h>
#include "system.h"
//#include "io.h"
#include "sys/alt_stdio.h"
#include <altera_avalon_mutex.h>

extern void delay (int millisec);
char count = 0;

unsigned char* shared = (unsigned char*) SHARED_ONCHIP_BASE;

int main()
{ 
	alt_putstr("Hello cpu_4!\n");
	alt_mutex_dev* mutex_4 = altera_avalon_mutex_open( "/dev/mutex_4" );
  	if(mutex_4) {
  		alt_putstr("cpu_4 found mutex_4 handle\n");
  	}
  	
  	while (1) {
		if(!altera_avalon_mutex_is_mine(mutex_4)) {
			altera_avalon_mutex_lock(mutex_4, 1);
			alt_putstr("cpu_4 reading image\n");
			
			/*
			* TODO: read image from shared memory here
			* */
			delay(100); 	//simulating read time
			alt_printf("cpu_4 read %x\n", *shared);
			
			//signal read completion
			altera_avalon_mutex_unlock(mutex_4);
			delay(1);
			altera_avalon_mutex_lock(mutex_4, 1);
			
			/*
			* TODO: put grayscaling here
			* */
			alt_putstr("cpu_4 grayscaling\n");
			delay(100); 	//simulating read time
			
			/*
			* TODO: xcorr2 here
			* */
			alt_putstr("cpu_4 xcorr2'ing image\n");
			delay(100); 	//simulate xcorr time
			
			/*
			* TODO: offsetting here
			* */
			alt_putstr("cpu_4 offsetting\n");
			delay(10);	//simulating offsetting time
			
			/*
			 * TODO: calculate position
			 * */
			alt_putstr("cpu_4 calculating position\n");
			delay(10);	//simulating position time
			
			
			
			//signal computation completion
			altera_avalon_mutex_unlock(mutex_4);  
			delay(1);
			//wait until granted access to write to shared memory
			altera_avalon_mutex_lock(mutex_4, 1);
			
			
			
			
			/*
			* TODO: write result to shared memory here
			* */
			alt_putstr("cpu_4 writing result\n");
			delay(100); 	//simulating write time
//			*shared = 24;
			
			//signal write completion
			altera_avalon_mutex_unlock(mutex_4);
		} else {
			alt_putstr("ERROR DETECTED: cpu_4 not supposed to own mutex at begin of loop\n");
			altera_avalon_mutex_unlock(mutex_4);
		}
		count++;
		alt_printf("cpu_4 count = %x\n", count);
	}
  	return 0;
}

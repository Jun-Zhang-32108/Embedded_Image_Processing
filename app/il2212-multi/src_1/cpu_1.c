#include <stdio.h>
#include "system.h"
#include "io.h"
#include "sys/alt_stdio.h"
#include <altera_avalon_mutex.h>

extern void delay (int millisec);




int main()
{
	alt_putstr("Hello cpu_1!\n");
	alt_mutex_dev* mutex_1 = altera_avalon_mutex_open( "/dev/mutex_1" );
	if(mutex_1) {
		alt_putstr("cpu_1 found mutex_1 handle");
	}
	
	while (1) {
		if(!altera_avalon_mutex_is_mine(mutex_1)) {
			altera_avalon_mutex_lock(mutex_1, 1);
			alt_putstr("cpu_1 reading image\n");
			
			/*
			* TODO: read image from shared memory here
			* */
			delay(100); 	//simulating read time
			
			
			altera_avalon_mutex_unlock(mutex_1);
			
			/*
			* TODO: put grayscaling here
			* */
			alt_putstr("cpu_1 grayscaling\n");
			
			//xcorr synchronisation
			altera_avalon_mutex_lock(mutex_1, 1);
			
			
			/*
			* TODO: xcorr2 here
			* */
			alt_putstr("cpu_1 xcorr2'ing image\n");
			delay(100); 	//simulate xcorr time
			
			
			altera_avalon_mutex_unlock(mutex_1);  
	  }
  }
  return 0;
}

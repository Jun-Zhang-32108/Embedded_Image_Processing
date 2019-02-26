#include <stdio.h>
#include "system.h"
#include "io.h"
#include "sys/alt_stdio.h"
#include <altera_avalon_mutex.h>

extern void delay (int millisec);


int main()
{
	alt_putstr("Hello cpu_2!\n");
	alt_mutex_dev* mutex_2 = altera_avalon_mutex_open( "/dev/mutex_2" );
	if(mutex_2) {
		alt_putstr("cpu_2 found mutex_2 handle");
	}
		
	while (1) {
		if(!altera_avalon_mutex_is_mine(mutex_2)) {
			altera_avalon_mutex_lock(mutex_2, 1);
			alt_putstr("cpu_2 reading image\n");
			  
			  
			/*
			* TODO: read image from shared memory here
			* */
			delay(100); 	//simulating read time
			
			
			altera_avalon_mutex_unlock(mutex_2);
			
			/*
			* TODO: put grayscaling here
			* */
			alt_putstr("cpu_2 grayscaling\n");
			//xcorr synchronisation
			altera_avalon_mutex_lock(mutex_2, 1);
			alt_putstr("cpu_2 xcorr2'ing image\n");
			
			/*
			* TODO: xcorr2 here
			* */
			delay(100); 	//simulate xcorr time
			
			
			altera_avalon_mutex_unlock(mutex_2);  
		  }
	  }
  return 0;
}

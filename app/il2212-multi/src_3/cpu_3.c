#include <stdio.h>
#include "system.h"
#include "io.h"
#include "sys/alt_stdio.h"
#include <altera_avalon_mutex.h>

extern void delay (int millisec);

int main()
{
	alt_putstr("Hello cpu_3!\n");
	alt_mutex_dev* mutex_3 = altera_avalon_mutex_open( "/dev/mutex_3" );
		if(mutex_3) {
			alt_putstr("cpu_3 found mutex_3 handle");
		}
		
	  while (1) {
		  if(!altera_avalon_mutex_is_mine(mutex_3)) {
			  altera_avalon_mutex_lock(mutex_3, 1);
			  alt_putstr("cpu_3 reading image\n");
			  
			  
			  /*
			   * TODO: read image from shared memory here
			   * */
			  delay(100); 	//simulating read time
			  
			  altera_avalon_mutex_unlock(mutex_3);
			  
			  /*
			   * TODO: put grayscaling here
			   * */
			  alt_putstr("cpu_3 grayscaling\n");
			  //xcorr synchronisation
			  altera_avalon_mutex_lock(mutex_3, 1);
			  alt_putstr("cpu_3 xcorr2'ing image\n");
			  
			  /*
			   * TODO: xcorr2 here
			   * */
			  delay(100); 	//simulate xcorr time
			  
			  
			  altera_avalon_mutex_unlock(mutex_3);  
		  }
	  }
  return 0;
}

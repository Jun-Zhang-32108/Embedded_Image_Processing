//#include <stdio.h>
//#include "system.h"
#include "altera_avalon_pio_regs.h"
//#include "io.h"
#include "sys/alt_stdio.h"
#include <altera_avalon_mutex.h>

extern void delay (int millisec);


/*
 * Example function for copying a p3 image from sram to the shared on-chip mempry
 */
//void sram2sm_p3(unsigned char* base)
//{
//	int x, y;
//	unsigned char* shared;
//
//	shared = (unsigned char*) SHARED_ONCHIP_BASE;
//
//	int size_x = *base++;
//	int size_y = *base++;
//	int max_col= *base++;
//	*shared++  = size_x;
//	*shared++  = size_y;
//	*shared++  = max_col;
//	printf("The image is: %d x %d!! \n", size_x, size_y);
//	for(y = 0; y < size_y; y++)
//	for(x = 0; x < size_x; x++)
//	{
//		*shared++ = *base++; 	// R
//		*shared++ = *base++;	// G
//		*shared++ = *base++;	// B
//	}
//}



//int* FLAGS = 0x0000;
//
//unsigned char* shared;
//
//shared = (unsigned char*) SHARED_ONCHIP_BASE;
//
//void new_image() {
//	FLAGS = FLAGS | 0x0001;
//}
//
//void image_cropped() {
//	FLAGS = FLAGS | 0x0010;
//}

///* get the mutex device handle */
//alt_mutex_dev* mutex = altera_avalon_mutex_open( "/dev/mutex_4" );
///* acquire the mutex, setting the value to one */
//altera_avalon_mutex_lock( mutex, 1 );
///*
//* Access a shared resource here.
//*/
///* release the lock */
//altera_avalon_mutex_unlock( mutex );



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
	
//	if(altera_avalon_mutex_is_mine(mutex_0)) {
//		alt_putstr("cpu_0 owns mutex 0\n");
//	} else {
//		alt_putstr("cpu_0 does not own mutex 0\n");
//	}
//	
//	if(altera_avalon_mutex_is_mine(mutex_1)) {
//		alt_putstr("cpu_0 owns mutex 1\n");
//	} else {
//		alt_putstr("cpu_0 does not own mutex 1\n");
//	}
//
//	if(altera_avalon_mutex_is_mine(mutex_2)) {
//		alt_putstr("cpu_0 owns mutex 2\n");
//	} else {
//		alt_putstr("cpu_0 does not own mutex 2\n");
//	}	
//	
//	
//	if(altera_avalon_mutex_is_mine(mutex_3)) {
//		alt_putstr("cpu_0 owns mutex 3\n");
//	} else {
//		alt_putstr("cpu_0 does not own mutex 3\n");
//	}
//	
//	if(altera_avalon_mutex_is_mine(mutex_4)) {
//		alt_putstr("cpu_0 owns mutex 4\n");
//	} else {
//		alt_putstr("cpu_0 does not own mutex 4\n");
//	}
	
	
	while (1) {
		
		
		
		/*
		 * TODO: write image to shared memory
		 * */
		alt_putstr("cpu_0 writing image to memory\n");
		delay(1000);	//simulating writing time
		
		
		
		/*
		 * schedule reading from memory
		 * */
		alt_putstr("cpu_0 scheduling reading and grayscaling to other cpu's\n");
		
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
		 * wait for cpu 4 to finish grayscaling
		 * */
		altera_avalon_mutex_lock(mutex_0, 1);
		altera_avalon_mutex_unlock(mutex_0);
		
		
		/*
		 * TODO: cropping function
		 * */
		alt_putstr("cpu_0 cropping\n");
		delay(100);	//simulating cropping time
		
		
		
		
		/*
		 * schedule xcorr2
		 * */
		alt_putstr("cpu_0 scheduling xcorr2 to other cpu's\n");
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
		 * TODO: offsetting function
		 * */
		alt_putstr("cpu_0 offsetting\n");
		delay(100);	//simulating offsetting time
		
		/*
		 * TODO: calculate coordinates
		 * */
		alt_putstr("cpu_0 calculating coordinates\n");
		delay(100); //simulating coordinates time
		
		/*
		 * TODO: calculate position
		 * */
		alt_putstr("cpu_0 calculating position\n");
		delay(100);	//simulating position time
		
		
		
  }
  return 0;
}
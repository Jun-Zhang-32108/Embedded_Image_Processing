#include <stdio.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
//#include "io.h"
#include "sys/alt_stdio.h"
#include <altera_avalon_mutex.h>
#include "altera_avalon_performance_counter.h"
#include "images.h"
#include "alt_types.h"
//#define SECTION_WRITEIMAGETOSHAREDMEMORY 	1

//#define SECTION_OTHER_COMPUTING 		4



#define SECTION_IMAGE_PROCESSING 				1
#define SECTION_WRITE_IMAGE_TO_SHARED_MEMORY	2
#define SECTION_READ_IMAGE_FROM_SHARED_MEMORY	3
#define SECTION_DISTRIBUTED_COMPUTING			4
#define SECTION_READ_RESULTS					5

typedef alt_u8 INT8U;
typedef alt_u16 INT16U;

#define img_w 64
#define img_h 64
#define img_w_0 192 // =img_w*2

#define dSPAN  15
#define cropSIZE  36
#define xPATTERN_length  5
#define offset_size_length 32
#define cropSIZE_int_RGB 27  // = 36*3/4 


extern void delay (int millisec);
INT8U count = 0;
//int current_image = 0;
unsigned char* shared = (unsigned char*) SHARED_ONCHIP_BASE;

#define results_offset 4000 //results from other CPUs are stored at a place starting from shared+4000
#define DEBUG 1




/*
 * Example function for copying a p3 image from sram to the shared on-chip mempry
 */
// void sram2sm_p3(unsigned char* base)
// {
// 	int x, y;
// 	unsigned char* shared;

// 	shared = (unsigned char*) SHARED_ONCHIP_BASE;

// 	int size_x = *base++;
// 	int size_y = *base++;
// 	int max_col= *base++;
// 	*shared++  = size_x;
// 	*shared++  = size_y;
// 	*shared++  = max_col;
// 	alt_printf("The image is: %d x %d!! \n", size_x, size_y);
// 	for(y = 0; y < size_y; y++)
// 	for(x = 0; x < size_x; x++)
// 	{
// 		*shared++ = *base++; 	// R
// 		*shared++ = *base++;	// G
// 		*shared++ = *base++;	// B
// 	}
// }


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
	
	
	
	INT8U *image;
	INT16U detected[3];
	INT8U coords[2];
	int *writePointer;
	INT16U *readPointer;
	INT16U valueFromCPUs[4];
	INT16U detectedCoords[2];

	detected[0] = 0;
	detected[1] = 0;	
	
	INT8U* startingPoINT8UAddress;
	int* staringPointAddress_forRow;
	INT8U i;
	INT8U j;

	delay(100);		
	while (1) {

		if(count == 1)
		{
			alt_putstr("all the four images has been processed!\n");
			return 0;
		}

		image = image_sequence[count] + 4;

		PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
		PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_IMAGE_PROCESSING);
		
		/*
		 * TODO: calculating the cropping point, cropping function and transmit the image to shared onship memory
		 * */
		alt_putstr("cpu_0 cropping & writing image to memory\n");

		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_WRITE_IMAGE_TO_SHARED_MEMORY);
		if(detected[0] <= dSPAN)
			coords[0] = 0;
		else if (detected[0] > (img_h - dSPAN))
			coords[0] = img_h - cropSIZE;
		else
			coords[0] = detected[0] - dSPAN ;
		if(detected[1] <= dSPAN)
			coords[1] = 0;
		else if (detected[1] > (img_w - dSPAN))
			coords[1] = img_w - cropSIZE;
		else
			coords[1] = detected[1] - dSPAN ;
		
		//alt_putstr("Coords point: %x, %x . \n",coords[0],coords[1]);
		
		
		startingPoINT8UAddress = image + img_w_0 * coords[0] + coords[1]*3; 
		writePointer = (int*)shared - cropSIZE_int_RGB;
		for ( i = 0; i < cropSIZE; i++)
		{
			staringPointAddress_forRow =(int*)(startingPoINT8UAddress + img_w_0*i);
			writePointer += cropSIZE_int_RGB;
			for( j = 0; j < cropSIZE_int_RGB; j = j + 9)
			{
				//*writePointer++ = *staringPointAddress_forRow++;
				*(writePointer+j)     = *(staringPointAddress_forRow+j);
				*(writePointer+j + 1) = *(staringPointAddress_forRow+j + 1); 
				*(writePointer+j + 2) = *(staringPointAddress_forRow+j + 2); 
				*(writePointer+j + 3) = *(staringPointAddress_forRow+j + 3); 
				*(writePointer+j + 4) = *(staringPointAddress_forRow+j + 4); 
				*(writePointer+j + 5) = *(staringPointAddress_forRow+j + 5); 
				*(writePointer+j + 6) = *(staringPointAddress_forRow+j + 6); 
				*(writePointer+j + 7) = *(staringPointAddress_forRow+j + 7); 
				*(writePointer+j + 8) = *(staringPointAddress_forRow+j + 8);  
			}		
		}
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_WRITE_IMAGE_TO_SHARED_MEMORY);
		
		/*
		 * TODO: write image to shared memory
		 * */
		//alt_putstr("cpu_0 writing image to memory\n");
		//delay(500);	//simulating writing time
		//sram2sm_p3(test_ppm_1);
		
		
		
		/*
		 * schedule reading from memory
		 * */
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_READ_IMAGE_FROM_SHARED_MEMORY);
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
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_READ_IMAGE_FROM_SHARED_MEMORY);
		
		/*
		 * let all cpus start computation at the same time.
		 * it is okay to have cpus wait for others to finish reading first
		 * because cpu0 will still need to synchronise 
		 * with all the results later anyways.
		 * */
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_DISTRIBUTED_COMPUTING);
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
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_DISTRIBUTED_COMPUTING);
		
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
		
		alt_putstr("cpu_0 reading results from memory and compare them in order to get the final result\n");
		//delay(100);	//simulating memory access
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_READ_RESULTS);
		// results are stored at a consecutive memory space starting from shared + 4000, the order is x1,y1,value1, x2, y2, value2 ... 
		// Datatype is INT16U
		readPointer = (INT16U*)(shared + results_offset);

		valueFromCPUs[0] = *(readPointer + 2);

		detected[2]		 = valueFromCPUs[0];
		detected[0]		 = *(readPointer );
		detected[1]		 = *(readPointer + 1);
		alt_printf("cpu_0 reading results from CPU1: %x \n",detected[0]);
		alt_printf("cpu_0 reading results from CPU1: %x \n",detected[1]);
		alt_printf("cpu_0 reading results from CPU1: %x \n",detected[2]);
		valueFromCPUs[1] = *(readPointer + 5);
		if(detected[2] < valueFromCPUs[1])
			{
				detected[2]		 = valueFromCPUs[1];
				detected[0]		 = *(readPointer + 3);
				detected[1]		 = *(readPointer + 4);
			}
		
		valueFromCPUs[2] = *(readPointer + 8);
		if(detected[2] < valueFromCPUs[2])
			{
				detected[2]		 = valueFromCPUs[2];
				detected[0]		 = *(readPointer + 6);
				detected[1]		 = *(readPointer + 7);
			}
		
		valueFromCPUs[3] = *(readPointer + 11);
		if(detected[2] < valueFromCPUs[3])
			{
				detected[2]		 = valueFromCPUs[3];
				detected[0]		 = *(readPointer + 9);
				detected[1]		 = *(readPointer + 10);
			}

		//calculating the positions by adding the offset to the coords

		detected[0] = detected[0] + coords[0] + 2;
		detected[1] = detected[1] + coords[1] + 2;

		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_READ_RESULTS);
		alt_printf("cpu_0 read %x\n", *shared);
		alt_printf("Final result: %x, %x, %x\n", detected[0],detected[1],detected[2]);
		

		
		/*
		 * TODO: compare results 
		 * */
		//alt_putstr("cpu_0 comparing results\n");
		// delay(10);	//simulating comparison time
		
		
		/*
		 * TODO: calculate coordinates
		 * */
		// alt_putstr("cpu_0 calculating coordinates\n");
		// delay(10); //simulating coordinates time
		
		count ++;
		alt_printf("image number %x processed\n", count);
		
		
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_IMAGE_PROCESSING);
	    PERF_STOP_MEASURING (PERFORMANCE_COUNTER_0_BASE);

	    perf_print_formatted_report
			(PERFORMANCE_COUNTER_0_BASE,
			ALT_CPU_FREQ,        // defined in "system.h"
			5,                   // How many sections to print
			"Total",
			"writing image",
			"reading image",
			"computing",
			"read results"
			);
	    
	    delay(500);	//nice wait
  }
  return 0;
}
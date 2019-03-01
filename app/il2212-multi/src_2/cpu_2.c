//#include <stdio.h>
#include "system.h"
//#include "io.h"
#include "sys/alt_stdio.h"
#include <altera_avalon_mutex.h>

#define shared_address_offset_for_cpu2 864 // = 8*36*3
#define cropSIZE  36
#define img_h 64
#define img_w 64
#define img_w_0 192 // = 64*3
#define image_piece_h 12
#define offset_size_length 32
#define offset_size_height 8  // = 12 - 4
#define cropSIZE_int_RGB 27  // = 36*3/4 
#define results_offset 4000 //results from other CPUs are stored at a place starting from shared+4000
typedef unsigned char INT8U;
typedef unsigned short INT16U;

extern void delay (int millisec);
char count = 0;

unsigned char* shared = (unsigned char*) SHARED_ONCHIP_BASE;

//Print the 2-D matrix with INT8U datatype
void printMatrix(INT8U *inputMatrix, INT8U matrix_h, INT8U matrix_w)
{
	INT8U i;
	INT8U j;
	alt_putstr("============================================================\n");
	alt_putstr("============================================================\n");
	alt_putstr("\n");
	alt_putstr("[");
	for(i = 0; i < matrix_h; i++)
	{
		alt_putstr("[	");
		for(j = 0; j < matrix_w; j++)
		{
			alt_printf("  %x	",*(inputMatrix+matrix_w*i+j));
		    //alt_printf("  %d	",inputMatrix[i][j]);
		}
		alt_putstr("]");
		alt_putstr("\n");
	}
	alt_putstr("]");
	alt_putstr("============================================================\n");
	alt_putstr("============================================================\n");
	alt_putstr("\n");
}

//Print the 2-D matrix with INT8U datatype
void printMatrix_INT16U(INT16U *inputMatrix, INT8U matrix_h, INT8U matrix_w)
{
	INT8U i;
	INT8U j;
	alt_putstr("============================================================\n");
	alt_putstr("============================================================\n");
	alt_putstr("\n");
	alt_putstr("[");
	for(i = 0; i < matrix_h; i++)
	{
		alt_putstr("[	");
		for(j = 0; j < matrix_w; j++)
		{
			alt_printf("  %x	",*(inputMatrix+matrix_w*i+j));
		    //alt_printf("  %d	",inputMatrix[i][j]);
		}
		alt_putstr("]");
		alt_putstr("\n");
	}
	alt_putstr("]");
	alt_putstr("============================================================\n");
	alt_putstr("============================================================\n");
	alt_putstr("\n");
}

int main()
{
	int i;
	INT8U j;
	INT8U image_piece[1296];  // = 12*36*3
	INT8U grayed[432];		  // = 12*36
	INT16U xcropp2ed[offset_size_height*offset_size_length];
	int* writePointer;
	INT16U* writeBackPointer;
	INT8U *row_add;
	INT8U *startingPoINT8UAddress;
	INT16U output[3];
	INT16U max=0;
	// image piece is stored from shared ~ shared + 12*36*3 -1
	int* readAddress; 


	// alt_putstr("Hello cpu_2!\n");
	alt_mutex_dev* mutex_2 = altera_avalon_mutex_open( "/dev/mutex_2" );
	if(mutex_2) {
		alt_putstr("cpu_2 found mutex_2 handle\n");
	}
		
		/*
	 * NEW PART - START FROM THE SAME LINE
	 * */
	
	altera_avalon_mutex_lock(mutex_2, 1);
	delay(1);
	altera_avalon_mutex_unlock(mutex_2);
	delay(10);
		
	/*
	 * END OF NEW PART - ALL CPUS ARE ON THE SAME PAGE
	 * */
	while (1) {
		//if(!altera_avalon_mutex_is_mine(mutex_2)) {
			altera_avalon_mutex_lock(mutex_2, 1);
			// alt_putstr("cpu_2 reading image\n");
			  
			/*
			* TODO: read image from shared memory here
			* */
			//delay(100); 	//simulating read time
			readAddress = (int*)(shared + shared_address_offset_for_cpu2 )- cropSIZE_int_RGB;
			writePointer = (int*)image_piece - cropSIZE_int_RGB;
			//delay(100); 	//simulating read time
			for( i = 0; i < image_piece_h; i++)
			{
				writePointer += cropSIZE_int_RGB;
				readAddress  += cropSIZE_int_RGB;
				for ( j = 0; j < cropSIZE_int_RGB; j = j +9)
				{
					*(writePointer+j)     = *(readAddress +j);
					*(writePointer+j + 1) = *(readAddress +j + 1); 
					*(writePointer+j + 2) = *(readAddress +j + 2); 
					*(writePointer+j + 3) = *(readAddress +j + 3); 
					*(writePointer+j + 4) = *(readAddress +j + 4); 
					*(writePointer+j + 5) = *(readAddress +j + 5); 
					*(writePointer+j + 6) = *(readAddress +j + 6); 
					*(writePointer+j + 7) = *(readAddress +j + 7); 
					*(writePointer+j + 8) = *(readAddress +j + 8); 
				}
			}			




			// alt_printf("cpu_2 read %x\n", *shared);

			
			
			//signal read completion
			altera_avalon_mutex_unlock(mutex_2);
			//delay(1);
			//altera_avalon_mutex_lock(mutex_2, 1);
			
			/*
			* TODO: put grayscaling here
			* */
			// alt_putstr("cpu_2 grayscaling\n");
			//delay(100); 	//simulating read time

			for ( i = 0; i < image_piece_h; i++)
			{
				//grayed[i] = grayed[0] + cropSIZE * i;
				row_add = image_piece + i*cropSIZE*3;
				for(j = 0; j < cropSIZE*3; j = j+36)
				{
					//one algorithm to transform RGB value to grayscale value which is suitable for operation between 16-bit variables
					//the good balance between speed and precision
					//grayed[i][j/3] = ((inputMatrix[i][j]*38
					//			 +inputMatrix[i][j+1]*75
					//			 +inputMatrix[i][j+2]*15)>>7);

					//Another grayscale algorithm, lose some precison but increase the speed
					//using loop unrolling, assume that we know the cropSIZE is 36
					*(grayed+i*cropSIZE+j/3)   = (((*(row_add +j ))>>2 )
								 +((*(row_add +j+ 1))>>1 )
								 +((*(row_add +j+ 2))>>3));
					*(grayed+i*cropSIZE+j/3 + 1) = (((*(row_add +j + 3))>>2 )
								 +((*(row_add +j+ 4))>>1 )
								 +((*(row_add +j+ 5))>>3));
					*(grayed+i*cropSIZE+j/3 + 2) = (((*(row_add +j + 6 ))>>2 )
								 +((*(row_add +j+ 7))>>1 )
								 +((*(row_add +j+ 8))>>3));
					*(grayed+i*cropSIZE+j/3 + 3)= (((*(row_add +j + 9))>>2 )
								 +((*(row_add +j+ 10))>>1 )
								 +((*(row_add +j+ 11))>>3));
					*(grayed+i*cropSIZE+j/3 + 4) = (((*(row_add +j + 12))>>2 )
								 +((*(row_add +j+ 13))>>1 )
								 +((*(row_add +j+ 14))>>3));
					*(grayed+i*cropSIZE+j/3 + 5) = (((*(row_add +j + 15))>>2 )
								 +((*(row_add +j+ 16))>>1 )
								 +((*(row_add +j+ 17))>>3));
					*(grayed+i*cropSIZE+j/3 + 6) = (((*(row_add +j + 18))>>2 )
								 +((*(row_add +j+ 19 ))>>1 )
								 +((*(row_add +j+ 20))>>3));
					*(grayed+i*cropSIZE+j/3 + 7) = (((*(row_add +j + 21))>>2 )
								 +((*(row_add +j+ 22))>>1 )
								 +((*(row_add +j+ 23))>>3));
					*(grayed+i*cropSIZE+j/3 + 8) = (((*(row_add +j + 24))>>2 )
								 +((*(row_add +j+ 25))>>1 )
								 +((*(row_add +j+ 26))>>3));
					*(grayed+i*cropSIZE+j/3 + 9) = (((*(row_add +j + 27))>>2 )
								 +((*(row_add +j+ 28))>>1 )
								 +((*(row_add +j+ 29))>>3));
					*(grayed+i*cropSIZE+j/3 + 10) = (((*(row_add +j + 30))>>2 )
								 +((*(row_add +j+ 31 ))>>1 )
								 +((*(row_add +j+ 32))>>3));
					*(grayed+i*cropSIZE+j/3 + 11) = (((*(row_add +j + 33))>>2 )
								 +((*(row_add +j+ 34))>>1 )
								 +((*(row_add +j+ 35))>>3));

				}		
			}

			//printMatrix(grayed,image_piece_h, cropSIZE);
						
			/*
			* TODO: xcorr2 here
			* */
			// alt_putstr("cpu_2 xcorr2'ing image\n");
			//delay(100); 	//simulate xcorr time


			
			for ( i = 0; i < offset_size_height; i++)
			{
				for( j = 0; j < offset_size_length; j=j+8)
				{

					// startingPoINT8UAddress = *inputMatrix + cropSIZE*i + j;

					// // unroll the loop partially and assume the pattern we want to detected is fixed
					// *(xcropp2ed + i*output_size_length + j) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + cropSIZE*2) + *(startingPoINT8UAddress + cropSIZE*2 +  4)
					// 		+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + cropSIZE*4 +  2);
					// //product = (*(startingPoINT8UAddress + cropSIZE*m + n));

					// startingPoINT8UAddress = startingPoINT8UAddress+1;
					// *(xcropp2ed + i*output_size_length + j + 1) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + cropSIZE*2) + *(startingPoINT8UAddress + cropSIZE*2 +  4)
					// 		+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + cropSIZE*4 +  2);

					// startingPoINT8UAddress = startingPoINT8UAddress+1;
					// *(xcropp2ed + i*output_size_length + j + 2) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + cropSIZE*2) + *(startingPoINT8UAddress + cropSIZE*2 +  4)
					// 		+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + cropSIZE*4 +  2);

					// startingPoINT8UAddress = startingPoINT8UAddress+1;
					// *(xcropp2ed + i*output_size_length + j + 3) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + cropSIZE*2) + *(startingPoINT8UAddress + cropSIZE*2 +  4)
					// 		+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + cropSIZE*4 +  2);

					// startingPoINT8UAddress = startingPoINT8UAddress+1;
					// *(xcropp2ed + i*output_size_length + j + 4) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + cropSIZE*2) + *(startingPoINT8UAddress + cropSIZE*2 +  4)
					// 		+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + cropSIZE*4 +  2);

					// startingPoINT8UAddress = startingPoINT8UAddress+1;
					// *(xcropp2ed + i*output_size_length + j + 5) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + cropSIZE*2) + *(startingPoINT8UAddress + cropSIZE*2 +  4)
					// 		+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + cropSIZE*4 +  2);

					// startingPoINT8UAddress = startingPoINT8UAddress+1;
					// *(xcropp2ed + i*output_size_length + j + 6) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + cropSIZE*2) + *(startingPoINT8UAddress + cropSIZE*2 +  4)
					// 		+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + cropSIZE*4 +  2);

					// startingPoINT8UAddress = startingPoINT8UAddress+1;
					// *(xcropp2ed + i*output_size_length + j + 7) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + cropSIZE*2) + *(startingPoINT8UAddress + cropSIZE*2 +  4)
					// 		+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + cropSIZE*4 +  2);



					startingPoINT8UAddress = grayed + cropSIZE*i + j;
					*(xcropp2ed + (i<<5) + j ) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + (cropSIZE<<1)) + *(startingPoINT8UAddress + (cropSIZE<<1) +  4)
							+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + (cropSIZE<<2) +  2);

					startingPoINT8UAddress = startingPoINT8UAddress+1;
					*(xcropp2ed + (i<<5) + j + 1 ) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + (cropSIZE<<1)) + *(startingPoINT8UAddress + (cropSIZE<<1) +  4)
							+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + (cropSIZE<<2) +  2);

					startingPoINT8UAddress = startingPoINT8UAddress+1;
					*(xcropp2ed + (i<<5) + j + 2) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + (cropSIZE<<1)) + *(startingPoINT8UAddress + (cropSIZE<<1) +  4)
							+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + (cropSIZE<<2) +  2);

					startingPoINT8UAddress = startingPoINT8UAddress+1;
					*(xcropp2ed + (i<<5) + j + 3) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + (cropSIZE<<1)) + *(startingPoINT8UAddress + (cropSIZE<<1) +  4)
							+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + (cropSIZE<<2) +  2);

					startingPoINT8UAddress = startingPoINT8UAddress+1;
					*(xcropp2ed + (i<<5) + j + 4) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + (cropSIZE<<1)) + *(startingPoINT8UAddress + (cropSIZE<<1) +  4)
							+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + (cropSIZE<<2) +  2);

					startingPoINT8UAddress = startingPoINT8UAddress+1;
					*(xcropp2ed + (i<<5) + j + 5) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + (cropSIZE<<1)) + *(startingPoINT8UAddress + (cropSIZE<<1) +  4)
							+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + (cropSIZE<<2) +  2);

					startingPoINT8UAddress = startingPoINT8UAddress+1;
					*(xcropp2ed + (i<<5) + j + 6) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + (cropSIZE<<1)) + *(startingPoINT8UAddress + (cropSIZE<<1) +  4)
							+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + (cropSIZE<<2) +  2);

					startingPoINT8UAddress = startingPoINT8UAddress+1;
					*(xcropp2ed + (i<<5) + j + 7) = *(startingPoINT8UAddress + 2) + *(startingPoINT8UAddress + cropSIZE +  1) + *(startingPoINT8UAddress + cropSIZE +  3) + *(startingPoINT8UAddress + (cropSIZE<<1)) + *(startingPoINT8UAddress + (cropSIZE<<1) +  4)
							+ *(startingPoINT8UAddress + cropSIZE*3 +  1) + *(startingPoINT8UAddress + cropSIZE*3 +  3) + *(startingPoINT8UAddress + (cropSIZE<<2) +  2);

				}		
			}

			//printMatrix_INT16U(xcropp2ed,offset_size_height, offset_size_length);

			

			max = 0;
			/*
			* TODO: offsetting here
			* */
			// alt_putstr("cpu_2 offsetting\n");
			//delay(10);	//simulating offsetting time
			
			for(i = 0; i<offset_size_height; i++)
				for(j = 0; j <offset_size_length; j++)
				{
					if(max < *(xcropp2ed+offset_size_length*i+j))
					{
						max = *(xcropp2ed+offset_size_length*i+j);
						output[0] = i;
						output[1] = j;
						output[2] = max;
					}
				}

			output[0] += 8;


			/*
			 * TODO: calculate position
			 * */
			// alt_putstr("cpu_2 calculating position\n");
			// delay(10);	//simulating position time
			
			
			
			//signal computation completion
			//altera_avalon_mutex_unlock(mutex_2);  
			//delay(1);
			//wait until granted access to write to shared memory
			
			
			
			
			
			/*
			* TODO: write result to shared memory here
			* */
			// alt_putstr("cpu_2 writing result\n");
			//delay(100); 	//simulating write time
//			*shared = 22;
			altera_avalon_mutex_lock(mutex_2, 1);
			writeBackPointer = (INT16U*)(shared + results_offset) + 3;
			*(writeBackPointer) 	= output[0] ; 
			*(writeBackPointer + 1) = output[1] ;
			*(writeBackPointer + 2)	= output[2]; 
			// alt_printf("value: %x\n",output[0]);
			// alt_printf("value: %x\n",output[1]);
			// alt_printf("value: %x\n",output[2]);			
			//signal write completion
			altera_avalon_mutex_unlock(mutex_2);
		//} else {
		//	alt_putstr("ERROR DETECTED: cpu_2 not supposed to own mutex at begin of loop\n");
		//	altera_avalon_mutex_unlock(mutex_2);
		//}
		count++;
		// alt_printf("cpu_2 count = %x\n", count);
	}
	return 0;
}

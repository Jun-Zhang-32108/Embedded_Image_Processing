/* Date: 2019-2-5
 * Author: Jun Zhang, Jaakko Laiho
 * Email£º zhangjun32108@outlook.com 
 * Description: An image processing program that captures the required pattern in each frame of the input flow
 */

/*
 * Things have done: The single-core bare medal program is basically done. We have tested it on laptop but not on the board.
 * 
 * 
 * Todo list 1: Test the source file on the Board
 * Todo list 2: Optimize the code in terms of time complexity and space complexity
 */

#include <stdlib.h>

//Path:\il2212-lab-master\app\hello_image\src_0
//Contain the RGB value of tesing image. Using this test .h file can avoid reading data from the PPM file. 
#include "images.h"


//Define two new datatypes to save memory. Since the maximum grayscale value is 255, INT8U (8 bits) should be big enough to hold most variables.
//Only when we call xcorr2 functions the dot product value is possible to be greater than 255. So we use INT16U to hold that value.  

//typedef unsigned char INT8U;
//typedef unsigned short INT16U;

INT8U dSPAN = 15;
//Here I use 33 instead of 31 because if we use 31, it may not capture the whole pattern when the pattern move 15 pixels in one frame
INT8U cropSIZE = 33; //cropSIZE = 2 * dsPAN + 1 + 5 - 3;
INT8U xPATTERN[5][5] = {{0,0,1,0,0},
					  {0,1,0,1,0},
					  {1,0,0,0,1},
					  {0,1,0,1,0},
					  {0,0,1,0,0}};					  
INT8U xPATTERN_length = 5;
INT8U offset_size_length = 29; // offset_size_length = cropSIZE - 4
INT8U INT8U_poINT8Uer_size = sizeof(INT8U*);
INT8U INT8U_size		   = sizeof(INT8U);


//Print the 2-D matrix with INT8U datatype
void printMatrix(INT8U **inputMatrix, INT8U matrix_h, INT8U matrix_w)
{
	INT8U i;
	INT8U j;
	printf("============================================================\n");
	printf("============================================================\n");
	printf("\n");
	printf("[");
	for(i = 0; i < matrix_h; i++)
	{
		printf("[	");
		for(j = 0; j < matrix_w; j++)
		{
			//printf("(%d, %d)  %d	",i,j,*(*inputMatrix+matrix_w*i+j));
			//printf("(%d, %d)  %d	",i,j,inputMatrix[i][j]);
			printf("  %d	",inputMatrix[i][j]);
		}
		printf("]");
		printf("\n");
	}
	printf("]");
	printf("============================================================\n");
	printf("============================================================\n");
	printf("\n");
}

//Print the 2-D matrix with INT16U datatype
void printMatrix_INT16U(INT16U **inputMatrix, INT8U matrix_h, INT8U matrix_w)
{
	INT8U i;
	INT8U j;
	printf("============================================================\n");
	printf("============================================================\n");
	printf("\n");
	printf("[");
	for(i = 0; i < matrix_h; i++)
	{
		printf("[	");
		for(j = 0; j < matrix_w; j++)
		{
			printf("  %d	",inputMatrix[i][j]);
		}
		printf("]");
		printf("\n");
	}
	printf("]");
	printf("============================================================\n");
	printf("============================================================\n");
	printf("\n");
}

// transform the input input list to a 2D-matrix with the size of img_h * (img_w * 3)
INT8U ** matrix(INT8U *source, INT8U matrix_h, INT8U matrix_w)
{
	INT8U i;
	INT8U j;
    INT8U **group = (INT8U **) calloc(matrix_h, INT8U_poINT8Uer_size);
    if(group == NULL)
    {
    	printf("Calloc Error! Inside the matrix function, location1\n");
    	//exit(0);
    }
    group[0] = (INT8U*)calloc(matrix_h*matrix_w, INT8U_size);
    if(group[0] == NULL)
    {
    	printf("Calloc Error! Inside the matrix function, location2\n");
    	//exit(0);
    }
	for( i = 0; i< matrix_h; i++)
	{
		group[i] = group[0] + matrix_w*i;
		for( j = 0; j < matrix_w; j++)
		{
			group[i][j] = *(source+i*matrix_w+j);
		}
	}
	//printMatrix(group, matrix_h, matrix_w);
	return group;
}

// transform RGB value to grayscale value
// Input: a 2D-matrix with the size of img_h * (img_w * 3), img_h, img_w
// Output: a 2D-matrix with the size of img_h * img_w
INT8U ** groupV_3(INT8U **inputMatrix, INT8U matrix_row, INT8U matrix_col)
{
	INT8U i;
	INT8U j;
	INT8U **group = (INT8U**)calloc(matrix_row, INT8U_poINT8Uer_size);
    if(group == NULL)
    {
    	printf("Calloc Error! Inside the groupV_3 function, location1\n");
    	//exit(0);
    }
    group[0] = (INT8U*)calloc(matrix_col*matrix_row, INT8U_size);
    if(group[0] == NULL)
    {
    	printf("Calloc Error! Inside the groupV_3 function, location2\n");
    	//exit(0);
    }
	for ( i = 0; i < matrix_row; i++)
	{
		group[i] = group[0] + matrix_col * i;
		for(j = 0; j < matrix_col*3; j = j+3)
		{
			//one algorithm to transform RGB value to grayscale value which is suitable for operation between 16-bit variables
			//the good balance between speed and precision
			// group[i][j/3] = ((inputMatrix[i][j]*38
			// 			 +inputMatrix[i][j+1]*75
			// 			 +inputMatrix[i][j+2]*15)>>7);


			//Another grayscale algorithm, lose some precison but increase the speed
			group[i][j/3] = ((inputMatrix[i][j]>>2 )
						 +(inputMatrix[i][j+1]>>1 )
						 +(inputMatrix[i][j+2]>>3));
		}		
	}
	// printf("Grayed Matrix\n");
	// printMatrix(group, matrix_row, matrix_col);
	return group;
}

// Cropped the image into a smaller piece with the size of  cropSIZE*cropSIZE
// here startingPoINT8U_x means the index of row, startingPoINT8U_y means the index of column
INT8U** crop(INT8U startingPoINT8U_x, INT8U startingPoINT8U_y, INT8U** inputMatrix, INT8U img_w)
{
	INT8U i;
	INT8U j;
	INT8U* startingPoINT8UAddress = inputMatrix[0]+img_w * startingPoINT8U_x + startingPoINT8U_y;
	printf("Staring Point Address in memory: %d\n", startingPoINT8UAddress); 
	INT8U** group = (INT8U**)calloc(cropSIZE, INT8U_poINT8Uer_size);
    if(group == NULL)
    {
    	printf("Calloc Error! Inside the crop function, location1\n");
    	//exit(0);
    }
    group[0] = (INT8U*)calloc(cropSIZE*cropSIZE, INT8U_size);
    if(group[0] == NULL)
    {
    	printf("Calloc Error! Inside the crop function, location2\n");
    	//exit(0);
    }
	for ( i = 0; i < cropSIZE; i++)
	{
		group[i] = group[0] + i*cropSIZE;
		for( j = 0; j < cropSIZE; j++)
		{
			group[i][j] = *(startingPoINT8UAddress + img_w*i + j); 
		}		
	}
	// printf("Cropped Matrix\n");
	// printMatrix(group, cropSIZE, cropSIZE);
	return group;
}

//Optimization poINT8Uï¼š store the INT8Uermediate value to speed up the program
INT16U ** xcorr2(INT8U **inputMatrix, INT8U xPATTERN[5][5], INT8U inputMatrix_w)
{
	INT8U i;
	INT8U j;
	INT8U m;
	INT8U n;
	INT8U product = 0;
	INT16U sum;
	INT8U output_size_length = offset_size_length;
	INT8U *startingPoINT8UAddress;
	INT16U **group = (INT16U**)calloc(output_size_length, sizeof(INT16U*));
    if(group == NULL)
    {
    	printf("Calloc Error! Inside the xcorr2 function, location1\n");
    	//exit(0);
    }
	group[0] = (INT16U*)calloc(output_size_length*output_size_length, sizeof(INT16U));
    if(group[0] == NULL)
    {
    	printf("Calloc Error! Inside the xcorr2 function, location2\n");
    	//exit(0);
    }
	for ( i = 0; i < output_size_length; i++)
	{
		group[i] = group[0] + i*output_size_length;
		for( j = 0; j < output_size_length; j++)
		{
			sum = 0;
			startingPoINT8UAddress = *inputMatrix + inputMatrix_w*i + j;
			for( m = 0; m < xPATTERN_length; m++)
			{
				for( n = 0; n < xPATTERN_length; n++)
				{
					if(xPATTERN[m][n]>0)
					{
						product = *(startingPoINT8UAddress + inputMatrix_w*m + n);
					}
					else
						product = 0;
					sum = sum + product;
				}
			}
			group[i][j] = sum; 			
		}		
	}
//	printf("Xcropp2ed Matrix\n");
//	printMatrix_INT16U(group, output_size_length,output_size_length);
	return group;
}

// The inputMatrix here is the xcorred Matrix
INT8U * posMax(INT16U** inputMatrix)
{
	INT16U* output = (INT16U*)calloc(3, sizeof(INT16U));
	INT8U i;
	INT8U j;
	INT16U max = 0;
	for(i = 0; i<offset_size_length; i++)
		for(j = 0; j <offset_size_length; j++)
		{
			if(max < *(*inputMatrix+offset_size_length*i+j))
			{
				max = *(*inputMatrix+offset_size_length*i+j);
				output[0] = i;
				output[1] = j;
				output[2] = max;
			}
		}
	printf("Offset: %d, %d, %d\n",output[0],output[1], output[2]);
	return output; 
}

//Calculate the next cropped point according to the previous detected pattern position ( the middle point of the detected pattern)
INT8U *calcCoord(INT8U *previousPoINT8U, INT8U img_h, INT8U img_w )
{
	INT8U *output = (INT8U*)calloc(2, INT8U_size);
	if(output == NULL)
	{
		printf("Calloc Error! Inside the calcCoord function, location1\n");
		//exit(0);
	}
	if(previousPoINT8U[0] <= dSPAN)
		output[0] = 0;
	else if (previousPoINT8U[0] > (img_h - dSPAN))
		output[0] = img_h - cropSIZE;
	else
		output[0] = previousPoINT8U[0] - dSPAN ;
	if(previousPoINT8U[1] <= dSPAN)
		output[1] = 0;
	else if (previousPoINT8U[1] > (img_w - dSPAN))
		output[1] = img_w - cropSIZE;
	else
		output[1] = previousPoINT8U[1] - dSPAN ;
	printf("Next cropped point: %d, %d . \n",output[0],output[1]);
	return output;
}

//croPoINT8U represents the position of cropped point
INT8U * objectPos(INT8U cropPoINT8U[2], INT16U *offset)
{
	INT8U* output = (INT8U*)calloc(2, INT8U_size);
	if(output == NULL)
	{
		printf("Calloc Error! Inside the objectPos function, location1\n");
		//exit(0);
	}
	output[0] = cropPoINT8U[0] + offset[0] + 2;
	output[1] = cropPoINT8U[1] + offset[1] + 2;
	printf("Object Position: %d, %d\n",output[0],output[1]);
	return output;
}

INT8U loop()
{
	//each image is represented by a list
	//First value of the list is the value of image height
	//Second value of the list is the value of the real value of image weight
	//Third value of the list is the value of maximum color
	//Later is the R, G, B values of each pixel point, respectively, all the RGB values are oganized in an consecutive order accroding to C language convention

    INT8U counter;
	// intial point: (0,0)    
	INT8U *intial_point = (INT8U*)calloc(2, INT8U_size);
	if(intial_point == NULL)
	{
		printf("Calloc Error! Inside the intial loop, location1\n");
		//exit(0);
	}
	intial_point[0] = 0; 
	intial_point[1] = 0;
	INT8U *image;
	INT8U img_h = 0;
	INT8U img_w = 0;
	INT8U img_w_0 = 0;
	INT8U **Matrix_;
	INT8U **grayed;
	INT8U *previous_point;
	INT8U * detected;
	INT8U *coords;
	INT8U **croped;
	INT16U **xcropp2ed;
	INT16U *position_Max;
	INT8U finalResult[sequence_length][2];
    for( counter = 0; counter < sequence_length; counter++)
    {
		image = image_sequence[counter];
    	img_w = image[0];
		img_h = image[1];
		//Extended by the RGB value
		img_w_0 = img_w * 3;
		//Start from the first R value 
	    Matrix_ = matrix(image+3, img_h, img_w_0);
//		free(image);
//		image = NULL;
	    grayed = groupV_3(Matrix_, img_h, img_w);
		free(Matrix_[0]);
		free(Matrix_);
	    if(counter == 0)
	    {
	    	previous_point = intial_point; 
	    }
	    else
	    	previous_point = detected;
	    coords = calcCoord(previous_point, img_h, img_w );
	    croped = crop(coords[0],coords[1],grayed, img_w);
		free(grayed[0]);
		free(grayed);	    
	   	xcropp2ed = xcorr2(croped, xPATTERN, cropSIZE);
	   	free(croped[0]);
	   	free(croped);
	    position_Max = posMax(xcropp2ed);
	    free(xcropp2ed[0]);
	    free(xcropp2ed);
	    detected = objectPos(coords,position_Max);
	    free(coords);
	    free(position_Max);
	    finalResult[counter][0] = detected[0];
	    finalResult[counter][1] = detected[1];
	    //free(detected);
	    printf("Test image %d : %d , %d \n\n",counter,finalResult[counter][0],finalResult[counter][1]);
    }

	return 0;
}

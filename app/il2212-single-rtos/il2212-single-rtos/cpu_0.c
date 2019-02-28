// Skeleton for lab 2
// 
// Task 1 writes periodically RGB-images to the shared memory
//
// No guarantees provided - if bugs are detected, report them in the Issue tracker of the github repository!

#include <stdio.h>
#include "altera_avalon_performance_counter.h"
#include "includes.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"
#include "system.h"
#include "io.h"

#include "ascii_gray.h"
#include "functions.h"

#define DEBUG 0
#define ITERATIONS 100
char current_iteration = 0;


#define HW_TIMER_PERIOD 100 /* 100ms */
#define OS_OPT_TIME_PERIODIC 1

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    StartTask_Stack[TASK_STACKSIZE]; 
OS_STK    taskGRAYSDF_stk[TASK_STACKSIZE];
OS_STK    taskCROPSDF_stk[TASK_STACKSIZE];
OS_STK    taskXCORR2SDF_stk[TASK_STACKSIZE];
OS_STK    taskGETOFFSETSDF_stk[TASK_STACKSIZE];
OS_STK    taskCALCPOSSDF_stk[TASK_STACKSIZE];
OS_STK    taskDELAYSDF_stk[TASK_STACKSIZE];
OS_STK    taskCALCCOORDSDF_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define STARTTASK_PRIO      		1
#define TASK1_PRIORITY      		10
#define TASKGRAYSDF_PRIORITY      	11
#define TASKCROPSDF_PRIORITY      	12
#define TASKXCORR2SDF_PRIORITY      13
#define TASKGETOFFSETSDF_PRIORITY   14
#define TASKCALCPOSSDF_PRIORITY     15
#define TASKDELAYSDF_PRIORITY      	16
#define TASKCALCCOORDSDF_PRIORITY   17

/* Definition of Task Periods (ms) */
#define TASK1_PERIOD 			10000
#define TASKGRAYSDF_PERIOD 		100
#define TASKCROPSDF_PERIOD 		100
#define TASKXCORR2SDF_PERIOD 	100
#define TASKGETOFFSETSDF_PERIOD 100
#define TASKCALCPOSSDF_PERIOD 	100
#define TASKDELAYSDF_PERIOD 	100
#define TASKCALCCOORDSDF_PERIOD 100


//#define SECTION_1 				1
#define SECTION_GRAYSDF 		1
#define SECTION_CROPSDF			2
#define SECTION_XCORR2SDF		3
//#define SECTION_GETOFFSETSDF	4
#define SECTION_CALCCOORDSDF	4
#define SECTION_CALCPOSSDF		5
//#define SECTION_placeholder		7


/* Signals - Semaphores */
OS_EVENT *TaskGRAYSDFSem;
OS_EVENT *TaskCROPSDFSem;
OS_EVENT *TaskXCORR2SDFSem;
OS_EVENT *TaskGETOFFSETSDFSem;
OS_EVENT *TaskCALCPOSSDFSem;
OS_EVENT *TaskDELAYSDFSem;
OS_EVENT *TaskCALCCOORDSDFSem;
OS_EVENT *TaskPrintReportSem;

// Semaphores
OS_EVENT *Task1TmrSem;

/* Signals - Message queues */
OS_EVENT *MQ_GRAYSDF;
OS_EVENT *MQ_CROPSDF;
OS_EVENT *MQ_XCORR2SDF;
OS_EVENT *MQ_GETOFFSETSDF;
OS_EVENT *MQ_CALCPOSSDF;
OS_EVENT *MQ_DELAYSDF;
OS_EVENT *MQ_CALCCOORDSDF;
OS_EVENT *MQ_PrintReport;


// SW-Timer
OS_TMR *Task1Tmr;

/*
 * Global variables
 */
int delay; // Delay of HW-timer
INT16U ** current_image_matrix;

INT8U *image;
INT8U img_h;
INT8U img_w;
INT8U img_w_0;
INT8U **Matrix_;
INT8U **grayed;
INT8U *previous_point;
INT8U * detected;
INT8U *coords;
INT8U **croped;
INT16U **xcropp2ed;
INT8U *position_Max;
INT8U counter = 0;
/*
 * Example function for copying a p3 image from sram to the shared on-chip mempry
 */
void sram2sm_p3(unsigned char* base)
{
	int x, y;
	unsigned char* shared;

	shared = (unsigned char*) SHARED_ONCHIP_BASE;

	int size_x = *base++;
	int size_y = *base++;
	int max_col= *base++;
	*shared++  = size_x;
	*shared++  = size_y;
	*shared++  = max_col;
	printf("The image is: %d x %d!! \n", size_x, size_y);
	for(y = 0; y < size_y; y++)
	for(x = 0; x < size_x; x++)
	{
		*shared++ = *base++; 	// R
		*shared++ = *base++;	// G
		*shared++ = *base++;	// B
	}
}


/*
 * ISR for HW Timer
 */
alt_u32 alarm_handler(void* context)
{
  OSTmrSignal(); /* Signals a 'tick' to the SW timers */
  
  return delay;
}


/* Timer Callback Functions */ 
void Task1TmrCallback (void *ptmr, void *callback_arg){
  OSSemPost(Task1TmrSem);
}

void task1(void* pdata)
{
	INT8U err;
	INT8U value=0;
	INT8U current_image=0;

	while (1)
	{ 
		/* Extract the x and y dimensions of the picture */
		unsigned char i = *image_sequence[current_image];
		unsigned char j = *(image_sequence[current_image]+1);


		
		/* Measurement here */
//		sram2sm_p3(image_sequence[current_image]);

		/* Just to see that the task compiles correctly */
		IOWR_ALTERA_AVALON_PIO_DATA(LEDS_GREEN_BASE,value++);

		/* Make available the next task */
		OSSemPost(TaskGRAYSDFSem);

		OSSemPend(Task1TmrSem, 0, &err);


		/* Increment the image pointer */
		current_image=(current_image+1) % sequence_length;




	}
}


// Transform the input RGB matrix to a greyscale matrix, the R,G,B valuses are placed consecutively
void taskGRAYSDF(void* pdata)
{
	INT8U err;

	while (1)
	{
		OSSemPend(TaskGRAYSDFSem, 0, &err);

		if(counter == 4 )
		{
			if(DEBUG) {
				printf("Program finished - debug mode\n");
				exit(0);
			} else {
				counter = 0;
			}
		}

		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_GRAYSDF);

		/* Measurement here */
		if(DEBUG) {
			printf("Task GRAYSDFF\n");
		}

		image = image_sequence[counter];
    	img_w = image[0];
		img_h = image[1];
		//Extended by the RGB value
		img_w_0 = img_w * 3;
		//Start from the first R value 
	    Matrix_ = matrix(image+3, img_h, img_w_0);

	    grayed = groupV_3(Matrix_, img_h, img_w);
		free(Matrix_[0]);
		free(Matrix_);
		counter += 1;

		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_GRAYSDF);

		OSSemPost(TaskCROPSDFSem);
		OSSemPost(TaskCALCCOORDSDFSem);
	}
}


// cut the image to a small piece of given size
void taskCROPSDF(void* pdata)
{
	INT8U err;

	while (1)
	{
		OSSemPend(TaskCROPSDFSem, 0, &err);
		OSSemPend(TaskCROPSDFSem, 0, &err);

		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_CROPSDF);

		/* Measurement here */
		if(DEBUG) {
			printf("Task CROPSDF\n");
		}
	    croped = crop(coords[0],coords[1],grayed, img_w);
//		free(grayed[0]);
//		free(grayed);

		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_CROPSDF);

		OSSemPost(TaskXCORR2SDFSem);

	}
}


// calculate the coordinates of the next cropping point according to the previous object position
void taskXCORR2SDF(void* pdata)
{
	INT8U err;

	while (1)
	{
		OSSemPend(TaskXCORR2SDFSem, 0, &err);

		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_XCORR2SDF);

		/* Measurement here */
		if(DEBUG) {
			printf("Task XCORR2SDF\n");
		}
	   	xcropp2ed = xcorr2(croped, xPATTERN, cropSIZE);
	   	free(croped[0]);
	   	free(croped);

		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_XCORR2SDF);

		OSSemPost(TaskGETOFFSETSDFSem);
	}
}

void taskGETOFFSETSDF(void* pdata)
{
	INT8U err;

	while (1)
	{
		OSSemPend(TaskGETOFFSETSDFSem, 0, &err);


		/* Measurement here */
		if(DEBUG) {
			printf("Task GETOFFSETSDF\n");
		}
	    position_Max = posMax(xcropp2ed);
	    free(xcropp2ed[0]);
	    free(xcropp2ed);

		OSSemPost(TaskCALCPOSSDFSem);
	}
}

void taskCALCPOSSDF(void* pdata)
{
	INT8U err;

	while (1)
	{
		OSSemPend(TaskCALCPOSSDFSem, 0, &err);
		OSSemPend(TaskCALCPOSSDFSem, 0, &err);

		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_CALCPOSSDF);

		/* Measurement here */
		if(DEBUG) {
			printf("Task CALCPOSSDF\n");
		}
	    detected = objectPos(coords,position_Max);
	    free(coords);
	    free(position_Max);
	    if(DEBUG) {
	    	printf("printMatrix\n");
//	    	printMatrix(grayed[0], image_sequence[counter][0], image_sequence[counter][1]);
//	    	printAscii(grayed[0], image_sequence[counter][0], image_sequence[counter][1]);
//	    	printf("------------------------------------\n\n");
//	    	printAsciiHidden(grayed[0], image_sequence[counter][0], image_sequence[counter][1],
//	    			detected[0], detected[1], 5, 0);
	    }
	    printf("Test image %d : %d , %d \n\n", counter, detected[0], detected[1]);
	    free(grayed[0]);
		free(grayed);
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_CALCPOSSDF);

		if(!DEBUG) {
			current_iteration++;
			if(current_iteration >= ITERATIONS) {
				PERF_STOP_MEASURING (PERFORMANCE_COUNTER_0_BASE);
				/* Print report */
				perf_print_formatted_report
					(PERFORMANCE_COUNTER_0_BASE,
					ALT_CPU_FREQ,        // defined in "system.h"
					5,                   // How many sections to print
					"graySDF",
					"cropSDF",
					"xcorr2SDF",
					"calcCoordSDF",
					"calcPosSDF"
					);
				printf("Program finished - performance mode");
				//stop the program by causing an artificial deadlock
				exit(0);
			}
		}
		OSSemPost(TaskDELAYSDFSem);
		if(!DEBUG) {
			OSSemPost(Task1TmrSem);
		}
	}
}


// Initial delay token
void taskDELAYSDF(void* pdata)
{
	INT8U err;
	INT8U *intial_point = (INT8U*)calloc(2, INT8U_size);
	if(intial_point == NULL)
	{
		printf("Calloc Error! Inside the intial loop, location1\n");
		//exit(0);
	}
	intial_point[0] = 0; 
	intial_point[1] = 0;

	detected = intial_point;


	while (1)
	{
		OSSemPend(TaskDELAYSDFSem, 0, &err);

		previous_point = detected;

		OSSemPost(TaskCALCCOORDSDFSem);

	}
}


// calculate the coordinates of the next cropping point according to the previous object position
void taskCALCCOORDSDF(void* pdata)
{
	INT8U err;

	while (1)
	{
		OSSemPend(TaskCALCCOORDSDFSem, 0, &err);
		OSSemPend(TaskCALCCOORDSDFSem, 0, &err);

		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_CALCCOORDSDF);

		/* Measurement here */
		if(DEBUG) {
			printf("Task CALCCOORDSDF\n");
		}
	    coords = calcCoord(previous_point, img_h, img_w );
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_CALCCOORDSDF);

		OSSemPost(TaskCROPSDFSem);
		OSSemPost(TaskCALCPOSSDFSem);

	}
}




void StartTask(void* pdata)
{
  INT8U err;
  void* context;

  static alt_alarm alarm;     /* Is needed for timer ISR function */

  /* Base resolution for SW timer : HW_TIMER_PERIOD ms */
  delay = alt_ticks_per_second() * HW_TIMER_PERIOD / 1000;
  printf("delay in ticks %d\n", delay);

  /*
   * Create Hardware Timer with a period of 'delay'
   */
  if (alt_alarm_start (&alarm,
      delay,
      alarm_handler,
      context) < 0)
      {
          printf("No system clock available!n");
      }

  /*
   * Create and start Software Timer
   */

   //Create Task1 Timer
   Task1Tmr = OSTmrCreate(0, //delay
                            TASK1_PERIOD/HW_TIMER_PERIOD, //period
                            OS_TMR_OPT_PERIODIC,
                            Task1TmrCallback, //OS_TMR_CALLBACK
                            (void *)0,
                            "Task1Tmr",
                            &err);

   if (DEBUG) {
    if (err == OS_ERR_NONE) { //if creation successful
      printf("Task1Tmr created\n");
    }
   }


   /*
    * Start timers
    */

   //start Task1 Timer
   if(DEBUG) {
	   OSTmrStart(Task1Tmr, &err);
   }

   if (DEBUG) {
    if (err == OS_ERR_NONE) { //if start successful
      printf("Task1Tmr started\n");
    }
   }



   /*
   * Creation of Kernel Objects
   */

  if (!DEBUG) {
	  Task1TmrSem 			= OSSemCreate(1);
  } else {
	  Task1TmrSem 			= OSSemCreate(0);
  }
  TaskGRAYSDFSem 		= OSSemCreate(0);
  TaskCROPSDFSem 		= OSSemCreate(0);
  TaskXCORR2SDFSem 		= OSSemCreate(0);
  TaskGETOFFSETSDFSem 	= OSSemCreate(0);
  TaskCALCPOSSDFSem 	= OSSemCreate(0);
  TaskDELAYSDFSem 		= OSSemCreate(1);
  TaskCALCCOORDSDFSem 	= OSSemCreate(0);
  TaskPrintReportSem 	= OSSemCreate(0);

   /*
   * Creation of message queues
   */
  INT8U *DELAY_Q[2];
  MQ_DELAYSDF 			= OSQCreate(&DELAY_Q[0], 2);




  /*
   * Create statistics task
   */

  OSStatInit();

  /*
   * Creating Tasks in the system
   */



//This task is not needed for the single-rtos lab

  err=OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("Task1 created\n");
    }
   }





  err=OSTaskCreateExt(taskGRAYSDF,
                  NULL,
                  (void *)&taskGRAYSDF_stk[TASK_STACKSIZE-1],
                  TASKGRAYSDF_PRIORITY,
                  TASKGRAYSDF_PRIORITY,
                  taskGRAYSDF_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("TaskGRAYSDF created\n");
    }
   }

  err=OSTaskCreateExt(taskCROPSDF,
                  NULL,
                  (void *)&taskCROPSDF_stk[TASK_STACKSIZE-1],
                  TASKCROPSDF_PRIORITY,
                  TASKCROPSDF_PRIORITY,
                  taskCROPSDF_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("TaskCROPSDF created\n");
    }
   }

  err=OSTaskCreateExt(taskXCORR2SDF,
                  NULL,
                  (void *)&taskXCORR2SDF_stk[TASK_STACKSIZE-1],
                  TASKXCORR2SDF_PRIORITY,
                  TASKXCORR2SDF_PRIORITY,
                  taskXCORR2SDF_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("TaskXCORR2SDF created\n");
    }
   }

  err=OSTaskCreateExt(taskGETOFFSETSDF,
                  NULL,
                  (void *)&taskGETOFFSETSDF_stk[TASK_STACKSIZE-1],
                  TASKGETOFFSETSDF_PRIORITY,
                  TASKGETOFFSETSDF_PRIORITY,
                  taskGETOFFSETSDF_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("TaskGETOFFSETSDF created\n");
    }
   }

  err=OSTaskCreateExt(taskCALCPOSSDF,
                  NULL,
                  (void *)&taskCALCPOSSDF_stk[TASK_STACKSIZE-1],
                  TASKCALCPOSSDF_PRIORITY,
                  TASKCALCPOSSDF_PRIORITY,
                  taskCALCPOSSDF_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("TaskCALCPOSSDF created\n");
    }
   }

  err=OSTaskCreateExt(taskDELAYSDF,
                  NULL,
                  (void *)&taskDELAYSDF_stk[TASK_STACKSIZE-1],
                  TASKDELAYSDF_PRIORITY,
                  TASKDELAYSDF_PRIORITY,
                  taskDELAYSDF_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("TaskDELAYSDF created\n");
    }
   }

  err=OSTaskCreateExt(taskCALCCOORDSDF,
                  NULL,
                  (void *)&taskCALCCOORDSDF_stk[TASK_STACKSIZE-1],
                  TASKCALCCOORDSDF_PRIORITY,
                  TASKCALCCOORDSDF_PRIORITY,
                  taskCALCCOORDSDF_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("TaskCALCCOORDSDF created\n");
    }
   }

  printf("All Tasks and Kernel Objects generated!\n");

  PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
  PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);

  /* Task deletes itself */

  OSTaskDel(OS_PRIO_SELF);
}


int main(void) {


  printf("MicroC/OS-II-Vesion: %1.2f\n", (double) OSVersion()/100.0);



  OSTaskCreateExt(
	 StartTask, // Pointer to task code
         NULL,      // Pointer to argument that is
                    // passed to task
         (void *)&StartTask_Stack[TASK_STACKSIZE-1], // Pointer to top
						     // of task stack
         STARTTASK_PRIO,
         STARTTASK_PRIO,
         (void *)&StartTask_Stack[0],
         TASK_STACKSIZE,
         (void *) 0,
         OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

  OSStart();

  return 0;
}







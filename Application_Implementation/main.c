/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"

#include "queue.h"
/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

typedef struct message{
	char *string;
	int size;
}message;
static QueueHandle_t msg_queue;
static const uint8_t msg_queue_len = 20;


int deadline_misses[6] = {0};

void task_1(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t start_time = 0, end_time = 0;
	int button_1_state = 1;
	int current_state = 0;
	message msg1;
	message *pMsg1 = &msg1;
	
	for(;;){
		GPIO_write(PORT_0, PIN1, PIN_IS_HIGH);
		start_time = xTaskGetTickCount();
		
		/* START */
		current_state = GPIO_read(PORT_1, PIN0);
		/* a change occured */
		if(current_state != button_1_state){
			/* a rising edge detected on button 1 */
			if(current_state){
				msg1.string = "Rising Edge On Button 1\n";
				msg1.size = 24;
				xQueueSend(msg_queue, (void*) &pMsg1, 0);
			}
			/* a falling edge detected on button 1 */
			else{
				msg1.string = "Falling Edge On Button 1\n";
				msg1.size = 25;
				xQueueSend(msg_queue, (void*) &pMsg1, 0);
			}

			button_1_state = current_state;			
		}
		/* END */
		
		end_time = xTaskGetTickCount();
		if(end_time - start_time > 50) deadline_misses[0]++;		
		GPIO_write(PORT_0, PIN1, PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime, 50);

		

	}
}


void task_2(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();	
	TickType_t start_time = 0, end_time = 0;
	int button_2_state = 1;
	int current_state = 0;
	message msg2;
	message *pMsg2 = &msg2;
	
	for(;;){
		GPIO_write(PORT_0, PIN2, PIN_IS_HIGH);	
		start_time = xTaskGetTickCount();		
		
		/* START */
		current_state = GPIO_read(PORT_1, PIN1);
		/* a change occured */
		if(current_state != button_2_state){
			/* a rising edge detected on button 2 */
			if(current_state){
				msg2.string = "Rising Edge On Button 2\n";
				msg2.size = 24;
				xQueueSend(msg_queue, (void*) &pMsg2, 0);
			}
			/* a falling edge detected on button 2 */
			else{
				msg2.string = "Falling Edge On Button 2\n";
				msg2.size = 25;
				xQueueSend(msg_queue, (void*) &pMsg2, 0);
			}
			button_2_state = current_state;			
		}
		/* END */
		
		end_time = xTaskGetTickCount();
		if(end_time - start_time > 50) deadline_misses[1]++;		
		GPIO_write(PORT_0, PIN2, PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime, 50);

	}
}


void task_3(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();	
	TickType_t start_time = 0, end_time = 0;
	message msg3;
	message *pMsg3 = &msg3;
	msg3.string = "Periodic String!\n";
	msg3.size = 17;
	for(;;){
		GPIO_write(PORT_0, PIN3, PIN_IS_HIGH);		
		start_time = xTaskGetTickCount();
		
		/* START */
		xQueueSend(msg_queue, (void*) &pMsg3, 0);
		/* END */
		
		end_time = xTaskGetTickCount();
		if(end_time - start_time > 100) deadline_misses[2]++;		
		GPIO_write(PORT_0, PIN3, PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime, 100);
	}
}

void task_4(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();	
	TickType_t start_time = 0, end_time = 0;

	message *received_msg;
	
	for(;;){
		GPIO_write(PORT_0, PIN4, PIN_IS_HIGH);	
		start_time = xTaskGetTickCount();		
		
		/* START */
		while(xQueueReceive(msg_queue, (void *) &received_msg, 0) == pdTRUE){
			vSerialPutString((const signed char *)received_msg->string, received_msg->size);
		}
		/* END */
		
		end_time = xTaskGetTickCount();
		if(end_time - start_time > 20) deadline_misses[3]++;
		GPIO_write(PORT_0, PIN4, PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime, 20);
	}
}

/* Excution time 5ms */
void task_5(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();	
	TickType_t start_time = 0, end_time = 0;
	
	int i;
	for(;;){
		GPIO_write(PORT_0, PIN5, PIN_IS_HIGH);
		start_time = xTaskGetTickCount();
	
		/* START */
		for(i = 0; i < 33220; i++){
			i = i;
		}
		/* END */
		
		end_time = xTaskGetTickCount();
		if(end_time - start_time > 10) deadline_misses[4]++;
		GPIO_write(PORT_0, PIN5, PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime, 10);
	}
}

/* Excution time 12ms */
void task_6(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t start_time = 0, end_time = 0;
	
	int i;
	for(;;){
		GPIO_write(PORT_0, PIN6, PIN_IS_HIGH);		
		start_time = xTaskGetTickCount();	
		
		/* START */
		for(i = 0; i < 79740; i++){
			i = i;
		}
		/* END */
		
		end_time = xTaskGetTickCount();
		if(end_time - start_time > 100) deadline_misses[5]++;	
		GPIO_write(PORT_0, PIN6, PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime, 100);
	}
}

/* TICK HOOK */
void vApplicationTickHook(void){
	GPIO_write(PORT_0, PIN0, PIN_IS_HIGH);
	GPIO_write(PORT_0, PIN0, PIN_IS_LOW);
}

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
TaskHandle_t task_1_handle;
TaskHandle_t task_2_handle;
TaskHandle_t task_3_handle;
TaskHandle_t task_4_handle;
TaskHandle_t task_5_handle;
TaskHandle_t task_6_handle;
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	/* CREATE THE QUEUE */
	msg_queue = xQueueCreate( msg_queue_len, sizeof( message * ) );
	
   /* Create Tasks here */
	xTaskPeriodicCreate(task_1,
							"Button_1_Monitor",
							100,
							(void *) 0,
								2,
							&task_1_handle,
							50);
							
	xTaskPeriodicCreate(task_2,
							"Button_2_Monitor",
							100,
							(void *) 0,
								2,
							&task_2_handle,
							50);		
							
	xTaskPeriodicCreate(task_3,
							"Periodic_Transmitter",
							100,
							(void *) 0,
								2,
							&task_3_handle,
							100);
							
	xTaskPeriodicCreate(task_4,
							"Uart_Receiver",
							100,
							(void *) 0,
								2,
							&task_4_handle,
							20);
							
		xTaskPeriodicCreate(task_5,
							"Load_1_Simulation",
							100,
							(void *) 0,
								2,
							&task_5_handle,
							10);
							
	xTaskPeriodicCreate(task_6,
							"Load_2_Simulation",
							100,
							(void *) 0,
								2,
							&task_6_handle,
							100);							
							

														
	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/



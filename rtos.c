#include <LPC17xx.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rtos.h"
#include "context.h"

#define CONTEXT_STACK_SIZE 16
#define CONTEXT_STACK_DEFAULT_VAL 0x00000000
#define PSR_DEFULT 0x01000000

uint32_t* MAIN_STACK_PTR;
uint32_t* TASK_STACK_PTRS [MAX_TASK_NUM];

TCB_T TCPS[MAX_TASK_NUM];
uint32_t current_task_id, next_task_id; //current task is the task that is running right now
uint32_t num_task; //number of tasks declared, increment each time when a new task is declared

TCB_QUEUE_T ready_queue;
TCB_QUEUE_T blocked_queue;

bool scheduler_started = false;


void SysTick_Handler(void) {
   __disable_irq();
	
	if(scheduler_started){
		
		
	}
	 __enable_irq();
}

void pendSV_Handler(void){
	__disable_irq();
	
	TCPS[current_task_id].stack_ptr = (uint32_t*) storeContext();
	restoreContext((uint32_t)TCPS[current_task_id].stack_ptr);
	
	__enable_irq();
}

void TCB_init(int TCB_id){
	TCB_T TCB ={
		.id = TCB_id,
		.priority = 0,
		.stack_ptr = TASK_STACK_PTRS[TCB_id],
		.state = TERMINATED,
		.ready_queue = &ready_queue,
		.blocked_queue = &blocked_queue,
	};
	
	TCPS[TCB_id] = TCB;
}

void RTOS_init(void){
	//init main stack pointer and each task stack pointer
	//also init the TCBs
	uint32_t* MAIN_STACK_PTR_ADDR = (uint32_t*)0x00;
	MAIN_STACK_PTR = (uint32_t*)*MAIN_STACK_PTR_ADDR;
	
	if(MAX_TASK_NUM > 0){
		TASK_STACK_PTRS[0] = MAIN_STACK_PTR + MAIN_STACK_SIZE;
		TCB_init(0);
		
		for(int i=1; i< MAX_TASK_NUM; i++){
			TASK_STACK_PTRS[i] = TASK_STACK_PTRS[i-1] + TASK_STACK_SIZE;
			TCB_init(i);
		}
	}
	
	//copy contents in the main stack to the main task stack -> first task becomes the main task
	uint32_t* main_task_stack_ptr = TASK_STACK_PTRS[0];
	for(uint32_t* main_stack=MAIN_STACK_PTR; main_stack> (uint32_t*)__get_MSP(); main_stack--){
		*main_task_stack_ptr = *main_stack;
		main_task_stack_ptr--;
	}
	
	//set MSP to base address of main stack
	__set_MSP((uint32_t)MAIN_STACK_PTR);
	
	//swicth to PSP and set to the top of the stack for the main task
	CONTROL_Type control;
	control.w = __get_CONTROL();
	control.b.SPSEL = 1;
	__set_CONTROL(control.w);
	
	__set_PSP((uint32_t)main_task_stack_ptr);
	

	//set current task to the first task
	num_task = 1;
	current_task_id = 0;
	TCPS[0].state = RUNNING;
	
	//set timer to interrupt every 1ms
	SysTick_Config(SystemCoreClock/1000);
}


void create_task(rtosTaskFunc_t task_func, void* args, uint32_t priority){
	
	//set up the stack for the new task
	uint32_t* task_stack_base_ptr = TASK_STACK_PTRS[num_task]; //num task will be the same as the id of the task
																														 //the first task is already declared in RTOS_init
	
	for(int i=0; i<CONTEXT_STACK_SIZE; i++){
		*(task_stack_base_ptr+i) = CONTEXT_STACK_DEFAULT_VAL;
	}
	
	*task_stack_base_ptr = PSR_DEFULT; //PSR
	*(task_stack_base_ptr+1) = (int)task_func; //PC
	*(task_stack_base_ptr+7) =  (int)args;// R0
	
	TCPS[num_task].state = READY;
	TCPS[num_task].priority = priority;
	
	enqueue_TCB(&ready_queue, &TCPS[num_task]);
	
	num_task++;
}


//this should be called after rtos_init in main
void RTOS_start(void){
	scheduler_started = true;
	
	//blocks indifinitely so we don't exit main
	for(;;);
}

void enqueue_TCB(TCB_QUEUE_T* TCB_queue, TCB_T* TCB){
}

void dequeue_TCB(TCB_QUEUE_T* TCB_queue, TCB_T* TCB){
}

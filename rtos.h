#ifndef __RTOS_H
#define __RTOS_H

#include <stdint.h>

#define MAX_TASK_NUM 6
#define MAIN_STACK_SIZE 2000 //2000KiB
#define TASK_STACK_SIZE 1000 //1000KiB

typedef struct TCB_QUEUE_T TCB_QUEUE_T;


typedef enum{
	RUNNING,
	READY,
	BLOCKED,
	TERMINATED,
}TASK_STATE;

typedef struct{
	uint32_t id; // start from 0 - MAX_TASK_NUM
	uint32_t priority; // start from 0, lower the value of priority, lower the priority
	uint32_t* stack_ptr;
	TASK_STATE state;
	TCB_QUEUE_T* ready_queue;
	TCB_QUEUE_T* blocked_queue;
} TCB_T;


struct TCB_QUEUE_T{
	TCB_T* head;
	TCB_T* tail;
	int length;
};


void TCB_init(int TCB_id);
void RTOS_init(void);

typedef void(*rtosTaskFunc_t)(void* args);
void create_task(rtosTaskFunc_t task_func, void* args, uint32_t priority);

void enqueue_TCB(TCB_QUEUE_T* TCB_queue, TCB_T* TCB);
void dequeue_TCB(TCB_QUEUE_T* TCB_queue, TCB_T* TCB);

void RTOS_start(void);



#endif 
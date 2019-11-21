/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool


typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue
static task task_queue[QUEUE_SIZE+1]; // another space to determine full or empty
int head=0;  // the head of the queue
int tail=0;  // the tail of the queue
int task_count = 0;

// the worker bee
pthread_t bee[NUMBER_OF_THREADS];


pthread_mutex_t queue_lock; // lock for enqueue and dequeue
sem_t sem_count; // semaphore for notification

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
	pthread_mutex_lock(&queue_lock);
	if (task_count == QUEUE_SIZE)
	{
		pthread_mutex_unlock(&queue_lock);
		return 1;
	}
	task_queue[tail] = t;
	tail = (tail + 1) % (QUEUE_SIZE+1);
	pthread_mutex_unlock(&queue_lock);
	task_count += 1;
	return 0;
}

// remove a task from the queue
task dequeue() 
{
	pthread_mutex_lock(&queue_lock);
	task work_to_do = task_queue[head];
	head = (head + 1) % (QUEUE_SIZE + 1);
	task_count -= 1;
	pthread_mutex_unlock(&queue_lock);
    	return work_to_do;
}

// the worker thread in the thread pool
void *worker(void *param)
{
	task work_to_do;
	while(TRUE){
		sem_wait(&sem_count);
		work_to_do = dequeue();
		
		execute(work_to_do.function, work_to_do.data);	
	}
   
    // pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
	int error;
	task work_to_do;
    	work_to_do.function = somefunction;
    	work_to_do.data = p;
	
	error = enqueue(work_to_do);
	if (!error)
	{
		sem_post(&sem_count);
	}
    	return error;
}

// initialize the thread pool
void pool_init(void)
{
	pthread_mutex_init(&queue_lock, NULL);
	sem_init(&sem_count, 0, 0);
	int i;
	for (i=0; i<NUMBER_OF_THREADS; i++){
    		pthread_create(&bee[i],NULL,worker,NULL);
	}
}

// shutdown the thread pool
void pool_shutdown(void)
{
	int i;
	for (i=0; i<NUMBER_OF_THREADS; i++){
		pthread_cancel(bee[i]);  //cancel without signal
		pthread_join(bee[i], NULL);
	}
	sem_destroy(&task_count);
    	pthread_mutex_destroy(&queue_lock);
}

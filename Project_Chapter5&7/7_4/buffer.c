#include "buffer.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

buffer_item buffer[BUFFER_SIZE];
int head;
int tail;
int count_item;
pthread_mutex_t lock;
sem_t empty;
sem_t full;

void init_buffer(){
	head = 0;
	tail = 0;
	count_item = 0;
	pthread_mutex_init(&lock, NULL);
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, BUFFER_SIZE);
}

int insert_item(buffer_item item){
	pthread_mutex_lock(&lock);
	if (count_item == BUFFER_SIZE)
	{
		pthread_mutex_unlock(&lock);
		return 1;
	}
	buffer[tail] = item;
	tail = (tail + 1) % BUFFER_SIZE;
	count_item += 1;
	pthread_mutex_unlock(&lock);
	return 0;
}

int remove_item(buffer_item *item){
	pthread_mutex_lock(&lock);
	if (count_item == 0){
		pthread_mutex_unlock(&lock);
		return 1;
	}
	*item = buffer[head];
	head = (head + 1 ) % BUFFER_SIZE;
	count_item -= 1;
	pthread_mutex_unlock(&lock);
	return 0;
}

void *producer(void *param){
	buffer_item item;
	int temp_sleep_time;
	while(1){
		temp_sleep_time = 1 + rand() % MAX_SLEEP_TIME;
		// printf("Producer: temp sleep time = %d. \n", temp_sleep_time);
		sleep(temp_sleep_time);
		item = rand();
		if (insert_item(item)) 
			printf("producer: report error condition\n");
		else
			printf("producer produced %d\n", item);
	}
	return NULL;
}
void *consumer(void *param){
	buffer_item item;
	int temp_sleep_time;
	while(1){
		temp_sleep_time = 1 + rand() % MAX_SLEEP_TIME;
		// printf("Consumer: temp sleep time = %d. \n", temp_sleep_time);
		sleep(temp_sleep_time);
		if (remove_item(&item)) 
			printf("consumer: report error condition\n");
		else
			printf("consumer consumed %d\n", item);
		
	}
	return NULL;
}


void destroy_buffer(){
	sem_destroy(&empty);
	sem_destroy(&full);
    	pthread_mutex_destroy(&lock);
}

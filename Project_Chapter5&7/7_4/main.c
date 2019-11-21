#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "buffer.h"

int main(int argc, char *args[]){
	int sleep_time = atoi(args[1]);
	int num_of_producers = (atoi(args[2]));
	int num_of_consumers = atoi(args[3]);
	int i;
	printf("sleep time = %d, number of producers = %d, number of consumers = %d.\n", sleep_time, num_of_producers, num_of_consumers);

	init_buffer();
	printf("Initialization finished.\n");

	pthread_t *producers = malloc(num_of_producers * sizeof(pthread_t));
	for (i=0; i < num_of_producers; i++){
		pthread_create(&producers[i], NULL, producer, NULL);
	}

	pthread_t *consumers = malloc(num_of_consumers * sizeof(pthread_t));
    	for(i=0; i != num_of_consumers; i++) {
        	pthread_create(&consumers[i], NULL, consumer, NULL);
    	}

    	printf("Sleep for %u second(s) before exit.\n", sleep_time);
    	sleep(sleep_time);

	for (i=0; i < num_of_producers; i++){
		pthread_cancel(producers[i]);
		pthread_join(producers[i], NULL);
	}
	for (i=0; i<num_of_consumers; i++){
		pthread_cancel(consumers[i]);
		pthread_join(consumers[i], NULL);
	}
	
	sleep(2);
	destroy_buffer();
	printf("Destroy finished.\n");
	return 0;
}

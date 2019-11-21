/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
	int num_of_task;
	int i;
	printf("Please input the number of tasks: ");
        scanf("%d", &num_of_task);

    // initialize the thread pool
    pool_init();

	struct data *data_arr = malloc(sizeof(struct data) * num_of_task);
	for (i=0; i<num_of_task; i++){
		data_arr[i].a = i;
		data_arr[i].b = i+1;
		while (pool_submit(&add, &data_arr[i])); // if error, return 1, keep submitting
	}

	printf("All work has been submited.\n");



    pool_shutdown();

	free(data_arr);
    return 0;
}

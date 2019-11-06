/**
 * A pthread program illustrating how to
 * create a simple thread and some of the pthread API
 * This program implements the summation function where
 * the summation operation is run as a separate thread.
 *
 * Most Unix/Linux/OS X users
 *  gcc -o thread thrd-posix.c -lpthread  (in order to compile)
   ./thread  (in order to run)
 *
 * Figure 4.11
 *
 * @author Gagne, Galvin, Silberschatz
 * Operating System Concepts  - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int arr_size = 8;
int arr[10] = {2, 8, 5, 7, 1, 4, 0, 9}; /* this data is shared by the thread(s) */

void *runner_sort(void* );
void *runner_merge(void* );
void display_arr(int* , int , int );
void quick_sort(int *, int , int );
int partition (int *, int , int );
void merge(int*, int, int, int);

typedef struct 
{
	int start;
    	int end;
}param_sort;

typedef struct 
{
    	int sp1;
	int sp2;
	int ep;
}param_merge;

int main(int argc, int *argv[])
{
	int sp1 = 0;
	int sp2 = arr_size / 2;
	int ep = arr_size - 1;

	param_sort param1, param2;
	param_merge param_m;
	param1.start = sp1;
	param1.end = sp2 - 1; 
	param2.start = sp2;
	param2.end = ep;
	param_m.sp1 = sp1;
	param_m.sp2 = sp2;
	param_m.ep = ep;

	printf("original array: \n");
	display_arr(arr, 0, arr_size-1);
	printf("starting point1 = %d, starting point2 = %d, end point = %d, array size = %d\n", sp1, sp2, ep, arr_size);

	pthread_t tid_s1, tid_s2; /* create two sorting threads*/
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_create(&tid_s1, &attr, runner_sort, (void*)&param1);
	pthread_create(&tid_s2, &attr, runner_sort, (void*)&param2);

	pthread_join(tid_s1, NULL);
	pthread_join(tid_s2, NULL);
	printf("after executing two sorting threads:\n");
	display_arr(arr, 0, arr_size-1);

	pthread_t tid_m;
	
	pthread_create(&tid_m, &attr, runner_merge, (void*)&param_m);
	pthread_join(tid_m, NULL);
	printf("after executing the merging thread:\n");
	display_arr(arr, 0, arr_size-1);
}

void *runner_sort(void *argv)
{	
	param_sort* param = (param_sort*)argv;
	quick_sort(arr, param->start, param->end);
	pthread_exit(0);
}

void *runner_merge(void *argv)
{	param_merge* param = (param_merge*)argv;
	merge(arr, param->sp1, param->sp2, param->ep);
	pthread_exit(0);
}

void quick_sort(int *arr, int lb, int ub)
{
	int pi;
	if (lb < ub){
		pi = partition(arr, lb, ub);
		quick_sort(arr, lb, pi-1);
		quick_sort(arr, pi+1, ub);
	}
}	

int partition(int *arr, int lb, int ub)
{
	int pivot = arr[ub];
	int i=lb;  //All elements prior to i are smaller than pivot;
	int j;
	int temp;

	for (j=lb; j<ub; j++){
		if (arr[j] < pivot){
			temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
			i += 1;	
		}
	}
	temp = arr[i];
	arr[i] = pivot;
	arr[ub] = temp;
	return i;
}

void display_arr(int* arr, int lb, int ub)
{
	int i;
	// printf("lower_index=%d, upper_index=%d\n", lb, ub);
	for (i=lb; i<=ub; i++){
		printf("%d ", arr[i]);
	}
	printf("\n");
}

void merge(int *arr, int sp1, int sp2, int ep)
{
	int arr_size = ep - sp1 + 1;
	int* temp_arr = (int*) malloc(arr_size * sizeof(int));
	int i = sp1;
	int j = sp2;
	int k = 0;

	while (i < sp2 && j <= ep){
		if (arr[i] < arr[j]){
			temp_arr[k] = arr[i];
			i += 1;
		}
		else{
			temp_arr[k] = arr[j];
			j += 1;
		}
		k += 1;
	}
	
	while (i < sp2){
		temp_arr[k] = arr[i];
		i += 1;
		k += 1;
	}
	while (j <= ep){
		temp_arr[k] = arr[j];
		j += 1;
		k += 1;
	}
	for (k=0; k<arr_size; k++){
		arr[k] = temp_arr[k];
	}
	
}




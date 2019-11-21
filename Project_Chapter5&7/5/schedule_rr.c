#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "task.h"
#include "cpu.h"
#include "schedulers.h"

struct node *listForTask = NULL;
int tid_count = 0;
void add(char *name, int priority, int burst){
	Task *t = malloc(sizeof(Task));
	t -> tid = tid_count;
	__sync_fetch_and_add(&tid_count, 1);
	t -> name = malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(t -> name, name);
	t -> priority = priority;
	t -> burst = burst;
	t -> remain = burst;
	t -> arrival_time = 0;
	t -> first_served_time = -1;
	insert(&listForTask, t);
}

Task *pickNextTask(struct node *pre){
	if (pre->next ==NULL){
		return listForTask->task;
	}
	else{
		return pre->next->task;
	}
}

void reverse(){
	struct node *tail = listForTask;
	struct node *head = listForTask->next;
	tail-> next = NULL;
	struct node *temp;
	while (head){
		temp = head;
		head = temp -> next;
		temp -> next = tail;
		tail = temp;
	}	
	listForTask = tail;
}


void display(struct node *temp){
	struct node *newnode = temp;
	while (newnode){
		Task *t = newnode ->task;
		run(t, t->burst);
		newnode = newnode -> next;
	}
}

void schedule(){
	int timer;
	int total_task;
	int total_turnaround;
	int total_waiting;
	int total_response;
	float average_turnaround;
	float average_waiting;
	float average_response;

	timer = 0;
	total_task = 0;
	total_turnaround = 0;
	total_waiting = 0;
	total_response = 0;

	reverse();
	struct node *pre = malloc(sizeof(struct node));
	pre -> next = NULL;
	while (listForTask){
		Task *t = pickNextTask(pre);
		if (t->first_served_time == -1){
			total_task += 1;
			t->first_served_time = timer;
		}

		if (pre->next == NULL){
			pre = listForTask;
		}
		else{
			pre = pre -> next;
		}

		if (t->remain <=QUANTUM){
			run(t, t->remain);
			timer += t->remain;
			t->finished_time = timer;

			total_turnaround += (t->finished_time - t->arrival_time);
			total_waiting += (t->finished_time - t->arrival_time - t->burst);
			total_response += (t->first_served_time - t->arrival_time);
			printf("Task = [%s], tid = [%d], arrival time = [%d], first served time = [%d], finished time = [%d] .\n\n",t->name, t->tid, t->arrival_time, t->first_served_time, t->finished_time);
			delete(&listForTask, t);
		}
		else{
			timer += QUANTUM;
			t->remain= t->remain- QUANTUM;
			run(t, QUANTUM);
		}

	}
	printf("All tasks finished.\n");
	average_turnaround = total_turnaround / total_task;
	average_waiting = total_waiting / total_task;
	average_response = total_response / total_task;
	printf("Average turnaround time = [%.2f].\nAverage waiting time = [%.2f].\nAverage response time = [%.2f].\n\n", average_turnaround, average_waiting, average_response);

}



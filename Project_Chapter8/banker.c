#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define MAXIMUM_LINE_LENGTH 128
#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

void init(char *args[]);
void display();
int strictly_le(int needi[], int work[]);
void display_work(int work[]);
int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);

int main(int argc, char *args[]){
	init(args);
	int i;
	char* first_token;
	char * command = malloc(sizeof(char*) * MAXIMUM_LINE_LENGTH);
	int customer_num;
	int *request = malloc(sizeof(int*) * (NUMBER_OF_RESOURCES));
	int attempt_success;
	while (1){
		fgets(command, MAXIMUM_LINE_LENGTH, stdin);
		command[strlen(command)-1] = '\0';  /* remove \n */
		first_token = strtok(command, " ");
		if (strcmp(first_token, "*") == 0){
			display();
		}
		else if (strcmp(first_token, "RQ") == 0){
			int customer_num;
			int *request = malloc(sizeof(int*) * (NUMBER_OF_RESOURCES));
			customer_num = atoi(strtok(NULL, " ")); /* customer */
			for (i=0; i<NUMBER_OF_RESOURCES; i++){
				request[i] = atoi(strtok(NULL, " "));
			}
			attempt_success = request_resources(customer_num, request);
			if (attempt_success == 0){
				for (i=0; i<NUMBER_OF_RESOURCES; i++){
					available[i] -= request[i];
					need[customer_num][i] -= request[i];
					allocation[customer_num][i] += request[i];
				}
				printf("Request permitted.\n");
			}
			else {
				printf("Request denied.\n");
			}
		}
		else if (strcmp(first_token, "RL") == 0){
			int customer_num;
			int *release = malloc(sizeof(int*) * (NUMBER_OF_RESOURCES));
			customer_num = atoi(strtok(NULL, " ")); /* customer */
			for (i=0; i<NUMBER_OF_RESOURCES; i++){
				release[i] = atoi(strtok(NULL, " "));
			}
			release_resources(customer_num, release);
		}
		else if (strcmp(first_token, "QUIT") == 0){
			printf("Quit.\n");
			break;
		}
		else{
			printf("invalid input. \n");
		}
		
	}

	display();
	return 0;
}

void init(char *args[]){
	int i, j;
	/* set available*/
	for (i=0; i<NUMBER_OF_RESOURCES; i++){
		available[i] = atoi(args[i+1]);
	}

	/* read from file and set maximum*/
	int line_num;
	int token_num;
	FILE *fptr;
	size_t len=0;
	fptr = fopen("input.txt","r");
	char * line = malloc(sizeof(char*) * MAXIMUM_LINE_LENGTH);
	char *token;

	line_num = 0;
	while(getline(&line, &len, fptr) != -1){
		line[strlen(line)-1] = '\0';  /* remove \n */
		token_num = 0;
		token = strtok(line, ",");
		while (token != NULL){
			maximum[line_num][token_num] = atoi(token);
			token = strtok(NULL, ",");
			token_num += 1;
		} 
		line_num += 1;
	}
	fclose(fptr);

	/* initialize allocation */
	for (i=0; i<NUMBER_OF_CUSTOMERS; i++){
		for (j=0; j<NUMBER_OF_RESOURCES; j++){
			allocation[i][j]= 0;
		}
	}

	/* initialize need */
	for (i=0; i<NUMBER_OF_CUSTOMERS; i++){
		for (j=0; j<NUMBER_OF_RESOURCES; j++){
			need[i][j]= maximum[i][j] - allocation[i][j];
		}
	}


}

int strictly_le(int needi[], int work[]){
	int i;
	for (i=0; i<NUMBER_OF_RESOURCES; i++){
		if (needi[i] > work[i]){
			return FALSE;
		}
	}
	return TRUE;
}

int request_resources(int customer_num, int request[]){
	int need_new[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
	int allocation_new[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
	int finish[NUMBER_OF_CUSTOMERS];
	int work[NUMBER_OF_RESOURCES];
	int i, j;

	/* make a copy*/
	for (i=0; i<NUMBER_OF_CUSTOMERS; i++){
		for (j=0; j<NUMBER_OF_RESOURCES; j++){
			need_new[i][j] = need[i][j];
			allocation_new[i][j] = allocation[i][j];
		}
	}
	

	for (i=0; i<NUMBER_OF_RESOURCES; i++){
		work[i] = available[i] - request[i];
		need_new[customer_num][i] -= request[i];
		allocation_new[customer_num][i] += request[i];
	}

	for (i=0; i<NUMBER_OF_CUSTOMERS; i++){
		finish[i] = FALSE;
	}

	/*
	printf("New Need: \n");
	for (i=0; i<NUMBER_OF_CUSTOMERS; i++){
		for (j=0; j<NUMBER_OF_RESOURCES; j++){
			printf("%d ", need_new[i][j]);
		}
		printf("\n");
	}
	
	printf("New Allocation: \n");
	for (i=0; i<NUMBER_OF_CUSTOMERS; i++){
		for (j=0; j<NUMBER_OF_RESOURCES; j++){
			printf("%d ", allocation_new[i][j]);
		}
		printf("\n");
	}

	display_work(work);
	*/
	int counter;
	int exist;
	counter = 0;
	while (counter < NUMBER_OF_CUSTOMERS){
		exist = FALSE;
		for (i=0; i<NUMBER_OF_CUSTOMERS; i++){
			if (! finish[i] && strictly_le(need_new[i], work)) {
				exist = TRUE;
				break;
			}
		}
		if (! exist) return -1;
		// printf("i=%d finished.\n", i);
		finish[i] = TRUE;
		for (j=0; j<NUMBER_OF_RESOURCES; j++){
			work[j] = work[j] + allocation_new[i][j];
		}
		// display_work(work);
		counter += 1;
	}
	return 0;
}

void release_resources(int customer_num, int release[]){
	int i;
	for (i=0; i<NUMBER_OF_RESOURCES; i++){
		allocation[customer_num][i] -= release[i];
		need[customer_num][i] += release[i];
		available[i] += release[i];
	}
}


void display_work(int work[]){
	int i;
	printf("Work: ");
	for (i=0; i<NUMBER_OF_RESOURCES; i++){
		printf(" %d", work[i]);
	}
	printf("\n");
}

void display(){
	int i, j;

	printf("Available: \n");
	for (i=0; i<NUMBER_OF_RESOURCES; i++){
		printf("%d ",available[i]);
	}
	printf("\n");

	printf("Maximum: \n");
	for (i=0; i<NUMBER_OF_CUSTOMERS; i++){
		for (j=0; j<NUMBER_OF_RESOURCES; j++){
			printf("%d ", maximum[i][j]);
		}
		printf("\n");
	}
	printf("Allocation: \n");
	for (i=0; i<NUMBER_OF_CUSTOMERS; i++){
		for (j=0; j<NUMBER_OF_RESOURCES; j++){
			printf("%d ", allocation[i][j]);
		}
		printf("\n");
	}
	printf("Need: \n");
	for (i=0; i<NUMBER_OF_CUSTOMERS; i++){
		for (j=0; j<NUMBER_OF_RESOURCES; j++){
			printf("%d ", need[i][j]);
		}
		printf("\n");
	}


}

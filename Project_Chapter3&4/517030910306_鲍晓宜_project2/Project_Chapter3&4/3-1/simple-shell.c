/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAX_LINE		512 /* 80 chars per line, per command */
#define TRUE 1 
#define FALSE 0
#define BUFFER_SIZE 2048
#define READ_END 0
#define WRITE_END 1
int decode_command(char*, char** , const char* , int);
int copy_to_storage(char**, char**, int);
int execute_recent(char**, int, int);
int fork_child(char**, int, int);
int free_storage(char**);
int display_storage(char**);
int communication(char**, int, int);
int main(void)
{
	int i;
	int should_run = 1;
	int counter=0;

	const char delimiter[2] = " ";
	const char together[2] = "&";
	int wait_for_child;

	int out_fd = dup(STDOUT_FILENO);
    	int in_fd = dup(STDIN_FILENO);
	int fd=0;
	int restore_in = FALSE;
	int restore_out = FALSE;
	int commu = FALSE;

	char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
	int num_of_args = 0;
	char one_line[100];	/* space to store current line*/

	char **storage;

	storage = malloc ( (MAX_LINE/2 + 1) * sizeof(char*));
	for (i=0; i<MAX_LINE/2 + 1; i++){
		storage[i] = malloc (20 * sizeof(char));
	}

    	while (should_run){   
        	printf("osh>");
        	fflush(stdout);
        	
		fgets(one_line, 100, stdin);
		one_line[strlen(one_line)-1] = '\0';	/* remove \n from input */

		if (strcmp(one_line, "exit") == 0)	/* check whether the user wants to terminate */
		{
			should_run = 0;
		}
		else{
			if(strcmp(one_line, "!!") == 0)	/* execute the most recent command */
			{
				execute_recent(storage, counter, num_of_args);
			}
			else{
				num_of_args = decode_command(one_line, args, delimiter, 0);
				if (strcmp(args[num_of_args-1], together) == 0){
					wait_for_child = FALSE;
					num_of_args -= 1;	/*the last one does not serve as an argument */
					args[num_of_args] = NULL;
				}
				else{
					wait_for_child = TRUE;
				}
				copy_to_storage(storage, args, num_of_args);
			}

			if (num_of_args > 1 && strcmp(storage[1], ">") == 0){
				printf("output to file %s\n", storage[2]);
				fd = open(storage[2], O_WRONLY| O_CREAT);
				dup2(fd, STDOUT_FILENO);
				restore_out = TRUE;
				for (i=1; i<num_of_args; i++){	
					strcpy(storage[i], "");	
				}
				num_of_args = 1;
			} 
			if (num_of_args > 1 && strcmp(storage[1], "<") == 0){
				printf("input from file %s\n", storage[2]);
				fd = open(storage[2], O_RDONLY);
				dup2(fd, STDIN_FILENO);
				restore_in = TRUE;
				read(fd, one_line, 100);	
				
				i = strlen(one_line) - 1;
				while (one_line[i] == '\n' || one_line[i] == EOF)	/* remove \n from input file */	
				{
					one_line[i] = '\0';
					i -= 1;
				}
				printf("The input is: %s\n", one_line);
				num_of_args = 1;

				storage[num_of_args] = strtok(one_line, delimiter);
				while(num_of_args < MAX_LINE/2 + 1 && storage[num_of_args] != NULL){
					num_of_args += 1;
					storage[num_of_args] = strtok(NULL, delimiter);				
				}
				storage[num_of_args-1][strlen(storage[num_of_args-1])] = '\0';
				storage[num_of_args] = malloc (20 * sizeof(char));	/* important!*/
				for (i=num_of_args; i<MAX_LINE / 2 + 1; i++)
				{
					strcpy(storage[i], "");
				}
				// display_storage(storage);

				// printf("Num of args: %d\n", num_of_args);
				
				if (strcmp(args[num_of_args-1], together) == 0){
					wait_for_child = FALSE;
					num_of_args -= 1;	/*the last one does not serve as an argument */
					strcpy(storage[num_of_args], "");
				}
				else{
					wait_for_child = TRUE;
				}
			}

			for (i=0; i<num_of_args; i++){
				if (strcmp(storage[i], "|") == 0){
					communication(storage, num_of_args, i);
					commu = TRUE;
					break;
				}
			}

			if (! commu){ 			
				fork_child(storage, wait_for_child, num_of_args);
			}

			if (restore_out){ 
				dup2(out_fd, STDOUT_FILENO); 
				restore_out=FALSE;
			}
			if (restore_in){
				dup2(in_fd, STDIN_FILENO);
				restore_in=FALSE;
			}
			counter += 1;
			// display_storage(storage);
			commu = FALSE;
		}
	}
	free_storage(storage);
  
	return 0;
}

int execute_recent(char ** storage, int counter, int num_of_args)
{
	int i;
	if (counter == 0){
		printf("No commands in history.\n");
		exit(1);
	}
	else{
		printf("Duplicate the last command:");
		for (i=0; i<num_of_args; i++){
			printf("%s", storage[i]);
		}
		printf("\n");
	}
	return 0;
}

int decode_command(char* one_line, char** args, const char* delimiter, int num_of_args)
{
	args[num_of_args] = strtok(one_line, delimiter);
	// printf("%d: %s ",num_of_args, args[num_of_args]);
	while(num_of_args < MAX_LINE/2 + 1 && args[num_of_args] != NULL){
		num_of_args += 1;
		args[num_of_args] = strtok(NULL, delimiter);
		// printf("%d: %s ",num_of_args, args[num_of_args]);
	}
	args[num_of_args] = NULL;
	return num_of_args;
}

int copy_to_storage(char** storage, char **args, int num_of_args)
{
	int i;
	for (i=0; i<num_of_args; i++){
		strcpy(storage[i], args[i]);	
	}
	for (i=num_of_args; i<MAX_LINE/2 + 1; i++){ strcpy(storage[i], "");}
	return 0;
}

int communication(char **storage, int num_of_args, int splitting_point)
{
	int out_fd = dup(STDOUT_FILENO);
    	int in_fd = dup(STDIN_FILENO);
	int fd[2];
	pid_t pid;
	pid_t pid_c;
	int i;
	
	char ** left;
	char ** right;
	left = malloc ( (MAX_LINE/2 + 1) * sizeof(char*));
	for (i=0; i<MAX_LINE/2 + 1; i++){
		left[i] = malloc (20 * sizeof(char));
	}
	for (i=0; i<splitting_point; i++){
		strcpy(left[i], storage[i]);
	}

	right = malloc ( (MAX_LINE/2 + 1) * sizeof(char*));
	for (i=0; i<MAX_LINE/2 + 1; i++){
		right[i] = malloc (20 * sizeof(char));
	}
	for (i=splitting_point+1; i<num_of_args; i++){
		strcpy(right[i-splitting_point-1], storage[i]);
	}

	if (pipe(fd) == -1){
		fprintf(stderr, "Pipe failed");
		return 1;
	}
	pid = fork();
	if (pid < 0){
		fprintf(stderr, "Fork Failed");
		return 1;
	}
	else if (pid == 0){  /* child */
		pid_c = fork();
		if (pid_c == 0)
		{
			close(fd[READ_END]);
			dup2(fd[WRITE_END], STDOUT_FILENO);
			left[splitting_point] = NULL;
			execvp(left[0], left);
			left[splitting_point] = malloc(20 * sizeof(char));

		}
		else{
			wait(NULL);
			// printf("grand child finished.\n");
			close(fd[WRITE_END]);
			dup2(fd[READ_END], STDIN_FILENO);
			right[num_of_args - splitting_point - 1] = NULL;
			execvp(right[0], right);
			right[num_of_args - splitting_point - 1] = malloc(20 * sizeof(char));
			dup2(out_fd, STDOUT_FILENO);
			dup2(in_fd, STDIN_FILENO);
		}
	}
	else{
		wait(NULL);
		// printf("child finished.\n");
		free_storage(left);
		free_storage(right);
		return 0;
	}	
		 
}

int fork_child(char **storage, int wait_for_child, int num_of_args)
{
	int i;
	pid_t pid;

	pid = fork();

	if (pid < 0) { /* error occurred */
		fprintf(stderr, "Fork Failed\n");
		return 1;
	}
	else if (pid == 0) { /* child process */
		/*
		printf("I am the child %d\n",pid);
		printf(" I execute the command: " );
		for (i=0; i<num_of_args; i++){
			printf("%s ", storage[i]);
		}
		display_storage(storage);
		*/
		storage[num_of_args] = NULL;
		execvp(storage[0],storage);
		storage[num_of_args] =  malloc (20 * sizeof(char));	/* restore space */
		// display_storage(storage);
	}
	else { /* parent process */
		// printf("I am the parent %d\n",pid);
		
		if (wait_for_child){
			// printf("I wait for my child to exit\n");
			wait(NULL);
			// printf("Child Complete\n");
		}
		else{
			// printf("I do not wait for my child to exit\n");
		}
	}
	return 0;
}

int free_storage(char **storage)
{
	int i;
	for (i=0; i<MAX_LINE/2 + 1; i++){
		free(storage[i]);
	}

	return 0;
}

int display_storage(char**storage)
{
	int i;
	for (i=0; i<MAX_LINE/2 + 1; i++){
		printf("@%s@ ", storage[i]);
	}	
	printf("\n");
	return 0;
}



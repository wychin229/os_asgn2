/* 
Task 3 : Deadline-based scheduling
Student Name : Chin Wen Yuan
Student ID   : 29975239

Algorithms implemented : EDF
*/
#include "pcb.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Function to calculate the turnaround time */
int cal_turnaround(int arrival, int complete)
{
	return complete-arrival;
}
/* Function to know if deadline is met or not */
int cal_deadline(int deadline, int turnaround)
{
	if(turnaround <= deadline){
		return 1;
	}
	else{
		return 0;
	}
}
/* Function to calculate the waiting time */
int cal_wait(int arrival, int getcpu)
{
	return getcpu-arrival;
}
/* Function to "pop" the first item from the queue */
int dequeue(int *front, int rear)
{
	if (*front > rear){
		//printf("rear is reached.\n");
		return -1;
	}
	else{
		*front += 1;
		return 0;
	}
}
/* Function to check if there are still unprocessed items in the queue */
int has_item(int front, int rear)
{
	if (front < rear) {
		return 0;
	}
	else{
		//printf("Empty queue\n");
		return -1;
	}
}
/* Function to append the result to the output file */
int append_file(char *p_name,int wait,int turnaround,int deadline)
{
	FILE* fp = fopen("results-3.txt", "a");
	fprintf(fp, "%s %d %d %d\n", p_name, wait, turnaround, deadline);
	fclose(fp);
}
/* Check if a process has arrived */
int check_entry(process *process_arr, int front, int time)
{
	if(process_arr[front].entryTime == time){
		return 0;
	}
	else{
		return -1;
	}
}
/* Function to check for unfinished PCB */
int has_unfinished(pcb_t *pcb_arr,int rear)
{
	int unfinished = -1;
	for(int i=0; i<rear; i++){
		if(pcb_arr[i].state == READY || pcb_arr[i].state == RUNNING){
			unfinished = 0;
			break;
		}
	}
	return unfinished;
}
/* Function to get shortest deadline PCB */
int get_shortest(pcb_t *pcb_arr, int rear)
{
	int index = -1;
	int fastest_deadline = 0;
	for(int i=0; i<rear; i++){
		if(pcb_arr[i].remainingTime >= 0) {
			if(fastest_deadline == 0) {
				fastest_deadline = pcb_arr[i].deadline;
				index = i;
			}
			else{
				if(pcb_arr[i].deadline < fastest_deadline){
					fastest_deadline = pcb_arr[i].deadline;
					index = i;
				}
			}
		}
	}
	return index;
}
/* Main functionality of the program */
int main(int argc, char *argv[])
{
	char filepath[1024] = "processes.txt";
	long fsize; // file size
	char *text; // pointer to the text read 
	text = (char *) malloc(15); // initial a random size
	
	// Select source text file
	if (argc > 2) {
		printf("Redundant argument is given.\n");
	}
	else if (argc == 2){
		strcpy(filepath,argv[1]);
		//printf("%s is used.\n",filepath);
	}
	//else{
		//printf("Default input is used.\n");
	//}
	
	// Read the file
	FILE *f = fopen(filepath, "r");
	
	if(f == NULL) {
		printf("Error in opening file.\n");
		exit(1);
	}

	fseek(f,0,SEEK_END);
	fsize = ftell(f);
	fseek(f,0,SEEK_SET);
	
	text = (char *) realloc(text, fsize+1); // reallocate size base on fsize
	fread(text,1,fsize,f);
	fclose(f);

	// Split the string by lines
	char delim[2] = "\n";
	char *temp;
	char split_lines[100][1024];
	int p_count = 0;
	
	temp = strtok(text, delim);
	
	while (temp != NULL) {
		strcpy(split_lines[p_count],temp);
		temp = strtok(NULL, delim);
		p_count += 1;
	}
	
	// Extract the values from the line-splitted string into process object
	process p_arr[100]; // most 100 process
	int p_front = 0;
	int p_rear = p_count;
	//printf("Process count : %d processes.\n\n",p_count);
	
	for(int i=0; i<p_count; i++){
		char delim2[2] = " ";
		char *temp2;
		int val_count = 0;
		
		temp2 = strtok(split_lines[i], delim2);
		
		while (temp2 != NULL) {
			if(val_count == 0) {
				strcpy(p_arr[i].process_name,temp2);
			}
			else if(val_count == 1) {
				p_arr[i].entryTime = atoi(temp2);
			}
			else if(val_count == 2) {
				p_arr[i].serviceTime = atoi(temp2);
			}
			else{
				p_arr[i].deadline = atoi(temp2);
			}
			temp2 = strtok(NULL, delim2);
			val_count += 1;
		}
		/*
		printf("Index       : %d\n",i);
		printf("Process name: %s\n",p_arr[i].process_name);
		printf("Entry time  : %d\n",p_arr[i].entryTime);
		printf("Service time: %d\n",p_arr[i].serviceTime);
		printf("Deadline    : %d\n",p_arr[i].deadline);
		*/
	}
	
	// PCBs
	pcb_t pcb_arr[100]; // array with at most 100 pcbs
	pcb_t pcb_buf;
	int pcb_rear = 0; // use to indicate the end of arr 
	int run_int = -1; // index of the process with shortest remaining time
	
	// simulate the scheduling - EDF
	int clock = 0;
	
	while((has_item(p_front,p_rear)) == 0 || (has_unfinished(pcb_arr,pcb_rear)) != -1){
		// check if any process arrive at this second
		if((has_item(p_front,p_rear)) == 0){
			while((check_entry(p_arr,p_front,clock)) >= 0){
				// store into PCB object and put into pcb queue
				strcpy(pcb_buf.process_name,p_arr[p_front].process_name);
				pcb_buf.entryTime = p_arr[p_front].entryTime;
				pcb_buf.serviceTime = p_arr[p_front].serviceTime;
				pcb_buf.remainingTime = p_arr[p_front].serviceTime;
				pcb_buf.deadline = p_arr[p_front].deadline;
				pcb_buf.getcpu = -1; // indicate have not get cpu
					
				pcb_buf.state = READY;
				printf("Time %d: %s has entered the system.\n",clock,pcb_buf.process_name);
				pcb_arr[pcb_rear] = pcb_buf;
				pcb_rear += 1;
				// reset the buffer pcb
				memset(&pcb_buf, 0, sizeof(pcb_t));
				
				dequeue(&p_front,p_rear);
			}
		}
		
		// process PCB
		if(run_int == -1){
			run_int = get_shortest(pcb_arr,pcb_rear);
			// if the new shortest has run before
			if(pcb_arr[run_int].remainingTime < pcb_arr[run_int].serviceTime){
				printf("Time %d: %s is in the running state.\n",clock,pcb_arr[run_int].process_name);
				pcb_arr[run_int].state = RUNNING;
			}
		}
		
		// if the shortest deadline process is different
		int shortest = get_shortest(pcb_arr,pcb_rear);
		if(shortest != run_int && pcb_arr[run_int].remainingTime > 0){
			pcb_arr[run_int].state = READY;
			printf("Time %d: %s has returned to ready state.\n",clock,pcb_arr[run_int].process_name);
			run_int = shortest;
			// if the new shortest has run before
			if(pcb_arr[run_int].remainingTime < pcb_arr[run_int].serviceTime){
				printf("Time %d: %s is in the running state.\n",clock,pcb_arr[run_int].process_name);
				pcb_arr[run_int].state = RUNNING;
			}
		}
		
		// if process is run for the first time
		if(pcb_arr[run_int].remainingTime == pcb_arr[run_int].serviceTime) {
			pcb_arr[run_int].getcpu = clock; // the first time process gets cpu
			printf("Time %d: %s is in the running state.\n",clock,pcb_arr[run_int].process_name);
			pcb_arr[run_int].state = RUNNING;
			pcb_arr[run_int].remainingTime -= 1;
			clock++;
		}
		// if process still have remaining time
		else if(pcb_arr[run_int].remainingTime > 0) {
			pcb_arr[run_int].remainingTime -= 1;
			clock++;
		}
		// if process has finished executing
		else if(pcb_arr[run_int].remainingTime == 0) {
			printf("Time %d: %s has finished execution.\n",clock,pcb_arr[run_int].process_name);
			pcb_arr[run_int].state = TERMINATED;
			pcb_arr[run_int].remainingTime -= 1; // make the remainingTime = -1
			int wait = cal_wait(pcb_arr[run_int].entryTime,pcb_arr[run_int].getcpu);
			int turnaround = cal_turnaround(pcb_arr[run_int].entryTime,clock);
			int deadline = cal_deadline(pcb_arr[run_int].deadline, turnaround);

			append_file(pcb_arr[run_int].process_name,wait,turnaround,deadline);
			run_int = -1;
		}
		
		if(clock == 50){
			break;
		}
	}
	
	// free the allocated space;
	free(text);
	exit(0);
}

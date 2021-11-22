/* 
Task 1 : Non-preemptive scheduling
Student Name : Chin Wen Yuan
Student ID   : 29975239

Algorithms implemented : FCFS
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
	FILE* fp = fopen("results-1.txt", "a");
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
	pcb_t pcb_arr[100]; // queue with at most 100 pcbs
	pcb_t pcb_buf;
	int pcb_front = 0;
	int pcb_rear = 0;
	
	// simulate the scheduling - FCFS
	int clock = 0;
	
	while((has_item(p_front,p_rear)) == 0 || (has_item(pcb_front,pcb_rear)) == 0){
		// check if any process arrive at this second
		if((has_item(p_front,p_rear)) == 0){
			while((check_entry(p_arr,p_front,clock)) >= 0){
				// store into PCB object and put into pcb queue
				strcpy(pcb_buf.process_name,p_arr[p_front].process_name);
				pcb_buf.entryTime = p_arr[p_front].entryTime;
				pcb_buf.serviceTime = p_arr[p_front].serviceTime;
				pcb_buf.remainingTime = p_arr[p_front].serviceTime;
				pcb_buf.deadline = p_arr[p_front].deadline;
					
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
		if((has_item(pcb_front,pcb_rear)) == 0) {
			// Run the process in the PCB for the first time
			if(pcb_arr[pcb_front].remainingTime == pcb_arr[pcb_front].serviceTime) {
				pcb_arr[pcb_front].getcpu = clock; // the first time process gets cpu
				printf("Time %d: %s is in the running state.\n",clock,pcb_arr[pcb_front].process_name);
				pcb_arr[pcb_front].state = RUNNING;
				pcb_arr[pcb_front].remainingTime -= 1;
			}
			// The process has finished execution
			else if(pcb_arr[pcb_front].remainingTime == 0) {
				printf("Time %d: %s has finished execution.\n",clock,pcb_arr[pcb_front].process_name);
				pcb_arr[pcb_front].state = TERMINATED;

				int wait = cal_wait(pcb_arr[pcb_front].entryTime,pcb_arr[pcb_front].getcpu);
				int turnaround = cal_turnaround(pcb_arr[pcb_front].entryTime,clock);
				int deadline = cal_deadline(pcb_arr[pcb_front].deadline, turnaround);

				append_file(pcb_arr[pcb_front].process_name,wait,turnaround,deadline);
				
				// Retrieve the next PCB(if any) that already arrived and run it.
				dequeue(&pcb_front,pcb_rear);
				if((has_item(pcb_front,p_rear)) == 0) {
					pcb_arr[pcb_front].getcpu = clock; // the first time process gets cpu
					printf("Time %d: %s is in the running state.\n",clock,pcb_arr[pcb_front].process_name);
					pcb_arr[pcb_front].remainingTime -= 1;
				}
				// do ntg if there is no next PCB 
			}
			// The current running process still has remaining time
			else if(pcb_arr[pcb_front].remainingTime < pcb_arr[pcb_front].serviceTime && pcb_arr[pcb_front].remainingTime > 0) {
				pcb_arr[pcb_front].remainingTime -= 1;
			}
		}
		clock++;
	}
	
	// free the allocated space;
	free(text);
	exit(0);
}

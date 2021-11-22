# os_asgn2  
Assignment 2 for FIT2100  
The purpose of these programs is to simulate various process scheduling algorithms  
1) Task 1 -> FCFS (First Come First Serve)
2) Task 2 -> SRTN (Shortest Remaining Time Next)  
3) Task 3 -> EDF (Earliest Deadline First)  

## Input file (default processes.txt)
Each line in the input file is of the following format (space-separated values) which corresponds to a single process  
[Process Name] [Arrival Time] [Service Time] [Deadline]  
*Note : Process can have same arrival time  

During simulation (running program), standard output message will be printed in the terminal. (process arriving, entering/exit running state, finished execution)  

## Output file (result-TASKNUM.txt)
Each line in the input file is of the following format (space-separated values) which corresponds to a single process  
[Process Name] [Wait time] [Turnaround time] [Deadline met]  

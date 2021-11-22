/* Special enumerated data type for process state */
typedef enum {
	READY, RUNNING, EXIT, TERMINATED
} process_state_t;

/* C data structure used as process control block. The scheduler
 * should create one instance per running process in the system.
 */
typedef struct {
	char process_name[11]; // a process name is at most 10 characters long
	
	/* Times are measured in seconds. */
	int entryTime; // The time process entered system
	int serviceTime; // The total CPU time required by the process
	int remainingTime; // Remaining service time until completion
	int deadline; // Expected maximum arrival time to completion time
	int getcpu; // The time the process gets the cpu
	process_state_t state; // current process state
} pcb_t;

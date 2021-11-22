/* C data structure used as process. Used to store process information (before
	arrival time.
 */
typedef struct {
	char process_name[11]; // a process name is at most 10 characters long
	
	/* Times are measured in seconds. */
	int entryTime; // The time process entered system
	int serviceTime; // The total CPU time required by the process
	int deadline; // Expected maximum arrival time to completion time

} process;

#include "prioque.h"
#include <stdbool.h>
#include <stdio.h>

//used to represent infinity in g and b values
#define INFINITY 255

//structure of a process and process behavior
typedef struct _ProcessBehavior{
	unsigned long cpuburst;
	unsigned long current_cpuburst;

	unsigned long ioburst;
	unsigned long current_ioburst;

	unsigned long repeat;
	unsigned long current_repeat;
}ProcessBehavior;

typedef struct _Process{
	unsigned long pid;
	unsigned long arrival_time;	

	Queue behaviors;
}Process;

//structure for holding processes
//there will be 3 of these high, mid, and low
typedef struct _ProcessQueue{
	unsigned char q;
	unsigned char b;
	unsigned char g;

	//this is the processses in the queue
	Queue processes;
}ProcessQueue;

//function declarations
void init_all_queues(void);
void init_process(Process* process);
void read_process_descriptions(void);
void execute_highest_priority_process(void);
bool processes_exist(void);
void queue_new_arrivals(void);
void do_IO(void);
void final_report(void);

//global variables
unsigned long Clock;
Queue ArrivalQ;


//this queue is always present
Process IdleProcess;
Process* CurrentBlockedProcess;
unsigned long IdleProcessCycles;

//says wether we are blocking for io
bool blocked = false;

// comparison function for queue of ProcessBehaviors
int process_behavior_comparison(const void* e1, const void* e2) {
	ProcessBehavior *processb1 = (ProcessBehavior*)e1;
	ProcessBehavior *processb2 = (ProcessBehavior*)e2;

	if(processb1->cpuburst > processb2->cpuburst)
		return 1;
	else 
		return 0;
}

// comparison function for queue of processes
int process_queue_comparison(const void* e1, const void* e2) {
	Process *process1 = (Process*)e1;
	Process *process2 = (Process*)e2;

	return process_behavior_comparison(process1->behaviors.queue->info, process2->behaviors.queue->info);
}

//defining the three queues to hold running processes
ProcessQueue HighProcessQ = {
	.q = 10,
	.b = 1,
	.g = INFINITY
};

ProcessQueue MidProcessQ = {
	.q = 30,
	.b = 2,
	.g = 2
};

ProcessQueue LowProcessQ = {
	.q = 100,
	.b = INFINITY,
	.g = 1
};

int main(int argc, char* argv[]){
	init_all_queues();
	init_process(&IdleProcess);
	read_process_descriptions();

	while(processes_exist()){
		Clock++;
		queue_new_arrivals();
		execute_highest_priority_process();
		do_IO();
	}

	Clock++;
	final_report();

	return 1;
}	

//read all processes descriptions from standard input and populate
//the 'ArrivalQueue'.
void read_process_descriptions(void){
	Process p;
	ProcessBehavior b;
	int pid = 0, first = 1;
	unsigned long arrival;

	init_process(&p);
	arrival = 0;
	while(scanf("%lu", &arrival) != EOF){
		scanf("%d %lu %lu %d",
		       &pid,
		       &b.cpuburst,
		       &b.ioburst,
		       &b.repeat);
		
		if(!first && p.pid != pid){
			add_to_queue(&ArrivalQ, &p, p.arrival_time);
			init_process(&p);
		}

		p.pid = pid;
		p.arrival_time = arrival;
		first = 0;

		add_to_queue(&p.behaviors, &b, 1);
	}
	add_to_queue(&ArrivalQ, &p, p.arrival_time);
}

//FIXME::implement this
void init_process(Process* process){
	printf("init_process\n");

	//this queue holds the new processes to be added to queues
	init_queue(&process->behaviors, sizeof(ProcessBehavior), TRUE, process_behavior_comparison, FALSE);
}

//FIXME::implement this
void init_all_queues(void){
	//arrival queue
	printf("init_all_queues\n");

	init_queue(&ArrivalQ, sizeof(Process), TRUE, NULL, FALSE);
	
	//initializing the process queues
	init_queue(&HighProcessQ.processes, sizeof(Process), TRUE, process_queue_comparison, FALSE);
	init_queue(&MidProcessQ.processes,  sizeof(Process), TRUE, process_queue_comparison, FALSE);
	init_queue(&LowProcessQ.processes, sizeof(Process), TRUE, process_queue_comparison, FALSE);
}

//FIXME::implement this
void do_IO(void){
	if(blocked){
		//increasing io spent in current process behavior
		ProcessBehavior* process_behavior = (ProcessBehavior*)CurrentBlockedProcess->behaviors.current;

		//unblocking if we have done enough io
		if(++(process_behavior->current_ioburst) == process_behavior->ioburst)
			blocked = false;
	}
}

//FIXME::implement this
void final_report(void){
	printf("final_report\n");
}

//FIXME::implement this
void execute_highest_priority_process(void){
	//check if we are not blocked for io
	if(!blocked){
		ProcessQueue* CurrentProcessQ = NULL;

		//checking if queues are empty or not
		if(!empty_queue(&HighProcessQ.processes))
			CurrentProcessQ = &HighProcessQ;
		else if(!empty_queue(&MidProcessQ.processes))
			CurrentProcessQ = &MidProcessQ;
		else if(!empty_queue(&LowProcessQ.processes))
			CurrentProcessQ = &LowProcessQ;
		else{
			//need to run the null process
			IdleProcessCycles++;
			return;
		}

		Process* process = (Process*)CurrentProcessQ->processes.current->info; 
		ProcessBehavior* process_behavior = (ProcessBehavior*)process->behaviors.current;

		//checking if we have ran enought cpu cycles
		if(++(process_behavior->current_cpuburst) == process_behavior->cpuburst){
			//check if this is the last cpu time we need so we repeated one more time than we need to end on cpu time
			if(++(process_behavior->current_repeat) > process_behavior->repeat){
				rewind_queue(&process->behaviors);
				delete_current(&process->behaviors);

				//if we have no process behaviors then we delete process from process queue
				if(empty_queue(&process->behaviors)){
					rewind_queue(&CurrentProcessQ->processes);
					delete_current(&CurrentProcessQ->processes);
				}
			}else{
				blocked = true;
				CurrentBlockedProcess = process;
			}
		}
	}
}

//FIXME::implement this
bool processes_exist(void){
	//checking if all queue's are empty
	if(empty_queue(&HighProcessQ.processes) && empty_queue(&HighProcessQ.processes) && empty_queue(&HighProcessQ.processes) && empty_queue(&ArrivalQ))
		return false;

	//one of the queues is not empty so processes are either waiting or process queues have processes
	return true;
}

//FIXME::might need to check if blocked before queueuing
void queue_new_arrivals(void){
	rewind_queue(&ArrivalQ);

	//first checking if the arrival queue has any processes
	while(!empty_queue(&ArrivalQ) && Clock == current_priority(&ArrivalQ)){
		//getting process for logging
		Process* process = (Process*)ArrivalQ.current->info; 
		printf("CREATE: Process %lu entered the ready queue at time %lu.\n", process->pid, Clock);

		//adding process to high priority queue
		add_to_queue(&HighProcessQ, process, NULL);

		//deleting process from ArrivalQ
		delete_current(&ArrivalQ);
	}
}
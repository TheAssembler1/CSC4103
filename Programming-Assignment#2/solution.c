#include "prioque.h"
#include <stdbool.h>
#include <stdio.h>

//used to represent infinity in g and b values
#define INFINITY 255

//structure of a process and process behavior
typedef struct _ProcessBehavior{
	unsigned long cpuburst;
	unsigned long ioburst;
	unsigned long repeat;
}ProcessBehavior;

typedef struct _Process{
	unsigned long pid;
	unsigned long arrival;
	unsigned long arrival_time;	

	//this should be a queue
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

//this holds the 3 queues of high, mid, and low processes
Queue ProcessQs;

// comparison function for queue of ProcessBehaviors
int process_comparison(const void* e1, const void* e2) {
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

	return process_comparison(process1->behaviors.queue->info, process2->behaviors.queue->info);
}

//this queue is always present
Process IdleProcess = {
	.arrival = 0,
	.arrival_time = 0,
	.pid = 0
};

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

	//FIXME::remove this
	rewind_queue(&ArrivalQ);
	printf("Queue contains:\n");
  	while (!end_of_queue(&ArrivalQ)) {
    	printf("%d\n", current_priority(&ArrivalQ));

		printf("Process behaviros burst time\n");
		while(!end_of_queue(&ArrivalQ.queue->info.behaviors)){
			printf("%d\n", &ArrivalQ.queue->info.behaviors->queue.cpuburst);
			next_element(&ArrivalQ.queue->info.behaviors);
		}

    	next_element(&ArrivalQ);
  	}
	printf("Length of Q: %d\n", queue_length(&ArrivalQ));
}

//FIXME::implement this
void init_process(Process* process){
	printf("init_process\n");

	//this queue holds the new processes to be added to queues
	init_queue(&process->behaviors, sizeof(ProcessBehavior), TRUE, process_comparison, FALSE);
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
	printf("do_IO\n");
}

//FIXME::implement this
void final_report(void){
	printf("final_report\n");
}

//FIXME::implement this
void execute_highest_priority_process(void){
	printf("execute_highest_priority_process\n");
}

//FIXME::implement this
bool processes_exist(void){
	printf("processes_exist\n");
	return false;
}

//FIXME::implement this
void queue_new_arrivals(void){
	printf("queue_new_arrivals\n");
}
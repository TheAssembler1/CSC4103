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

	int repeat;
	int current_repeat;
}ProcessBehavior;

typedef struct _Process{
	int pid;
	unsigned long arrival_time;	
	unsigned long total_clock_time;

	unsigned char current_b;
	unsigned char current_g;

	Queue behaviors;
}Process;

typedef struct _ProcessQueue{
	unsigned char q;
	unsigned char current_q;

	unsigned char b;
	unsigned char g;
	unsigned char level;

	//ProcessQ above it in prio
	Queue* HigherQ;

	//ProcessQ below it in prio
	Queue* LowerQ;

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

Process IdleProcess;

Queue ArrivalQ;
Queue RemovalQ;

ProcessQueue HighQ = {.q = 10,  .b =1, 		   .g = INFINITY, .level = 1};
ProcessQueue MedQ = { .q = 30,  .b = 2, 	   .g = 2, 		  .level = 2};
ProcessQueue LowQ = { .q = 100, .b = INFINITY, .g = 1, 		  .level = 3};

Process* CurrentBlockingProcess;

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

void init_process(Process* process){
	process->current_b = 0;
	process->current_g = 0;
	init_queue(&process->behaviors, sizeof(ProcessBehavior), TRUE, NULL, TRUE);
}

void init_all_queues(void){
	init_queue(&HighQ.processes, sizeof(Process), TRUE, NULL, TRUE);
	init_queue(&MedQ.processes, sizeof(Process), TRUE, NULL, TRUE);
	init_queue(&LowQ.processes, sizeof(Process), TRUE, NULL, TRUE);
	init_queue(&ArrivalQ, sizeof(Process), TRUE, NULL, FALSE);
	init_queue(&RemovalQ, sizeof(Process), TRUE, NULL, FALSE);

	//setting pointers to high and lower process qeueus
	HighQ.HigherQ = (Queue*)NULL;    HighQ.LowerQ = &MedQ.processes;
	MedQ.HigherQ = &HighQ.processes; MedQ.LowerQ = &LowQ.processes;
	LowQ.HigherQ = &MedQ.processes;  LowQ.LowerQ = (Queue*)NULL;
}

void do_IO(void){
	if(CurrentBlockingProcess){
		printf("IO\n");
		rewind_queue(&CurrentBlockingProcess->behaviors);
		ProcessBehavior* process_behavior = (ProcessBehavior*)CurrentBlockingProcess->behaviors.current->info;

		//process is done running io
		if(++(process_behavior->current_ioburst) == process_behavior->ioburst){
			CurrentBlockingProcess = NULL;

			//reseting process behavior
			process_behavior->current_cpuburst = 0;
			process_behavior->current_ioburst = 0;
			process_behavior->current_repeat++;
		}
	}
}

void final_report(void){
	printf("Scheduler shutdown at time %lu.\n", Clock);
	printf("Total CPU usage for all processes scheduled:\n\n");
	printf("Process <<null>>: %lu time units.\n", IdleProcess.total_clock_time);

	rewind_queue(&RemovalQ);
	while(!end_of_queue(&RemovalQ)){
		Process* process = (Process*)RemovalQ.current->info;
		printf("Process %d: %lu time units.\n", process->pid, process->total_clock_time);
		next_element(&RemovalQ);
	}
}

void execute_highest_priority_process(void){
	//if we are not blocked for io
	if(!CurrentBlockingProcess){
		//represents the current q we are working in
		ProcessQueue* CurrentQ = NULL;

		//setting the current q
		if(!empty_queue(&HighQ.processes))
			CurrentQ = &HighQ;
		else if(!empty_queue(&MedQ.processes))
			CurrentQ = &MedQ;
		else if(!empty_queue(&LowQ.processes))
			CurrentQ = &LowQ;
		else//no processes to run 
			IdleProcess.total_clock_time++;

		if(CurrentQ){
			//getting the process
			rewind_queue(&CurrentQ->processes);
			Process* process = (Process*)CurrentQ->processes.current->info;

			//getting its behaviors
			rewind_queue(&process->behaviors);
			ProcessBehavior* process_behavior = (ProcessBehavior*)process->behaviors.current->info;

			process_behavior->current_cpuburst++;

			printf("Process has an id of %d\n", process->pid);
			printf("CPU\n");

			/*
			printf("RUN AT TIME %lu.\n", Clock);
			printf("_______________________________________________\n");
			printf("Process has an id of %d\n", process->pid);
			printf("------\n");
			printf("Process current cpu burst %lu\n", process_behavior->current_cpuburst);
			printf("Process wanted cpu burst %lu\n", process_behavior->cpuburst);
			printf("------\n");
			printf("Process current io burst %lu\n", process_behavior->current_ioburst);
			printf("Process wanted io burst %lu\n", process_behavior->ioburst);
			printf("------\n");
			printf("Process current repeat %d\n", process_behavior->current_repeat);
			printf("Process wanted repeat %d\n", process_behavior->repeat);
			*/

			//process has run through all its need cpu cycles
			if(process_behavior->current_cpuburst == process_behavior->cpuburst){
				//process has done all the io and cpu it wants to
				if(process_behavior->current_repeat == process_behavior->repeat){
					rewind_queue(&process->behaviors);
					delete_current(&process->behaviors);

					//remove process from process queeu if it has no behaviors left to run
					if(empty_queue(&process->behaviors))
						delete_current(&CurrentQ->processes);
				}else{ //process needs to block for io
					Clock++;
					queue_new_arrivals();
					CurrentBlockingProcess = process;
				}
			}
		}
	}
}

bool processes_exist(void){
	if(empty_queue(&HighQ.processes) && empty_queue(&MedQ.processes) && empty_queue(&LowQ.processes) && empty_queue(&ArrivalQ))
		return false;
	return true;
}

void queue_new_arrivals(void){
	rewind_queue(&ArrivalQ);

	//first checking if the arrival queue has any processes
	while(!empty_queue(&ArrivalQ) && Clock == current_priority(&ArrivalQ)){
		//getting process for logging
		Process* process = (Process*)ArrivalQ.current->info; 

		//setting initial state of process behaivors
		rewind_queue(&process->behaviors);
		while(!end_of_queue(&process->behaviors)){
			ProcessBehavior* process_behavior = (ProcessBehavior*)process->behaviors.current->info;

			process_behavior->current_cpuburst = 0;
			process_behavior->current_ioburst = 0;
			process_behavior->current_repeat = 0;

			next_element(&process->behaviors);
		}

		//logging process info
		printf("CREATE: Process %d entered the ready queue at time %lu.\n", process->pid, Clock);

		//adding process to high priority queue
		add_to_queue(&HighQ.processes, process, 0);

		//deleting process from ArrivalQ
		delete_current(&ArrivalQ);
	}
}
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
}ProcessBehavior;

typedef struct _ProcessQueue ProcessQueue;

typedef struct _Process{
	int pid;
	unsigned long arrival_time;	
	unsigned long total_clock_time;

	unsigned char current_b;
	unsigned char current_g;

	//this tells io which queue to go back to when done
	ProcessQueue* CurrentQ;

	Queue behaviors;
}Process;

struct _ProcessQueue{
	//represents how much quantum the current process has used in the queue
	unsigned char current_q;
	unsigned char q;

	unsigned char b;
	unsigned char g;
	unsigned char level;

	//ProcessQ above it in priority
	ProcessQueue* HigherQ;

	//ProcessQ below it in priority
	ProcessQueue* LowerQ;

	//this is the processses in the queue
	Queue processes;
};

//function declarations
void init_all_queues(void);
void init_process(Process* process);
void read_process_descriptions(void);
void execute_highest_priority_process(void);
bool processes_exist(void);
void queue_new_arrivals(void);
void do_IO(void);
void final_report(void);
void check_b_and_c(ProcessQueue* CurrentQ, Process* process, ProcessBehavior* process_behavior);

//global variables
unsigned long Clock;

Process IdleProcess;
Process* LastProcess;

Queue ArrivalQ;
Queue RemovalQ;
Queue IOQ;

ProcessQueue HighQ = {.q = 10,  .b =1, 		   .g = INFINITY, .level = 1};
ProcessQueue MedQ =  {.q = 30,  .b = 2, 	   .g = 2, 		  .level = 2};
ProcessQueue LowQ =  {.q = 100, .b = INFINITY, .g = 1, 		  .level = 3};

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
	//setting process starting queue to the high queue
	process->CurrentQ = &HighQ;

	//setting b and g and total runtime values of process
	process->current_b = process->current_g = process->total_clock_time = 0;

	init_queue(&process->behaviors, sizeof(ProcessBehavior), TRUE, NULL, TRUE);
}

void init_all_queues(void){
	init_queue(&HighQ.processes, sizeof(Process), TRUE, NULL, TRUE);
	init_queue(&MedQ.processes,  sizeof(Process), TRUE, NULL, TRUE);
	init_queue(&LowQ.processes,  sizeof(Process), TRUE, NULL, TRUE);

	init_queue(&IOQ, 	  sizeof(Process), TRUE, NULL, FALSE);
	init_queue(&ArrivalQ, sizeof(Process), TRUE, NULL, FALSE);
	init_queue(&RemovalQ, sizeof(Process), TRUE, NULL, FALSE);

	//setting pointers to high and lower process qeueus
	HighQ.HigherQ = (ProcessQueue*)NULL;    HighQ.LowerQ = &MedQ;
	MedQ.HigherQ  = &HighQ; 				MedQ.LowerQ  = &LowQ;
	LowQ.HigherQ  = &MedQ;  				LowQ.LowerQ  = (ProcessQueue*)NULL;
}

void do_IO(void){
	//checking if there is any process to to io for
	if(!empty_queue(&IOQ)){
		//walking io queue and doing io for all processes in it
		rewind_queue(&IOQ);
		while(!end_of_queue(&IOQ)){
			//getting the process and process behavior
			Process* process = IOQ.current->info;
			rewind_queue(&process->behaviors);
			ProcessBehavior* process_behavior = process->behaviors.current->info;

			//decreasing io of process and checking if it is done with io
			if((process_behavior->current_ioburst)++ == process_behavior->ioburst){
				//resetting b value of process
				process_behavior->current_cpuburst = process_behavior->current_ioburst =process->current_b = 0;

				//check if current process behavior is done and it has no more repeats
				if(!(--(process_behavior->repeat)) && queue_length(&process->behaviors) > 1)
					delete_current(&process->behaviors);

				//adding process back to its original q
				add_to_queue(&process->CurrentQ->processes, process, 0);

				//removing element from IOQ
				delete_current(&IOQ);
				break;
			}
			next_element(&IOQ);
		}
	}
}

void final_report(void){
	printf("Scheduler shutdown at time %lu.\n", Clock);
	printf("Total CPU usage for all processes scheduled:\n");
	printf("Process <<null>>:	%lu time units.\n", IdleProcess.total_clock_time);

	rewind_queue(&RemovalQ);
	while(!end_of_queue(&RemovalQ)){
		Process* process = (Process*)RemovalQ.current->info;
		printf("Process %d:		%lu time units.\n", process->pid, process->total_clock_time);

		next_element(&RemovalQ);
	}
}

void execute_highest_priority_process(void){
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

	//if there is a q with a process in it
	if(CurrentQ){
		rewind_queue(&CurrentQ->processes);
		Process* process = (Process*)CurrentQ->processes.current->info;
		rewind_queue(&process->behaviors);
		ProcessBehavior* process_behavior = process->behaviors.current->info;

		if(LastProcess != process){
			printf("RUN: Process %d started execution from level %u at time %lu; wants to execute for %lu ticks.\n",
						process->pid, CurrentQ->level, Clock, process_behavior->cpuburst - process_behavior->current_cpuburst);
			
			LastProcess = process;
		}

		process->total_clock_time++;

		//increase quantom of the current process in the queue and check if it needs to be demoted
		if(++(CurrentQ->current_q) == CurrentQ->q && ++(process->current_b) == CurrentQ->b && CurrentQ->LowerQ){
			printf("QUEUED: Process %d queued at level %u at time %lu.\n", process->pid, CurrentQ->level + 1, Clock + 1);

			process->current_b = process->current_g = CurrentQ->current_q = 0;

			process->CurrentQ = CurrentQ->LowerQ;

			add_to_queue(&CurrentQ->LowerQ->processes, process, 0);
			delete_current(&CurrentQ->processes);

			CurrentQ = CurrentQ->LowerQ;
			CurrentQ->processes.current = CurrentQ->processes.tail;
			process = CurrentQ->processes.current->info;
		}

		if(++(process_behavior->current_cpuburst) == process_behavior->cpuburst){\
			if(CurrentQ->current_q < CurrentQ->q && ++(process->current_g) == CurrentQ->g && process_behavior->repeat && CurrentQ->HigherQ){ //process needs to be promoted
				process->current_b = process->current_g = 0;

				process->CurrentQ = CurrentQ->HigherQ;

				add_to_queue(&CurrentQ->HigherQ->processes, process, 0);
				delete_current(&CurrentQ->processes);

				CurrentQ = CurrentQ->HigherQ;
				CurrentQ->processes.current = CurrentQ->processes.tail;
				process = CurrentQ->processes.current->info;
			}

			//check if process is done running
			if(!(process_behavior->repeat)){ //process is done running remove from queue
				printf("FINISHED:  Process %d finished at time %lu.\n", process->pid, Clock + 1);
				add_to_queue(&RemovalQ, process, 0);
				delete_current(&CurrentQ->processes);
			}else{ //run io on it
				printf("I/O: Process %d blocked for I/O at time %lu.\n", process->pid, Clock + 1);
				process->total_clock_time++;
				add_to_queue(&IOQ, process, 0);
				delete_current(&CurrentQ->processes);
				LastProcess = NULL;
			}

			//need to reset quantum in any case
			CurrentQ->current_q = 0;
		}
	}
}

bool processes_exist(void){
	//if all queues are empty nothing left to do
	if(empty_queue(&IOQ) && empty_queue(&HighQ.processes) && empty_queue(&MedQ.processes) && empty_queue(&LowQ.processes) && empty_queue(&ArrivalQ))
		return false;
	return true;
}

void queue_new_arrivals(void){
	//first checking if the arrival queue has any processes
	rewind_queue(&ArrivalQ);
	while(!empty_queue(&ArrivalQ) && Clock == current_priority(&ArrivalQ)){
		//getting process for logging
		Process* process = (Process*)ArrivalQ.current->info; 
		rewind_queue(&process->behaviors);

		while(!end_of_queue(&process->behaviors)){
			ProcessBehavior* process_behavior = process->behaviors.current->info;
			//initializing the current cpu and io burst
			process_behavior->current_cpuburst = process_behavior->current_ioburst = 0;
			next_element(&process->behaviors);
		}

		printf("CREATE: Process %d entered the ready queue at time %lu.\n", process->pid, Clock);

		//adding process to high priority queue
		add_to_queue(&HighQ.processes, process, 0);

		//deleting process from ArrivalQ
		delete_current(&ArrivalQ);
	}
}
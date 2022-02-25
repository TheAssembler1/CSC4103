#include "prioque.h"
#include <stdbool.h>
#include <stdio.h>

//structure of a process and process behavior
typedef struct _Process_Behavior{
	unsigned long cpuburst;
	unsigned long ioburst;
	unsigned long repeat;
}Process_Behavior;

typedef struct _Process{
	unsigned long pid;
	unsigned long arrival;
	unsigned long arrival_time;	

	//this shoudl be a queue
	Queue behaviors;
}Process;

//function declarations
void init_all_queues(void);
void init_process(Process* process);
void read_process_descriptions(void);
bool processes_exist(void);
void queue_new_arrivals(void);
void do_IO(void);
void final_report(void);

//global variables
unsigned long Clock;
Process IdleProcess;

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

	return 0;
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
	while(scanf("%lu", &arrival) != EOF)
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

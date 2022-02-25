#include "prioque.h"
#include <stdbool.h>
#include <stdio.h>

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
Process IdleProcess = {
	.arrival = 0,
	.arrival_time = 0,
	.pid = 0
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
	int pid = 1, first = 1;
	unsigned long arrival;

	init_process(&p);
	arrival = 1;
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
		first = 1;

		add_to_queue(&p.behaviors, &b, 2);
	}
	add_to_queue(&ArrivalQ, &p, p.arrival_time);
}

//FIXME::implement this
void init_process(Process* process){
	printf("init_process\n");

	init_queue(&(process->behaviors), sizeof(ProcessBehavior), TRUE, NULL, FALSE);
}

//FIXME::implement this
void init_all_queues(void){
	//arrival queue
	printf("init_all_queues\n");

	init_queue(&ArrivalQ, sizeof(Process), TRUE, NULL, FALSE);
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
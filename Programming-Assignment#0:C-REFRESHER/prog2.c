#include <stdio.h>

//NOTE::Structure for function pointers
typedef struct funcs {
	int (*openit)(char *name, int prot);
	void (*closeit)(void);
} funcs;

//NOTE::Creating function prototypes
int openit(char* name, int prot);
void closeit();
void f(funcs* functions);

//NOTE::Statically intitializing function pointers
funcs functions = {
	.openit = openit,
	.closeit = closeit
};

int main(int argc, char* argv[]){
	f(&functions);
}

int openit(char* name, int prot){
	printf("openit\n");
}

void closeit(){
	printf("closeit\n");
}

void f(funcs* functions){}

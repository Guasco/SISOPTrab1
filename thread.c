#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#define STACKSIZE 32768

static ucontext_t uctx_main, uctx_func1, uctx_func2;


ucontext_t  ContextP, ContextQ, ContextMain;;
enum{
Running,
Blocked,
Ready,
Finished

};

typedef struct Process {
  int var1;
  float var2;
  ucontext_t contexto;
  ucontext_t caller;
  int sleeping;
  int state;
  char stack[STACKSIZE];

  struct Process *next;
  struct Process *prev;

} Process;


void Process_debug(Process *myProc){
	printf("me %p \n",myProc);
	printf("next %p\n",myProc->next);
	printf("prev %p\n",myProc->prev);
	int suspended=myProc->sleeping;
	int finished=!suspended;
	
	if(suspended){
		printf("Suspended ");
	}
	if(finished){
		printf("Finished ");
	}
	printf("\n");
	

}


Process *Running_thread=NULL;
Process *ReadyQueue=NULL;

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)



void yeld(){
	 Running_thread->sleeping=1;
	 Running_thread->state=Ready; // Cedeu 
	 int swapcontext_result=swapcontext(&Running_thread->contexto, &Running_thread->caller);
	 Running_thread->state=Running; // Recebeu novamente
	 Running_thread->sleeping=0;

}

int
func1(int arg)
{
	
    printf("func1: started\n");
	
	int x;
	for(x=0;x<10;x++){
	    printf("func1: yeld! ill be back!\n");
		yeld();
		printf("func1: im back!!\n");
	}
    
    printf("func1: cya\n");

}



static void
func2(int meuid)
{
	int x=0;
	printf("Thread%d started\n", meuid);
	 for(x=2;x<meuid;x++){
		printf("Thread%d computation %d\n",meuid, x);
		printf("Thread%d yeld! ill be back!\n", meuid);
		yeld();
		printf("Thread%d im back!!\n", meuid);
	}		
	printf("Thread%d near finishing, but not now.\n", meuid);
	yeld();
	
    printf("Thread%d cya\n", meuid);
}




Process* create_thread( void* func , void *arg){
	Process *myProc;
	int its_first_thread_created = !ReadyQueue;
	
	myProc=(Process *) malloc(sizeof(Process) );
	if (getcontext(&myProc->contexto ) == -1){
        handle_error("getcontext");
	}
	
	if(its_first_thread_created){
		ReadyQueue=myProc;
	}
	// Process_debug(Running_thread);
	myProc->sleeping=1;
	myProc->state=Ready;
	
	myProc->contexto.uc_stack.ss_sp=myProc->stack;
	myProc->contexto.uc_stack.ss_size=sizeof(myProc->stack);
	myProc->contexto.uc_link = &myProc->caller;
	makecontext(&myProc->contexto, func, 1, arg );
	return myProc;
}



int run_thread(Process *myProc){



Running_thread=myProc;
myProc->state=Running;
int swapcontext_result=swapcontext(&myProc->caller, &myProc->contexto);


int error_on_swapcontext=0;
error_on_swapcontext = (swapcontext_result==-1);

if (error_on_swapcontext)
        handle_error("swapcontext");

return swapcontext_result;

}


void join_thread(Process *thread_that_must_finish){
	
	Process *myRunning_thread;
	myRunning_thread=ReadyQueue;
	int isJoinInsideaThread=Running_thread!=0;
	
	if(isJoinInsideaThread){
		Running_thread->state=Blocked;
	}
	
	
	while(thread_that_must_finish->state==Ready ){ 
		if(myRunning_thread->state==Ready){
			run_thread(myRunning_thread);
		}
		myRunning_thread=myRunning_thread->next;
	}
	
}




int main(int argc, char *argv[])
{

	Process *lista[15];
	
	int x;
	int i=500;
	for(x=0;x<15;x++){
		lista[x]=create_thread(func2,(void *)x);
	}

	//Lista fake prototipo
	lista[0]->next=lista[1];
	lista[0]->prev=lista[14];
	lista[14]->next=lista[0];
	lista[14]->prev=lista[13];

	for(x=1;x<14;x++){
		lista[x]->next=lista[x+1];
		lista[x]->prev=lista[x-1];
	}
	
	// Process_debug(Running_thread);	
	join_thread(lista[7]);
	Process_debug(lista[7]);
	printf("main: le escalonateur, its me, ill join again\n");
	join_thread(lista[10]);
	
	printf("main: le escalonateur, its me again , ill join again\n");
	
	join_thread(lista[14]);
	
	
	
	// myProc=create_thread(func1,(void *)i);
	// myProc2=create_thread(func2,(void *)i);
	
	// start_thread(myProc);
	
	// while(myProc->sleeping){
		// if(myProc->sleeping)
			// start_thread(myProc);

		// if(myProc2->sleeping)
			// start_thread(myProc2);
	// }
	

    
    printf("main: exiting\n");
    exit(EXIT_SUCCESS);
}

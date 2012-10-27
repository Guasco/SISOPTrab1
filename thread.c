#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#define STACKSIZE 32768

static ucontext_t uctx_main, uctx_func1, uctx_func2;


ucontext_t  ContextP, ContextQ, ContextMain;;

typedef struct Processo {
  int var1;
  float var2;
  ucontext_t contexto;
  ucontext_t caller;
  int sleeping;
  int estado;
  char stack[STACKSIZE];

  struct Processo *next;
  struct Processo *prev;

} Processo;


void Processo_debug(Processo *meuProc){
	printf("me %p \n",meuProc);
	printf("next %p\n",meuProc->next);
	printf("prev %p\n",meuProc->prev);
	int suspended=meuProc->sleeping;
	int finished=!suspended;
	
	if(suspended){
		printf("Suspended ");
	}
	if(finished){
		printf("Finished ");
	}
	printf("\n");
	

}


Processo *Running_thread=NULL;

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)



void yeld(){
	 Running_thread->sleeping=1;
	 int swapcontext_result=swapcontext(&Running_thread->contexto, &Running_thread->caller);
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
	for(x=0;x<meuid;x++){
		printf("Thread%d computation %i\n",x);
		printf("Thread%d yeld! ill be back!\n", meuid);
		yeld();
		printf("Thread%d im back!!\n", meuid);
	}
    printf("Thread%d cya\n", meuid);
}




Processo* create_thread( void* func , void *arg){
	Processo *meuProc;
	int its_first_thread_created = !Running_thread;
	
	meuProc=(Processo *) malloc(sizeof(Processo) );
	if (getcontext(&meuProc->contexto ) == -1){
        handle_error("getcontext");
	}
	
	if(its_first_thread_created){
		Running_thread=meuProc;
	}
	// Processo_debug(Running_thread);
	meuProc->sleeping=1;
	meuProc->contexto.uc_stack.ss_sp=meuProc->stack;
	meuProc->contexto.uc_stack.ss_size=sizeof(meuProc->stack);
	meuProc->contexto.uc_link = &meuProc->caller;
	makecontext(&meuProc->contexto, func, 1, arg );
	return meuProc;
}



int start_thread(Processo *meuProc){


Running_thread=meuProc;
int swapcontext_result=swapcontext(&meuProc->caller, &meuProc->contexto);

int error_on_swapcontext=0;
error_on_swapcontext = (swapcontext_result==-1);

if (error_on_swapcontext)
        handle_error("swapcontext");

return swapcontext_result;

}


void join_thread(Processo *thread_that_must_finish){

	int exist_pending_thread=1;
	Processo *Running;
	
	Running=Running_thread;
	
	// Running=thread_that_must_finish;
	while(exist_pending_thread && thread_that_must_finish->sleeping ){ 
		Running=Running->next;
		exist_pending_thread=0;
		if(Running->sleeping){
			exist_pending_thread=1;
			start_thread(Running);
		}
	}

}




int main(int argc, char *argv[])
{

	Processo *meuProc, *meuProc2, *lista[15];
	
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
	
	Processo_debug(Running_thread);	
	join_thread(lista[7]);
	printf("main: le escalonateur, its me, ill join again\n");
	join_thread(lista[10]);
	
	printf("main: le escalonateur, its me again , ill join again\n");
	
	join_thread(lista[14]);
	
	
	
	// meuProc=create_thread(func1,(void *)i);
	// meuProc2=create_thread(func2,(void *)i);
	
	// start_thread(meuProc);
	
	// while(meuProc->sleeping){
		// if(meuProc->sleeping)
			// start_thread(meuProc);

		// if(meuProc2->sleeping)
			// start_thread(meuProc2);
	// }
	

    
    printf("main: exiting\n");
    exit(EXIT_SUCCESS);
}

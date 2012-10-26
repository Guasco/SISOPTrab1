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

Processo *Running_thread;

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
	printf("func2: started\n");
    printf("func2: yeld! ill be back!\n");
	printf("func1: Sou a Thread %d\n", meuid);
	yeld();
    printf("func2: im back!!\n");
    printf("func2: cya\n");
}




Processo* create_thread( void* func , void *arg){
	Processo *meuProc;
	meuProc=(Processo *) malloc(sizeof(Processo) );
	if (getcontext(&meuProc->contexto ) == -1){
        handle_error("getcontext");
	}

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




int main(int argc, char *argv[])
{

	Processo *meuProc, *meuProc2, *lista[15];
	
	int x;
	int i=500;
	for(x=0;x<15;x++){
		lista[x]=create_thread(func2,(void *)x);
		start_thread(lista[x]); //// Tansformar em lista encadeada
	}
	
	int pending_thread=1;
	while(pending_thread){ 
		pending_thread=0;
		for(x=0;x<15;x++){
			if(lista[x]->sleeping){
				pending_thread=1;
				start_thread(lista[x]);
			}
		}
	}
	
	
	meuProc=create_thread(func1,(void *)i);
	meuProc2=create_thread(func2,(void *)i);
	start_thread(meuProc);
	
	
	
	while(meuProc->sleeping){
		if(meuProc->sleeping)
			start_thread(meuProc);

		if(meuProc2->sleeping)
			start_thread(meuProc2);
	}
	

    
    printf("main: exiting\n");
    exit(EXIT_SUCCESS);
}

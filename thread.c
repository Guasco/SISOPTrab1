#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

static ucontext_t uctx_main, uctx_func1, uctx_func2;


typedef struct Processo {
  int var1;
  float var2;
  ucontext_t contexto;
  ucontext_t caller;
  int estado;
  char stack[16384];
  
  struct Processo *next;
  struct Processo *prev;
  
} Processo;

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void
func1(void)
{
    printf("func1: started\n");
    printf("func1: cya\n");
    // printf("func1: swapcontext(&uctx_func1, &uctx_func2)\n");
    // if (swapcontext(&uctx_func1, &uctx_func2) == -1)
        // handle_error("swapcontext");
    // printf("func1: returning\n");
}

static void
func2(void)
{
    printf("func2: started\n");
    printf("func2: swapcontext(&uctx_func2, &uctx_func1)\n");
    if (swapcontext(&uctx_func2, &uctx_func1) == -1)
        handle_error("swapcontext");
    printf("func2: returning\n");
}

Processo* create_thread(  ){
	Processo *meuProc;
	meuProc=(Processo *) malloc(sizeof(Processo) );
	if (getcontext(&meuProc->contexto ) == -1)
        handle_error("getcontext");
	
	meuProc->contexto.uc_stack.ss_sp=meuProc->stack;
	meuProc->contexto.uc_stack.ss_size=sizeof(meuProc->stack);
	meuProc->contexto.uc_link = &meuProc->caller;
	makecontext(&meuProc->contexto, func1, 0);
	return meuProc;
}

int start_thread(Processo *meuProc){

int swapcontext_result=swapcontext(&meuProc->caller, &meuProc->contexto);
int error_on_swapcontext=0;
error_on_swapcontext = (swapcontext_result==-1);

if (error_on_swapcontext)
        handle_error("swapcontext");

return swapcontext_result;

}


int main(int argc, char *argv[])
{
    char func1_stack[16384];
    char func2_stack[16384];
	
	Processo *meuProc;
	// meuProc=(Processo *) malloc(sizeof(Processo) );
	
	// prtContext=&meuProc->contexto;
	 // if (getcontext(&meuProc->contexto) == -1)
         // handle_error("getcontext");
	
	// meuProc->contexto.uc_stack.ss_sp=meuProc->stack;
	// meuProc->contexto.uc_stack.ss_size=sizeof(meuProc->stack);
	meuProc=create_thread();
	
	
	printf("main: ill start a thread\n");
	start_thread(meuProc);
   
   printf("main: exiting\n");
    exit(EXIT_SUCCESS);
}
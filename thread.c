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
  char stack[16384];

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
	 // Running_thread->
}

int
func1(void)
{
    printf("func1: started\n");
    printf("func1: yeld! ill be back!\n");
	yeld();
    printf("func1: im back!!\n");
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
    printf("func2: yeld! ill be back!\n");
	yeld();
    printf("func2: im back!!\n");
    printf("func2: cya\n");
}

/////////////////////////////////////
void funcP( void *arg)

{ int i ;

   printf ("%s iniciada\n", (char *) arg) ;

   for (i=0; i<4; i++)
   {
      printf ("%s %d\n", (char *) arg, i) ;
      swapcontext (&ContextP, &ContextQ);
   }
   printf ("%s FIM\n", (char *) arg) ;

   swapcontext (&ContextP, &ContextMain) ;
}

//////////////////////////////////////////

void funcQ (void * arg)
{
   int i ;

   printf ("%s iniciada\n", (char *) arg) ;

   for (i=0; i<4; i++)
   {
      printf ("%s %d\n", (char *) arg, i) ;
      swapcontext (&ContextQ, &ContextP);
   }
   printf ("%s FIM\n", (char *) arg) ;

   swapcontext (&ContextQ, &ContextMain) ;
}









Processo* create_thread( void* func ){
	Processo *meuProc;
	meuProc=(Processo *) malloc(sizeof(Processo) );
	if (getcontext(&meuProc->contexto ) == -1)
        handle_error("getcontext");

	meuProc->contexto.uc_stack.ss_sp=meuProc->stack;
	meuProc->contexto.uc_stack.ss_size=sizeof(meuProc->stack);
	meuProc->contexto.uc_link = &meuProc->caller;
	meuProc->sleeping=0;
	makecontext(&meuProc->contexto, func, 0);
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
    char func1_stack[16384];
    char func2_stack[16384];
    char *stack ;

	Processo *meuProc, *meuProc2;

	meuProc=create_thread(func1);
	meuProc2=create_thread(func2);


	printf("main: ill start a thread\n");
	start_thread(meuProc);
	printf("Sleep: %d\n", meuProc->sleeping);
	printf("main: escalonating\n");
	start_thread(meuProc2);
	
	start_thread(meuProc);
	printf("Sleep: %d\n", meuProc->sleeping);
	
	start_thread(meuProc2);

    printf("main: exiting\n");
   
    printf("main: exiting\n");
    exit(EXIT_SUCCESS);
}

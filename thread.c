#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#define STACKSIZE 32768

static ucontext_t uctx_main, uctx_func1, uctx_func2;

ucontext_t *Running_thread, ContextP, ContextQ, ContextMain;;

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



void yeld(){
	int swapcontext_result=swapcontext(Running_thread, Running_thread->uc_link);
}

void
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
    printf("func2: swapcontext(&uctx_func2, &uctx_func1)\n");
    if (swapcontext(&uctx_func2, &uctx_func1) == -1)
        handle_error("swapcontext");
    printf("func2: returning\n");
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


Running_thread=&meuProc->contexto;
int swapcontext_result=swapcontext(&meuProc->caller, &meuProc->contexto);
Running_thread=&meuProc->caller;
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
	printf("main: escalonating\n");
	start_thread(meuProc);

   printf("main: exiting\n");
 //////////////////////////////////////////////////////
    getcontext (&ContextP);

   stack = malloc (STACKSIZE) ;
   if (stack)
   {
      ContextP.uc_stack.ss_sp = stack ;
      ContextP.uc_stack.ss_size = STACKSIZE;
      ContextP.uc_stack.ss_flags = 0;
      ContextP.uc_link = 0;
   }
   else
   {
      perror ("Erro na criação da pilha: ");
      exit (1);
   }

   makecontext (&ContextP, (void*)(*funcP), 1, "    P");

   getcontext (&ContextQ);

   stack = malloc (STACKSIZE) ;
   if (stack)
   {
      ContextQ.uc_stack.ss_sp = stack ;
      ContextQ.uc_stack.ss_size = STACKSIZE;
      ContextQ.uc_stack.ss_flags = 0;
      ContextQ.uc_link = 0;
   }
   else
   {
      perror ("Erro na criação da pilha: ");
      exit (1);
   }

   makecontext (&ContextQ, (void*)(*funcQ), 1, "        Q");

   swapcontext (&ContextMain, &ContextP);
   swapcontext (&ContextMain, &ContextQ);

    exit(EXIT_SUCCESS);
}

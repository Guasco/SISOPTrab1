#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#define STACKSIZE 32768



int *frames;
int *pages;
int MAX_PAGE=0;
int MAX_FRAME=0;

int mv_start(int num_pages, int num_frames){

	frames= malloc(sizeof(int)*num_frames );
	pages= malloc(sizeof(int)*num_pages );
	MAX_PAGE=num_pages;
	MAX_FRAME =num_frames;
	
	int x;
	for(x=0;x<num_pages;x++){	pages[x]=0;	}
	for(x=0;x<num_frames;x++){	frames[x]=0;}
	
	// frames= malloc(sizeof(int) );
	// pages= malloc(sizeof(int) );
	
}
int mv(int page, int *frame){
	*frame=page;
}

int main(int argc, char *argv[])
{

	mv_start(5000, 1000);
	frames[0]=0;
	frames[1]=1;
	frames[2]=2;
	int *meuframe;
	int x;
	// for(x=0;x<5000;x++){
		// frames[x]=x*1000+1;
	// }
	meuframe = (int*) malloc(sizeof(int));
	*meuframe=7;
	int minha_pagina=16;
	mv(minha_pagina, meuframe);
	for(x=0;x<5000;x++){
		printf("abc  %i \n",frames[x]);
	}
}



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
	meuProc->sleeping=0;
	makecontext(&meuProc->contexto, func1, 0);
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





int main_old(int argc, char *argv[])
{
    char func1_stack[16384];
    char func2_stack[16384];
    char *stack ;

	Processo *meuProc;

	meuProc=create_thread();


	printf("main: ill start a thread\n");
	start_thread(meuProc);
	printf("Sleep: %d\n", meuProc->sleeping);
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

	
	printf("Sleep: %d\n", meuProc->sleeping);
   
    printf("main: exiting\n");
    exit(EXIT_SUCCESS);
}

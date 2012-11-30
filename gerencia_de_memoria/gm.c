#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#define STACKSIZE 32768
#define LRU_MODE 1
#define FIFO_MODE 2




int *page_on_frame;
int *frame_assigned_to_page;
int *frame_time;
int MAX_PAGE=0;
int MAX_FRAME=0;
int FRAME_VITIMA=0;
int mem_time=0;

///SELECAO DE MODO//
int MODE=LRU_MODE;
// int MODE=FIFO_MODE;

int mv_start(int num_pages, int num_frames){

	page_on_frame= malloc(sizeof(int)*num_frames );
	frame_assigned_to_page= malloc(sizeof(int)*num_pages );
	frame_time= malloc(sizeof(int)*num_pages );
	MAX_PAGE=num_pages;
	MAX_FRAME =num_frames;
	
	int x;
	for(x=0;x<num_pages;x++){	
		frame_assigned_to_page[x]=-1;	
	}
	for(x=0;x<num_frames;x++){	
		frame_time[x]=0;	
		page_on_frame[x]=-1;
	}
	
	// frames= malloc(sizeof(int) );
	// pages= malloc(sizeof(int) );
	
}


int mv(int page, int *frame){
	switch(MODE){
		case FIFO_MODE: return mv_fifo(page,frame);
		break;
		case LRU_MODE: return mv_lru(page,frame);
		break;
	}

}
#define page_miss frame_assigned_to_page[this_page]==-1
#define page_vitima page_on_frame[FRAME_VITIMA]
#define SWAP_OUT -1
#define FREE -1
#define NONE -1
#define this_frame frame_assigned_to_page[this_page]
#define print(a)  printf("%i\n",a)


int mv_lru(int this_page, int *frame){
	// pages[page]=1;
	int result=0;
	mem_time++;

	if(page_miss){
		printf("page miss %i => %i  (%i)  ",this_page,FRAME_VITIMA, page_vitima);
		frame_assigned_to_page[page_vitima]=SWAP_OUT;
		this_frame=FRAME_VITIMA;
		page_on_frame[FRAME_VITIMA]=this_page;
		frame_time[this_frame]=mem_time;
		result =1;
		int x=0;
		int know_oldest=frame_time[FRAME_VITIMA]+1;
		int min=mem_time;
		int found=0;
		
		
		while(x<MAX_FRAME && !found	){
			if(page_on_frame[x]==NONE
			|| frame_time[x]==know_oldest)
			{
				// print("page_on_frame[%i]=%i\n",x,page_on_frame[x]);
				// print("frame_time[x]=%i\n",x,frame_time[x]);
				// print("frame_time[x]=%i\n",x,frame_time[x]);
				FRAME_VITIMA=x;
				// printf("\nx:%i\n",x);
				found=1;
			}else if(min>frame_time[x]){
				min=frame_time[x];
				FRAME_VITIMA=x;
			}
			x++;
		}
	}/*elseif(HIT)*/else{
		printf("hit! page(%i) => frame(%i)    ",this_page,this_frame);
		frame_time[this_frame]=mem_time;
		
	}
	
	printf("next_vitima(%i)\n",FRAME_VITIMA);
	*frame=this_frame;
	
	int x;
	for(x=0;x<MAX_FRAME;x++){
		printf("Frame[%i] Page= %i   FrameTime=%i\n",x,page_on_frame[x],frame_time[x] );
	
	}
	printf("\n");
	return result;
}

int mv_fifo(int this_page, int *frame){
	// pages[page]=1;
	int result=0;
	
	if(page_miss){
		printf("page miss page(%i) => frame(%i)  page_out(%i)  \n",this_page,FRAME_VITIMA, page_vitima);
		frame_assigned_to_page[page_vitima]=SWAP_OUT;
		this_frame=FRAME_VITIMA;
		page_on_frame[FRAME_VITIMA]=this_page;
		result =1;
		FRAME_VITIMA++;
	}else{
		printf("hit! page(%i) => frame(%i)    \n",this_page,this_frame);
	}
	
	*frame=this_frame;
	if(FRAME_VITIMA==MAX_FRAME){
		FRAME_VITIMA=0;
	}
	return result;
}


int main(int argc, char *argv[])
{
	
	mv_start(5, 3);
	
	
	// mv(5,meuframe);
	int x;
	// for(x=0;x<5000;x++){
		// frames[x]=x*1000+1;
	// }
	int *meuframe;
	meuframe = (int*) malloc(sizeof(int));
	*meuframe=7;
	int minha_pagina=16;
	mv(1, meuframe);
	mv(1, meuframe);
	mv(1, meuframe);
	mv(2, meuframe);
	mv(3, meuframe);
	mv(4, meuframe);
	mv(0, meuframe);
	mv(2, meuframe);
	mv(2, meuframe);
	printf("abc  %i \n",*meuframe);
	print(page_on_frame[2]);
	
	// for(x=0;x<5000;x++){
		// printf("abc  %i \n",frames[x]);
	// }
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

#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
<<<<<<< HEAD
#include "uthread.h"
=======
#include <string.h>
>>>>>>> Funcao Fila
#define STACKSIZE 32768



ucontext_t  ContextP, ContextQ, ContextMain;;

int last_thread_id=0;

void Process_debug(Process *myProc){
<<<<<<< HEAD
=======

>>>>>>> Funcao Fila
	printf("Thread = {  \n");
	printf("	   id = %i \n",myProc->id);
	printf("	 next = %i\n",myProc->next->id);
	printf("	 prev = %i\n",myProc->prev->id);
	printf("	state = %s \n",enumDesc[myProc->state]);
	printf("} \n");
<<<<<<< HEAD
=======


>>>>>>> Funcao Fila
}


Process *Running_thread=NULL;
Process *ReadyQueue=NULL;

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
///Fila

<<<<<<< HEAD

struct fila *criaFila(){
	struct fila *ptrFila = (struct fila *) malloc(sizeof(struct fila));
    ptrFila->primeiro = ptrFila->ultimo = NULL;
    ptrFila->contador=FILA_VAZIA;
    return(ptrFila);
=======
typedef struct registro{
   char nome[20];
   int num;
   struct registro *next;
}Fila;

/*FUNCAO PRA A LIMPEZA DO BUFFER DO TECLADO*/

void flush_in()
{
   int ch;
   while( (ch = fgetc(stdin)) != EOF && ch != '\n' ){}
}


/*FUNCAO INICIALIZA*/

void inicializa(Fila **fila, Fila **ult){
   *fila = NULL;
   *ult = NULL;
}


/*FUNCAO PARA INSERIR NO FIM*/

void enqueue(Fila **fila, Fila **ult){
   Fila *novoFunc;
   int numero;

   printf("\nQuantos na fila? ");
   scanf("%d", &numero);
   flush_in();
   for(numero; numero > 0; numero--){
      novoFunc = ((Fila *)malloc(sizeof(Fila)));
      printf("\n|Funcionario|\n\n");
      printf("Nome: ");
      scanf("%s", novoFunc -> nome);
      flush_in();
      printf("Codigo: ");
      scanf("%d", &novoFunc -> num);
      flush_in();
      novoFunc -> next = NULL;

      if(*fila == NULL){
         *fila = novoFunc;
         *ult = novoFunc;
      }else{
         (*ult) -> next = novoFunc;
         *ult = (*ult) -> next;
      }
   }
}


/*FUNCAO PARA RETIRAR ELEMETO DA FILA*/

void Desenqueue(Fila **fila, Fila **ult){
   Fila *pont;
   int numero;

   printf("\nRetirar Quantos? ");
   scanf("%d", &numero);
   flush_in();
   for(numero; numero > 0; numero--){
      if(*fila != NULL){
         if(*fila == *ult){
            pont = *ult;
            *fila = NULL;
            *ult = NULL;
            free(pont);
         }else{
            pont = *fila;
            *fila = (*fila) -> next;
            free(pont);
         }
      }
   }
>>>>>>> Funcao Fila
}

/*FUNCAO CONSULTA*/

void consulta(Fila *fila, Fila *ult){
   int numero;

   printf("\nNumero: ");
   scanf("%d", &numero);
   if(fila != NULL){
      while(ult -> next != fila){
         if(fila -> num == numero){
            printf("\nNome: %s\n", fila -> nome);
            printf("Codigo: %d\n", fila -> num);
            getchar();
            flush_in();
            break;
         }else
            fila = fila -> next;
      }
   }
}

/*FUNCAO PRINCIPAL*/
/*
int main(){
   char op[1];
   Fila *fila;
   Fila *ult;

   inicializa(&fila, &ult);
   for(;;){
      system("clear");
      printf("===== Programa FILA =====\n\n");
      printf("Sequencia de Operacoes:\n\n");
      printf("a. Enqueue; \n");
      printf("b. Desenqueue; \n");
      printf("c. Consulta;\n");
      printf("s. SAIR.\n\n");
      printf("OPCAO: ");
      scanf("%s", op);
      flush_in();
      switch(toupper(op[0])){
         case 'A':{
            enqueue(&fila, &ult);
         }break;
         case 'B':{
            Desenqueue(&fila, &ult);
         }break;
         case 'C':{
            consulta(fila, ult);
         }break;
         case 'S':{
            printf("\nTem certeza? (S/N) ");
            scanf("%s", op);
            flush_in();
            if (toupper(op[0]) == 'S'){
               system("clear");
               exit(0);
            }
         }break;
      }
   }
}
*/
void Debug_elemento(struct elemento *myElemento){
	printf("elemento= {\n");
	printf("	valor: %i\n", myElemento->valor);
	printf("	 next: %p\n", myElemento->proximo);
	printf("	 prev: %p\n", myElemento->anterior);
	printf("}");


}



void fila_tests(){
	printf("teste de fila\n");
	Fila *myFila;
	struct elemento *myElemento;
	myFila= criaFila();
	myElemento=criaElemento();
	Debug_elemento(myElemento);
}

<<<<<<< HEAD
	
void uthread_yield(){
=======

void yeld(){
>>>>>>> Funcao Fila
	 // Running_thread->sleeping=1;
	 Running_thread->state=Ready; // Cedeu
	 swapcontext(&Running_thread->contexto, &Running_thread->caller);
	 Running_thread->state=Running; // Recebeu cpu novamente

	 // Running_thread->sleeping=0;

}

Process* uthread_create( void* func , void *arg){
	Process *myProc;
	int its_first_thread_created = !ReadyQueue;

	myProc=(Process *) malloc(sizeof(Process) );
	if (getcontext(&myProc->contexto ) == -1){
        handle_error("getcontext");
	}

	myProc->id=last_thread_id++;
	if(its_first_thread_created){
		ReadyQueue=myProc;
	}
	// Process_debug(Running_thread);
	// myProc->sleeping=1;
	myProc->state=Ready;

	myProc->contexto.uc_stack.ss_sp=myProc->stack;
	myProc->contexto.uc_stack.ss_size=sizeof(myProc->stack);
	myProc->contexto.uc_link = &myProc->caller;
	makecontext(&myProc->contexto, func, 1, arg );
	return myProc;
}



int run_thread(Process *myProc){


	Process *Prev_Running_thread=Running_thread;

	Running_thread=myProc;
	myProc->state=Running;

	int swapcontext_result=swapcontext(&myProc->caller, &myProc->contexto);
	if(myProc->state==Running){
		myProc->state=Finished;
	}
	// Process_debug(myProc);

	Running_thread=Prev_Running_thread;

	int error_on_swapcontext=0;
	error_on_swapcontext = (swapcontext_result==-1);

	if (error_on_swapcontext)
			handle_error("swapcontext");

	return swapcontext_result;

}


void join_thread(Process *thread_that_must_finish){

	Process *myRunning_thread;
	myRunning_thread=ReadyQueue;

	Process *JoinCaller;
	JoinCaller=Running_thread;


	int isJoinInsideaThread=Running_thread!=0;

	if(isJoinInsideaThread){
		JoinCaller->state=Blocked;
	}

	while(	thread_that_must_finish->state==Ready 	||
			thread_that_must_finish->state==Blocked
		){
		if(myRunning_thread->state==Ready){
			run_thread(myRunning_thread);
		}
		myRunning_thread=myRunning_thread->next;
	}

	if(isJoinInsideaThread){
		Process_debug(JoinCaller);
		JoinCaller->state=Blocked;
	}
	thread_that_must_finish->state=Finished;

}

Process *lista[15];

int func1(int arg)
{

    printf("func1: started\n");



	int x;
	for(x=0;x<10;x++){
	    printf("func1: yeld! ill be back!\n");
		uthread_yield();
		printf("func1: im back!!\n");
	}

    printf("func1: cya\n");
	return 0;

}



static void
func2(int meuid)
{
	int x=0;

	if(meuid==6){
		printf("Thread%d i need wait Thread8 finish, then i back\n", meuid);
		join_thread(lista[8]);
		printf("Thread%d the Thread8 has finished, here i go again\n", meuid);
	}

	printf("Thread%d started\n", meuid);
	 for(x=2;x<meuid;x++){
		printf("Thread%d computation %d\n",meuid, x);
		printf("Thread%d yeld! ill be back!\n", meuid);
		uthread_yield();
		printf("Thread%d im back!!\n", meuid);
	}
	printf("Thread%d near finishing, but not now.\n", meuid);
<<<<<<< HEAD
	uthread_yield();
	
=======
	yeld();

>>>>>>> Funcao Fila
    printf("Thread%d cya\n", meuid);
}





int main(int argc, char *argv[])
{



	int x;
	for(x=0;x<15;x++){
		lista[x]=uthread_create(func2,(void *)x);
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
	fila_tests();
    exit(EXIT_SUCCESS);
}

#include <stdio.h>
#include <stdlib.h>
#include "../include/uthread.h"
#define STACKSIZE 32768

int last_thread_id=0;
int debug=1;
static char *enumDesc[] = {"Running", "Blocked", "Ready","Finished" };
void Debug_process(Process *myProc){
	if(!debug) return;
	printf("Thread = {  \n");
	printf("	   id = %i \n",myProc->id);
	if(myProc->next) printf("	 next = %i\n",myProc->next->id);
	if(myProc->prev)printf("	 prev = %i\n",myProc->prev->id);
	printf("	state = %s \n",enumDesc[myProc->state]);
	printf("} \n");
}





Process *Running_thread=NULL;
Process *ReadyQueue=NULL;
Process *BlockedQueue=NULL;

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)


struct fila *criaFila(){
	struct fila *ptrFila = (struct fila *) malloc(sizeof(struct fila));
    ptrFila->primeiro = ptrFila->ultimo = NULL;
    ptrFila->contador=FILA_VAZIA;
    return(ptrFila);
}
//* Função para armazenar um elemento da fila */

struct elemento *criaElemento() {
    struct elemento *ptrElemento = (struct elemento *) malloc(sizeof(struct elemento));
    ptrElemento->nome = (char *) malloc(sizeof(char) * TAMANHO_NOME);
    ptrElemento->anterior = ptrElemento->proximo = NULL;
    return(ptrElemento);
}
/* Função que insere um elemento na fila */




int insereElemento (struct elemento* ptrElemento, struct fila* ptrFila, int posicao){
	// int i;
    // struct elemento *ptrAux = NULL;
/* Checando se a posicão é valida */
if ((posicao > ptrFila->contador+1) || (posicao < 0)) return(-1);
/* Iniciando a insercão */
/* Se for inserir na primeira posicão e já há elementos */
    else if ((!posicao) && (ptrFila->contador > FILA_VAZIA)){
        ptrElemento->proximo = ptrFila->primeiro;
        ptrFila->primeiro = ptrElemento;
        ptrElemento->proximo->anterior = ptrElemento;
        ptrFila->contador++;
        ptrElemento->valor = 1;
        ptrElemento = ptrElemento->proximo;
        for (;ptrElemento; ptrElemento = ptrElemento->proximo)
            ptrElemento->valor++;
    }
/* Inserindo na última posicão */
	else if (posicao == ptrFila->contador+1) {
		ptrFila->ultimo->proximo = ptrElemento;
		ptrElemento->anterior = ptrFila->ultimo;
		ptrFila->ultimo = ptrElemento;
		ptrFila->contador++;
		ptrElemento->valor = posicao + 1;
	}
	/* Ocorreu algum erro */
	else{
		return(-2);
	} 
	/* Não ocorrram erros. Finalizando */
	return(0);
}
void Debug_elemento(struct elemento *myElemento){
	if(!debug) return;
	printf("elemento= {\n");
	printf("	valor: %i\n", myElemento->valor);
	printf("	 next: %p\n", myElemento->proximo);
	printf("	 prev: %p\n", myElemento->anterior);
	printf("}");
	

}



// void fila_tests(){
	// printf("teste de fila\n");
	// Fila *myFila;
	// struct elemento *myElemento;
	// myFila= criaFila();
	// myElemento=criaElemento();
	// Debug_elemento(myElemento);
// }

	
void uthread_yield(){
	 // Running_thread->sleeping=1;
	 Running_thread->state=Ready; // Cedeu 
	 swapcontext(&Running_thread->contexto, &Running_thread->caller);
	 Running_thread->state=Running; // Recebeu cpu novamente

	 // Running_thread->sleeping=0;

}

Process* uthread_create( void* func , void *arg){
	Process *myProc;
	myProc=(Process *) malloc(sizeof(Process) );
	if (getcontext(&myProc->contexto ) == -1){
        return (Process*)-1;
	}
	
	myProc->id=last_thread_id++;
	
	// Process_debug(Running_thread);
	// myProc->sleeping=1;
	
	myProc->contexto.uc_stack.ss_sp=myProc->stack;
	myProc->contexto.uc_stack.ss_size=sizeof(myProc->stack);
	myProc->contexto.uc_link = &myProc->caller;
	makecontext(&myProc->contexto, func, 1, arg );
	
	SetReady(myProc);
	myProc->state=Ready;
	
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


int join_thread(Process *thread_that_must_finish){
	
	if(!thread_that_must_finish){
		return -1;
	}
	// Process *myRunning_thread;
	// myRunning_thread=ReadyQueue;
	
	Process *JoinCaller;
	JoinCaller=Running_thread;
	
	
	int isJoinInsideaThread=Running_thread!=0;
	
	if(isJoinInsideaThread){
		JoinCaller->state=Blocked;
		// Debug_process(JoinCaller);
		SetBlocked(JoinCaller);
		// Debug_process(ReadyQueue);
		// exit(0);
		
	}
	
	while(	ReadyQueue && (
			thread_that_must_finish->state==Ready 	||  
			thread_that_must_finish->state==Blocked	)
		){ 
		if(ReadyQueue->state==Ready){
			run_thread(ReadyQueue);
		}
		ReadyQueue=ReadyQueue->next;
	}
	if(isJoinInsideaThread){
		SetReady(JoinCaller);
		// Process_debug(JoinCaller);
		JoinCaller->state=Ready;
	}
	
	thread_that_must_finish->state=Finished;
	
	return 0;
	
}

void SetTop(Process **queue, Process *element){
	if(*queue==NULL){
		element->next=element;
		element->prev=element;
		
	}
	*queue=element;
}

int push(Process **queue, Process *element){
	if(*queue==NULL){
		*queue=element;
		element->next=element;
		element->prev=element;
		
	}else{
		Process *first;
		Process *last;
		first=*queue;
		last=first->prev;

		element->next=first;
		element->prev=last;
		last->next=element;
		first->prev=element;
	}
	return 0;

}



void remove_element_from_queue( Process *element){
	
	
	int isElementOnQueue=element && element->next && element->prev;
	if(isElementOnQueue){
		Process *nextElement=element->next;
		Process *prevElement=element->prev;
		
		nextElement->prev=prevElement;
		prevElement->next=nextElement;
		element->next=NULL;
		element->prev=NULL;
	}
}

void SetBlocked(Process *myProc){

if(myProc==ReadyQueue){
	if(ReadyQueue!=ReadyQueue->next)
		ReadyQueue=ReadyQueue->next;
	else	
		ReadyQueue=NULL;
		
	
}
remove_element_from_queue(myProc);
push(&BlockedQueue,myProc);
// Debug_process(myProc);

}


void SetReady(Process *myProc){

if(myProc==BlockedQueue){
	if(BlockedQueue!=BlockedQueue->next)
		BlockedQueue=BlockedQueue->next;
	else	
		BlockedQueue=NULL;
}

remove_element_from_queue(myProc);
push(&ReadyQueue,myProc);
// Debug_process(myProc);

}

void Debug_queue(Process *queue){
		if(!debug) return;
		printf("Queue = {\n");
		printf("	First= %p",queue);
		if(queue) printf("	id= %d\n",queue->id);
		if(queue) printf("	Next= %p   id=%d \n",queue->next,queue->next->id);
		if(queue) printf("	Last= %p   id=%d \n",queue->prev,queue->prev->id);
		printf("}\n");
		
}



int filas_testes(){
	Process *myProc;
	Process *myProc2;
	Process *myProc3;
	Process *myProc4;
	
	myProc=(Process *) malloc(sizeof(Process) );
	myProc2=(Process *) malloc(sizeof(Process) );
	myProc3=(Process *) malloc(sizeof(Process) );
	myProc4=(Process *) malloc(sizeof(Process) );
	myProc->id=1;
	myProc2->id=2;
	myProc3->id=3;
	myProc4->id=4;
	push(&ReadyQueue,myProc);
	push(&ReadyQueue,myProc2);
	push(&ReadyQueue,myProc3);
	push(&ReadyQueue,myProc4);
	
	printf("Este teste demonstra o funcionamento das filas\n");
	printf("Quatro threads sao criadas e inseridas na fila Ready\n\n");
	printf("Quatro elementos em uma fila {1,2,3,4}\n");
	Debug_queue(ReadyQueue);
	
	printf("Thread 2 eh bloqueado ReadyQueue= {1,3,4}\n");
	SetBlocked(myProc2);
	Debug_queue(ReadyQueue);
	printf("\n\nBlockedQueue= {2}\n");
	Debug_queue(BlockedQueue);
	SetReady(myProc2);
	
	printf("Thread 2 voltou Ready ReadyQueue= {1,3,4,2}\n");
	Debug_queue(ReadyQueue);
	
	
	
	// Debug_process(myProc2);
	
	
	
	return 0;
}
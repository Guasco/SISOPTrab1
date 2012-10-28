#include <stdio.h>
#include <stdlib.h>
#include "uthread.h"
#define STACKSIZE 32768


int last_thread_id=0;
void Process_debug(Process *myProc){
	printf("Thread = {  \n");
	printf("	   id = %i \n",myProc->id);
	printf("	 next = %i\n",myProc->next->id);
	printf("	 prev = %i\n",myProc->prev->id);
	printf("	state = %s \n",enumDesc[myProc->state]);
	printf("} \n");
}


Process *Running_thread=NULL;
Process *ReadyQueue=NULL;

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
	int i;
    struct elemento *ptrAux = NULL;
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


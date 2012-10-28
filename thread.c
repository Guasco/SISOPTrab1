#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#define STACKSIZE 32768

// static ucontext_t uctx_main, uctx_func1, uctx_func2;


ucontext_t  ContextP, ContextQ, ContextMain;;
enum{
Running,
Blocked,
Ready,
Finished

};
static char *enumDesc[] = {"Running", "Blocked", "Ready","Finished" };
int last_thread_id=0;

typedef struct Process {
  int var1;
  int id;
  float var2;
  ucontext_t contexto;
  ucontext_t caller;
  // int sleeping;
  int state;
  char stack[STACKSIZE];

  struct Process *next;
  struct Process *prev;

} Process;


void Process_debug(Process *myProc){
	
	printf("Thread = {  \n");
	printf("	   id = %i \n",myProc->id);
	printf("	 next = %i\n",myProc->next->id);
	printf("	 prev = %i\n",myProc->prev->id);
	// int suspended=myProc->sleeping;
	printf("	state = %s \n",enumDesc[myProc->state]);
	printf("} \n");
	

}


Process *Running_thread=NULL;
Process *ReadyQueue=NULL;

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define TAMANHO_NOME 30
#define FILA_VAZIA -1



struct elemento {char *nome;
                 int valor;
                 struct elemento *proximo;
                 struct elemento *anterior;
                 };

struct fila {
    struct elemento *primeiro;
    struct elemento  *ultimo;
    int contador;
    };





typedef struct fila Fila;

//funcao para armazenar a estutura base na fila
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



void fila_tests(){
	printf("teste de fila\n");
	Fila *myFila;
	struct elemento *myElemento;
	myFila= criaFila();
	myElemento=criaElemento();
	Debug_elemento(myElemento);
}

	
void yeld(){
	 // Running_thread->sleeping=1;
	 Running_thread->state=Ready; // Cedeu 
	 swapcontext(&Running_thread->contexto, &Running_thread->caller);
	 Running_thread->state=Running; // Recebeu novamente
	 // Running_thread->sleeping=0;

}

Process* create_thread( void* func , void *arg){
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
		yeld();
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
		yeld();
		printf("Thread%d im back!!\n", meuid);
	}		
	printf("Thread%d near finishing, but not now.\n", meuid);
	yeld();
	
    printf("Thread%d cya\n", meuid);
}





int main(int argc, char *argv[])
{

	
	
	int x;
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

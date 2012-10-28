#include <ucontext.h>

#define STACKSIZE 32768
#define TAMANHO_NOME 30
#define FILA_VAZIA -1

enum{
Running,
Blocked,
Ready,
Finished

};
static char *enumDesc[] = {"Running", "Blocked", "Ready","Finished" };
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

void Process_debug(Process *myProc);

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)


// typedef struct fila Fila;

//funcao para armazenar a estutura base na fila
struct fila *criaFila();
//* Função para armazenar um elemento da fila */

struct elemento *criaElemento();
/* Função que insere um elemento na fila */

int insereElemento (struct elemento* ptrElemento, struct fila* ptrFila, int posicao);

void Debug_elemento(struct elemento *myElemento);

void fila_tests();
	
void uthread_yield();

Process* uthread_create( void* func , void *arg);

int run_thread(Process *myProc);
void join_thread(Process *thread_that_must_finish);






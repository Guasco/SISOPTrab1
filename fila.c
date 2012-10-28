# include <stdio.h>
# include <stdlib.h>
# include <string.h>

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


/*FUNCAO ENFILEIRA*/

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


/*FUNCAO DESENFILEIRA*/

void Desenqueue(Fila **fila, Fila **ult){
   Fila *pont;
   int numero;

   printf("\nDesenfileirar Quantos? ");
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
}

void consulta(Fila *fila, Fila *ult){
   int numero;

   printf("\nConsulta Codigo: ");
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


filaTeste(){
    Fila *fila;
    Fila *ult;
    inicializa(&fila, &ult);
    enqueue(&fila, &ult);
    consulta(fila, ult);
    Desenqueue(&fila, &ult);
    consulta(fila, ult);
}

int main(){
            filaTeste();
        //    consulta(fila, ult);

            system("pause");

            return 0;




}

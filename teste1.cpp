#include <stdio.h>
#include <stdlib.h>



typedef struct NODO {
  int var1;
  float var2;
  struct NODO *next;
} NODO;


typedef struct{

char nome[20];
int cont;
}threadRecord;


void pedro(){
 printf("fuckyeahhh\n");
 NODO *minhaThread;
 minhaThread=(NODO *) malloc(sizeof(NODO) );
 minhaThread->var1=4;
 printf("fuckyeahhh %i\n", minhaThread->var1);
 //minhaThread = (*minhaThread)malloc(50);
}

void mike(){

threadRecord *minhaThread;

minhaThread=  (threadRecord *)malloc(sizeof(threadRecord));

minhaThread->cont=0;

printf("%d\n\n",minhaThread->cont+2);

free(minhaThread);

}

int main(){
	printf("a \noasijsoisa\nbc \n");
	mike();
	pedro();
	//system("pause");
	return 0;

}

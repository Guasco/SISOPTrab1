#include <stdio.h>
#include <stdlib.h>


typedef struct NODO {
  int var1;
  float var2;
  struct NODO *next;
} NODO;


void pedro(){
 printf("fuckyeahhh\n");
 NODO *minhaThread;
 minhaThread=(NODO *) malloc(sizeof(NODO) );
 minhaThread->var1=4;
 printf("fuckyeahhh %i\n", minhaThread->var1);
 //minhaThread = (*minhaThread)malloc(50);
}

void mike(){

printf("eu quero terminar o trabalho de SISOP KCT!\n");
}
int main(){
	printf("a \noasijsoisa\nbc \n");
	mike();
	pedro();
	//system("pause");
	return 0;

}

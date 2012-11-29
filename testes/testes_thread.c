#include <stdio.h>
#include <stdlib.h>
#include "../include/uthread.h"

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
	uthread_yield();
	
    printf("Thread%d cya\n", meuid);
}





int main(int argc, char *argv[])
{

	
	
	int x;
	for(x=0;x<15;x++){
		lista[x]=uthread_create(func2,(void *)x);
	}
	
	
	// Process_debug(Running_thread);	
	join_thread(lista[7]);
	Debug_process(lista[7]);
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
	// fila_tests();
    exit(EXIT_SUCCESS);
}

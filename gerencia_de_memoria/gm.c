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
int know_oldest=0;


///SELECAO DE MODO///
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
#define page_miss(this) frame_assigned_to_page[this]==-1
#define page_vitima page_on_frame[FRAME_VITIMA]
#define SWAP_OUT -1
#define FREE -1
#define NONE -1
#define this_frame frame_assigned_to_page[this_page]
#define print(a)  printf("%i\n",a)
void set_page_to_frame(int page, int frame){

	int old_page=page_on_frame[frame];

	page_on_frame[frame]=page;
	frame_assigned_to_page[page]=frame;

	if(old_page!=NONE){
		frame_assigned_to_page[old_page]=SWAP_OUT;
	}

}


int mv_lru(int this_page, int *frame){
	// pages[page]=1;
	int result=0;
	mem_time++;

	if(page_miss(this_page)){
		
		result =1;
		int x=0;
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
		printf("page miss %i =>  vitima frame (%i)  ",this_page,FRAME_VITIMA);

		// frame_assigned_to_page[page_vitima]=SWAP_OUT;
		// this_frame=FRAME_VITIMA;
		// page_on_frame[FRAME_VITIMA]=this_page;
		
		set_page_to_frame(this_page,FRAME_VITIMA);
		
		know_oldest=frame_time[FRAME_VITIMA]+1;
		frame_time[this_frame]=mem_time;
	}/*elseif(HIT)*/else{
		printf("hit! page(%i)    ",this_page,this_frame);
		frame_time[this_frame]=mem_time;
		
	}
	
	printf("\n",FRAME_VITIMA);
	*frame=this_frame;
	
	int x;
	for(x=0;x<MAX_FRAME;x++){
		printf("Frame[%i] Page= %i   FrameTime=%i ",x,page_on_frame[x],frame_time[x] );
		if(x==this_frame){
			if(result){
				printf(" miss");
			}else{
				printf(" hit");
			}
		
		}
		printf("\n");
	}
	printf("\n");
	return result;
}

int mv_fifo(int this_page, int *frame){
	// pages[page]=1;
	int result=0;
	
	if(page_miss(this_page)){
		printf("page miss page(%i) => frame(%i)  page_out(%i)  \n",this_page,FRAME_VITIMA, page_vitima);
		// frame_assigned_to_page[page_vitima]=SWAP_OUT;
		// this_frame=FRAME_VITIMA;
		// page_on_frame[FRAME_VITIMA]=this_page;
		set_page_to_frame(this_page,FRAME_VITIMA);
		FRAME_VITIMA++;
		result =1;
	}else{
		printf("hit! page(%i) => frame(%i)    \n",this_page,this_frame);
	}
	
	*frame=this_frame;

	int x;
	for(x=0;x<MAX_FRAME;x++){
		printf("Frame[%i] Page= %i   FrameTime=%i ",x,page_on_frame[x],frame_time[x] );
		if(page_on_frame[x]==this_page){
			if(result){
				printf(" miss");
			}else{
				printf(" hit");
			}
		
		}
		printf("\n");
	}
	printf("\n");
	
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
	mv(3, meuframe);
	mv(2, meuframe);
	mv(2, meuframe);
	printf("abc  %i \n",*meuframe);
	print(page_on_frame[2]);
	
	// for(x=0;x<5000;x++){
		// printf("abc  %i \n",frames[x]);
	// }
}


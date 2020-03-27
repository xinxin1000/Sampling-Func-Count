#define UNW_LOCAL_ONLY
#include <stdio.h>  
#include <signal.h>   //signal()
#include <string.h>  //memset()
#include <sys/time.h> //struct itimeral setitimer()
#include <stdlib.h>
#include <unistd.h>  //sleep
#include <libunwind.h>


struct FuncRec{
	char name[256];
	int cnt;
}funcrecords[100];
int funcTot = 0;

void alrm_handler(int signum){  //find call frame
	
	unw_cursor_t cursor;
	unw_context_t context;
	
	//initialize a cursor pointing to current frame
	unw_getcontext(&context);
	unw_init_local(&cursor, &context);
	unw_step(&cursor); //killpg
	unw_step(&cursor);

	
	char sym[256];
	unw_word_t offset;
	if(unw_get_proc_name(&cursor, sym, 256, &offset) == 0){
		int i=0;
		for(; i<funcTot; ++i){
			if(strcmp(funcrecords[i].name,sym)==0){  
			//this function name has already been recorded
				funcrecords[i].cnt++;
				break;
			}
		}
		if(i==funcTot){
			funcrecords[funcTot].cnt = 1;
			strcpy(funcrecords[funcTot++].name, sym);
			//printf("%s\n", funcrecords[funcTot-1].name );
		} 
			
		//printf(" %s\n", sym);
		//printf("%d\n", funcTot);
	}
	else{
		printf(" --error: unable to obtain symbol name for this frame\n");
	}
	
}


void a(){for(int i=0; i<6e7; ++i);}
void b(){for(int i=0; i<2e7; ++i);}
void c(){for(int i=0; i<8e7; ++i);}

int main(int argc, char **argv){
	

	//-------------set sigaction handler-----------------------

	
	signal(SIGALRM, alrm_handler);
	

	//-------------set alarm------------------------------------

	struct itimerval tick;  //declare itimerval struct
	memset(&tick, 0, sizeof(tick));

	tick.it_value.tv_sec = 0;  //sec  start time
	tick.it_value.tv_usec = 100000; //micro sec

	tick.it_interval.tv_sec = 0;  //interval time
	tick.it_interval.tv_usec = 100000;  

	
	
	if(setitimer(ITIMER_REAL, &tick, NULL))  //set real-time to decease timer
	{ 	printf("Set timer failed\n"); return 0;}


	//----------- set time counter------------------------------

	struct timeval start, end;
	gettimeofday(&start, NULL);


	//-----------test ------------------------------------------

	

	for(int i=0; i<10; ++i){a(); b(); c();}
	for(int i=0; i<24e7;++i);

	//-----------compute time and func ratio and print------------------

	gettimeofday(&end, NULL);

	double diff;
	diff = 1000000 * (end.tv_sec - start.tv_sec) +end.tv_usec - start.tv_usec;
	diff /= 1000;
	printf("Time duration: %.1lf miliseconds\n", diff);


	//for(int i=0; i<funcTot; ++i) printf("%s %d\n", funcrecords[i].name, funcrecords[i].cnt );

	//sort
	for(int i=0; i<funcTot; ++i){
		
		for(int j=i+1; j<funcTot; ++j){
			if(funcrecords[j].cnt>funcrecords[i].cnt){
				char tmp[256];
				strcpy(tmp, funcrecords[j].name);
				//memset(funcrecords[j].name, 0, 256);
				strcpy(funcrecords[j].name, funcrecords[i].name);
				//memset(funcrecords[i].name, 0, 256);
				strcpy(funcrecords[i].name, tmp);
				int tmpint = funcrecords[j].cnt;
				funcrecords[j].cnt = funcrecords[i].cnt;
				funcrecords[i].cnt = tmpint;
			}
		}
	}
				

	printf("Sampling frequency: %ldHz\n", 1000000/tick.it_interval.tv_usec);


	printf("function\tratio(%%)\trealtime(ms)\n");

	double cntsum = 0;
	for(int i=0; i<funcTot; ++i)
		cntsum += funcrecords[i].cnt;


	double remain = 100;
	double time_remain = diff;

	for(int i=0; i<funcTot-1; ++i){
		double perc = funcrecords[i].cnt/cntsum*100;
		
		printf("%s\t\t%.1lf\t\t%.2lf\n", funcrecords[i].name, perc, diff*perc/100);

		remain-=perc;
		time_remain-= diff*perc/100;
		
	}
	printf("%s\t\t%.1lf\t\t%.2lf\n", funcrecords[funcTot-1].name, remain, time_remain);


	return 0;
}
	
	
	


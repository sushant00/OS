#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define rtnice 323

// help from geeksforgeeks

int main(int argc,char **argv)
{

	int pid = fork();
	if(pid==0){
		//successfully forked: child process
		pid_t tmppid = getpid();
		printf("pid %d\n", tmppid);
		long res = syscall(323, tmppid, 1000000);
		clock_t startTime = clock();
		
		long loopSize = 2e9;
		long loop = 0;
		for(loop = 0; loop < loopSize; loop++){			
		}
		double ex_time = ((double)(clock() - startTime))/CLOCKS_PER_SEC;

		printf("child looped %ld times, took time %lf \n", loopSize, ex_time);
		
		exit(EXIT_SUCCESS);

	}else if(pid==-1){
		//error forking
		fprintf(stderr,"error forking. child process not created");

	}else{
		//successfully forked: parent process
		wait(0);
		pid_t tmp_pid = getpid();
		printf("pid %d \n", tmp_pid);
		//long res=syscall(323,tmp_pid ,1000000);
		 //printf("System call returned %ld\n",res);
		 //printf("Error: %s \n",strerror(errno));

		clock_t startTime = clock();
		
		long loopSize = 2e9;
		long loop = 0;
		for(loop = 0; loop < loopSize; loop++){			
		}
		double ex_time = ((double)(clock() - startTime))/CLOCKS_PER_SEC;

		printf("parent looped %ld times, took time %lf \n", loopSize, ex_time);
		

	}
	return 0;
}

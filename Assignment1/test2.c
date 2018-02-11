#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define sys_sh_task_info 318

int main(int argc,char **argv)
{
	int pid=atoi(argv[1]);
	printf("System call should return -1 and error should be no such process\n");
	long res=syscall(sys_sh_task_info,pid,"abc.txt");
	printf("System call returned %ld\n",res);
	printf("Error: %s\n",strerror(errno));
	return 0;
}

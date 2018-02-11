#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define sys_sh_task_info 318

int main(int argc,char **argv)
{
	int pid=atoi(argv[1]);
	long res=syscall(sys_sh_task_info,pid,"output.txt");
	printf("System calll should return -1 and error should be file does not exist\n");
	printf("System call returned %ld\n",res);
	printf("Error: %s\n",strerror(errno));
	return 0;
}

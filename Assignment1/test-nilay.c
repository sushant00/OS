#include <stdio.h>
#include <errno.h>
#include <string.h>

#define sys_sh_task_info 318

int main(int argc,char **argv)
{
	long res=syscall(sys_sh_task_info,1,"abc.txt");
	printf("System call returned %ld\n",res);
	printf("Error: %s",strerror(errno));
	return 0;
}

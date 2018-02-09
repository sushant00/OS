#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(){
	printf("testing system call 'sh_task_info() :'\n");

	printf("Test1:\n");

	printf("called: 'sh_task_info(1, \"output.txt\")' \n
		expected return value: 0 \n");
	int result1 = sh_task_info(1, "output.txt");
	printf("returned: %d\n", result1);
	if(result1 == -1){
		printf("some error occured, errno: %d\n", errno);
	}

	printf("Test2:\n");

	printf("called: 'sh_task_info(1, \"notExists.txt\")'\n
		expected return value: -1 , errno = %d \n", ENOENT);
	int result2 = sh_task_info(-1, "notExists.txt");
	printf("returned: %d\n", result2);
	if(result2 == -1){
		printf("some error occured, errno: %d\n", errno);
	}

	printf("Test3:\n");

	printf("called: 'sh_task_info(-1, \"output.txt\")'\n
		expected return value: -1, errno = %d\n", ENEXIST);
	int result3 = sh_task_info(-1, "output.txt");
	printf("returned: %d\n", result3);
	if(result3 == -1){
		printf("some error occured, errno: %d\n", errno);
	}


	return 0;
}


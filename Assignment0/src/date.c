#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int date(char **args);


int date(char **args){
	size_t max = 100;
	char *buf = malloc(max);
	struct tm *time_info;
	time_t cur = time(NULL);
	if(args[0]!=NULL){
		if(strcmp(args[0],"-u")==0){
			time_info = gmtime(&cur);
			strftime(buf, max, "%a %b %d %T UTC %Y\n", time_info);
		}else if(strcmp(args[0],"-R")==0){
			time_info = localtime(&cur);
			strftime(buf, max, "%a, %d %b %Y %T %z\n", time_info);
		}else if((strcmp(args[0],"-uR")==0) || (strcmp(args[0],"-Ru")==0)){
			time_info = gmtime(&cur);
			strftime(buf, max, "%a, %d %b %Y %T %z\n", time_info);
		}else if(args[0][0]=='-'){
			fprintf(stderr, "date: invalid option -- \'%c\'\n", args[0][1]);
			return -1;
		}else{			
			fprintf(stderr, "date: invalid date \'%s\'\n", args[0]);
			return -1;
		}
	}else{
		time_info = localtime(&cur);
		strftime(buf, max, "%a %b %d %T %Z %Y\n", time_info);		
	}
	printf("%s",buf);
	free(buf);
	return 0;
}

int main(int argc, char **argv){
	date(argv+1);
}

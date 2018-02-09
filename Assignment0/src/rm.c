#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


int rm(char **args);


int rm(char **args){
	int verbose = 0;
	int deleteDir = 0;
	int counter = 0;

	if(args[0]==NULL){
		fprintf(stderr, "rm: missing operand\n");
		return -1;
	}
	if(args[0][0]=='-'){
		if(strcmp(args[0],"-v")==0){
			verbose = 1;

		}else if(strcmp(args[0],"-d")==0){
			deleteDir = 1;

		}else if(strcmp(args[0],"-dv")==0  || strcmp(args[0],"-vd")==0){
			verbose = 1;
			deleteDir = 1;

		}else{
			fprintf(stderr, "rm: invalid option -- \'%c\'\n", args[0][1]);
			fprintf(stderr,"%s\n","rm: usage: rm [-vd] FILE...");

			return -1;
		}
		counter++;
	}

	while(args[counter]!=NULL){	
		if(!deleteDir){
			if(unlink(args[counter])==-1){
				if(errno == EACCES){
					fprintf(stderr, "rm: access denied\n");
				}else if(errno == ENOENT){
					fprintf(stderr, "rm: cannot remove \'%s\': file name doesn't exist\n", args[counter]);
				}else if(errno == EISDIR){
					fprintf(stderr, "rm: cannot remove \'%s\': Is a directory\n", args[counter]);
				}else{
					fprintf(stderr, "rm: unexpected error, errno %d\n",errno);
				}
			}else{
				if(verbose){
					printf("removed \'%s\'\n", args[counter]);
				}
			}
		}else{
			if(remove(args[counter]) == -1){	// else use unlink and then rmdir if error occurs				
				if(errno == EACCES){
					fprintf(stderr, "rm: access denied\n");
				}else if(errno == EEXIST || errno == ENOTEMPTY){
					fprintf(stderr, "rm: cannot remove \'%s\': Directory not empty\n", args[counter]);
				}else if(errno == ENOENT){
					fprintf(stderr, "rm: cannot remove \'%s\': non existent directory\n", args[counter]);
				}else{
					fprintf(stderr, "rm: unexpected error, errno %d\n",errno);
				}
			}else{
				if(verbose){
					printf("removed \'%s\'\n", args[counter]);
				}
			}
		}
		counter++;
	}
}



int main(int argc, char **argv){
	rm(argv+1);
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>


int mk_dir(char **args);


int mk_dir(char **args){
	int verbose = 0;
	int parents = 0;
	int counter = 0;

	if(args[0]==NULL){
		fprintf(stderr, "mkdir: missing operand\n");
		return -1;
	}
	if(args[0][0]=='-'){
		if(strcmp(args[0],"-v")==0){
			verbose = 1;

		}else if(strcmp(args[0],"-p")==0){
			parents = 1;

		}else if(strcmp(args[0],"-pv")==0  || strcmp(args[0],"-vp")==0){
			verbose = 1;
			parents = 1;

		}else{
			fprintf(stderr, "mkdir: invalid option -- \'%c\'\n", args[0][1]);
			fprintf(stderr,"%s\n","mkdir: usage: mkdir [-vp] DIRECTORY...");

			return -1;
		}
		counter++;
	}

	while(args[counter]!=NULL){	
		if(mkdir(args[counter], 0775)==-1){
			if(errno == EEXIST){
				if(!parents){
					fprintf(stderr, "mkdir: cannot create directory\'%s\': File exists\n", args[counter]);
				}
			}else{
				fprintf(stderr, "mkdir: unexpected error, errno %d\n", errno);
			}
		}else{
			if(verbose){
				printf("mkdir: created directory \'%s\'\n", args[counter]);
			}
		}
		counter++;
	}
}



int main(int argc, char **argv){
	mk_dir(argv+1);
}

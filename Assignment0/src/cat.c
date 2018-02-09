#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int cat(char **args);


int cat(char **args){
	int printNum = 0;
	int printEnd = 0;
	int counter = 0;
	int lineCounter = 0;
	if(args[0]==NULL){
		fprintf(stderr, "cat: missing operand\n");
		return -1;
	}

	if(args[0][0]=='-'){
		if(strcmp(args[0],"-n")==0){
			printNum = 1;

		}else if(strcmp(args[0],"-E")==0){
			printEnd = 1;

		}else if(strcmp(args[0],"-nE")==0  || strcmp(args[0],"-En")==0){
			printNum = 1;
			printEnd = 1;

		}else{
			fprintf(stderr, "cat: invalid option -- \'%c\'\n", args[0][1]);
			fprintf(stderr,"%s\n","cat: usage: cat [-nE] FILE...");

			return -1;
		}
		counter++;
	}
	if(printNum){
		lineCounter = 1;
	}
	while(args[counter]!=NULL){			
		FILE *fileptr = fopen(args[counter], "r");			//this is slow
		if(fileptr == NULL){
			fprintf(stderr, "cat: %s: No such file or directory\n", args[counter++]);
			continue;
		}	

		char c;
		c = fgetc(fileptr);
		if(c!=EOF){
			if(printNum){
				printf("%6d ", lineCounter++);
			}
		}
		while(c!=EOF){
			if(c=='\n'){
				if(printEnd){
					printf("$");
				}
				printf("\n");
				if(printNum){
					c = fgetc(fileptr);
					if(c!=EOF){
						printf("%6d ", lineCounter++);
						continue;
					}else{
						break;
					}
				}
				c = fgetc(fileptr);
				continue;
			}
			printf("%c", c);
			c = fgetc(fileptr);

		}
		fclose(fileptr);
		counter++;
	}
}



int main(int argc, char **argv){
	cat(argv+1);
}

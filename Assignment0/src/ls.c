#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

int ls(int argc, char **args);
// used man scandir
int ls(int argc, char **args){
	int showDot = 0;
	int appendSlash = 0;
	int counter = 0;
	if(argc !=0 && args[0][0]=='-'){
		argc--;
		if(strcmp(args[0],"-a")==0){
			showDot = 1;

		}else if(strcmp(args[0],"-p")==0){
			appendSlash = 1;

		}else if(strcmp(args[0],"-pa")==0  || strcmp(args[0],"-ap")==0){
			showDot = 1;
			appendSlash = 1;

		}else{
			fprintf(stderr, "ls: invalid option -- \'%c\'\n", args[0][1]);
			fprintf(stderr,"%s\n","ls: usage: ls [-ap] [FILE]...");
			return -1;
		}
		counter++;
	}
	struct dirent **files;
	int numFiles = 0;
	if(argc == 0){
		numFiles = scandir(".", &files, NULL, alphasort);
	 	if(numFiles != -1){
			int index = 0;
			while(index<numFiles){
				if(!showDot){
					if(files[index]->d_name[0]=='.'){
						free(files[index++]);
						continue;
					}
				}
				printf("%s", files[index]->d_name);
				if(appendSlash){
					DIR *tmp = opendir(files[index]->d_name);
					if(tmp==NULL){
						if(errno!=ENOTDIR){	
							printf("/");
						}
					}else{
						printf("/");
					}
					closedir(tmp);
				}
				printf("  ");
				free(files[index++]);
			}
		}else{
				fprintf(stderr, "ls: some error occurred, errno %d\n",errno);
		}
	}else{
		int empty;
		while(args[counter]!=NULL){				
			numFiles = scandir(args[counter], &files, NULL, alphasort);
		 	if(numFiles != -1){
			 	if(argc!=1){
					printf("%s:\n", args[counter]);
				}
				empty = 1;
				int index = 0;
				while(index<numFiles){
					if(!showDot){
						if(files[index]->d_name[0]=='.'){
							free(files[index++]);
							continue;
						}
					}
					printf("%s", files[index]->d_name);
					if(appendSlash){
						DIR *tmp = opendir(files[index]->d_name);
						if(tmp==NULL){
							if(errno!=ENOTDIR){	
								printf("/");
							}
						}else{
							printf("/");
						}
						closedir(tmp);
					}
					printf("  ");
					free(files[index++]);
				}
			}else{
				if(errno == EACCES){
					fprintf(stderr, "ls: cannot access \'%s\': access denied\n", args[counter]);
				}else if(errno == ENOENT){
					fprintf(stderr, "ls: cannot access \'%s\': No such file or directory\n", args[counter]);
				}else{
					fprintf(stderr, "ls: some error occurred, errno %d\n",errno);
				}
			}
			if(!empty){
				printf("\n\n");
			}
			counter++;
		}
	}
	printf("\n");
}



int main(int argc, char **argv){
	ls(argc-1, argv+1);
}

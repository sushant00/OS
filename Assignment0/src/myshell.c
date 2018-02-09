#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <inttypes.h>

// #include "builtin.h"

//helps implementing history
char *ROOT;
char *HISTORY_FILE;
char *OLDPWD;
int NUM_FILE_HISTORY;
int NUM_CUR_SESS_HIST;
char *LAST_COMMAND;
char **CUR_SESSION_HIST;

const char ECHO_ESCAPES[2][10] = {{'a','b','e','E','f','n','r','t','v','\\'},
								{'\a','\b','\e','\e','\f','\n','\r','\t','\v','\\'}};
const char *EXTERN_COMMANDS[5] = {"ls", "cat", "date", "rm", "mkdir"};
const char *delim = " \n\r\t";//space, carriage return , newline, tab
//FILE *fileptr;

size_t bufsize = 100;		//assume max line length = 100 chars
char *line;
int MAX_TOKENS = 50;		//assume max tokens = 50
char **tokens;


char *readCommand();
char **parseCommand(char * command);
int exec_internal(char **args);
int exec_external(char **args);

int addToHistory(char *historyItem);
int writeHist(void);
int cd(char **args);
int echo(char **args);
int exitShell(char **args);
int pwd(char **args);
int help(char **args);
int history(char **args);

int main(){
	char **args;
	char *command;

	line = malloc(sizeof(char)*bufsize);
	tokens = malloc(sizeof(char *)*MAX_TOKENS);


	char *user = getenv("USER");
	char *host = malloc(sizeof(char)*50);
	gethostname(host,50);
	
	NUM_CUR_SESS_HIST = 0;
	CUR_SESSION_HIST = malloc(sizeof(char *)*100);

	//location of history file
	HISTORY_FILE = malloc(sizeof(char)*bufsize);
	ROOT = malloc(sizeof(char)*bufsize);
	char *cwd = malloc(sizeof(char)*bufsize);
	strcpy(cwd, getenv("PWD"));		//corner case of long cwd
	strcpy(ROOT,cwd);
	strcat(ROOT, "/");
	strcpy(HISTORY_FILE,ROOT);
	strcat(HISTORY_FILE,"shell_history.txt");

	while(1){
		printf("%s@%s:%s$ ",user, host, strcpy(cwd, getenv("PWD")));
		command = readCommand();
		args = parseCommand(command);
		if(args[0] == NULL){
			continue;
		}

		addToHistory(command);

		int executed = exec_internal(args);	//check and execute if internal command

		if(!executed){						//check and execute if external command
			executed = exec_external(args);
		}
	}
	return 0;
}


char *readCommand(){
	getline(&line, &bufsize,stdin);
	if(bufsize>100){
		fprintf(stderr, "shell: input line is too long\n");
		bufsize = 100;
	}
	return line;
}


char **parseCommand(char * command){
	int index = 0;
	char *next = strtok(command, delim);

	while(next!=NULL && index<50){
		tokens[index++] = next;
		next = strtok(NULL, delim);
	}
	tokens[index] = 0;//end of args
	return tokens;
}



int exec_internal(char **args) {
	char *command = args[0];

	if(strcmp(command,"cd")==0){		
		cd(args+1);
		return 1;

	}
	else if(strcmp(command,"echo")==0){	
		echo(args+1);
		return 1;
	}
	else if(strcmp(command,"exit")==0){
		exitShell(args+1);
		return 1;
	}
	else if(strcmp(command,"help")==0){
		help(args+1);
		return 1;
	}
	else if(strcmp(command,"history")==0){
		history(args+1);
		return 1;
	}
	else if(strcmp(command,"pwd")==0){
		pwd(args+1);
		return 1;
	}
	return 0;				// not a internal command
}

int exec_external(char **args){
	int pid = fork();
	if(pid==0){
		//successfully forked: child process
		int counter = 0;
		int supported = 0;
		while(counter < 5){
			if(strcmp(args[0], EXTERN_COMMANDS[counter++])==0){
				supported = 1;
				char *name = malloc(sizeof(char)*(strlen(args[0])+strlen(ROOT)));
				strcpy(name, ROOT);
				strcat(name, args[0]);
				execvp(name,args);
			}
		}
		if(!supported){
			fprintf(stderr, "shell: Command \'%s\' not supported\n", args[0]);
		}
		exit(EXIT_SUCCESS);

	}else if(pid==-1){
		//error forking
		fprintf(stderr,"error forking. child process not created");

	}else{
		//successfully forked: parent process
		wait(0);
	}
	return 0;
}







int addToHistory(char *historyItem){
	if(LAST_COMMAND==NULL){
		FILE *fileptr = fopen(HISTORY_FILE, "r");			//this is slow
		if(!fileptr){
			fileptr = fopen(HISTORY_FILE, "w+");
			fclose(fileptr);
			fileptr = fopen(HISTORY_FILE, "r");
		}
		char *line = malloc(sizeof(char)*bufsize);
		NUM_FILE_HISTORY = 0;
		while(getline(&line, &bufsize, fileptr)!=-1){  //read until end of file
			NUM_FILE_HISTORY++;
			continue;
		}

		int commlen = strlen(line);
		LAST_COMMAND = malloc(sizeof(char)*commlen);
		memset(LAST_COMMAND, '\0', sizeof(char)*commlen);
		strcpy(LAST_COMMAND, line);
		free(line);
		fclose(fileptr);
	}
	int commlen = strlen(historyItem);
	if(strcmp(LAST_COMMAND, historyItem)==0){
		return 0;
	}

	// add history element
	CUR_SESSION_HIST[NUM_CUR_SESS_HIST] = calloc(100, sizeof(char));
	strcpy(CUR_SESSION_HIST[NUM_CUR_SESS_HIST], historyItem);
	free(LAST_COMMAND);
	LAST_COMMAND = malloc(sizeof(char)*commlen);
	strncpy(LAST_COMMAND, historyItem, commlen);
	NUM_CUR_SESS_HIST++;

	return 0;
}

int writeHist(void){
	FILE *fileptr = fopen(HISTORY_FILE, "a");			//this is slow
	int tmpCounter = 0;
	while(tmpCounter<NUM_CUR_SESS_HIST){
			fprintf(fileptr, "%s\n", CUR_SESSION_HIST[tmpCounter++]);
	}
	fclose(fileptr);

}


int cd(char **args){
	//assumption: not changing PWD, else use setenv() to even set the PWD
	//-L -P ~ - cd NUll
	int P_enabled = 0;
	int L_enabled = 1;
	int hasDotDot = 0;

	// store cur pwd as previous pwd

	if(args[0]!=NULL){
		if(strcmp(args[0],"-P")==0){
			args = args+1;
			P_enabled = 1;
		}else if(strcmp(args[0],"-L")==0){
			args = args+1;
			L_enabled = 1;
		}
		else if(args[0][0]=='-' && args[0][1]=='\0'){
			if(OLDPWD==NULL){
				fprintf(stderr, "cd: OLDPWD not set\n");
				return -1;
			}	
			char *tmp = malloc(sizeof(char)*bufsize);
			strcpy(tmp, getenv("PWD"));
				
			printf("%s\n", OLDPWD);
			chdir(OLDPWD);
			setenv("PWD", OLDPWD, 1);

			free(OLDPWD);
			OLDPWD = malloc(sizeof(char)*bufsize);
			strcpy(OLDPWD, tmp);
			free(tmp);
			return 0;
		}
		else if(args[0][0]=='-'){
			fprintf(stderr,"shell: cd: %s: invalid option\n", args[0]);
			fprintf(stderr,"%s\n","cd: usage: cd [-P] [-L] [DIR]");
			return -1;
		}
	}
	char *address = malloc(sizeof(char)*bufsize);
	if(args[0]==NULL){
		strcpy(address, getenv("HOME"));
	}else{
		if(args[0][0]=='~'){
			strcpy(address, getenv("HOME"));
			strcat(address, &args[0][1]);

		}else if(args[0][0]=='.' && args[0][1]!='.'){
			strcpy(address, getenv("PWD"));
			strcat(address, &args[0][1]);
		}else if(args[0][0]!='/'){
			strcpy(address, getenv("PWD"));
			strcat(address, "/");
			strcat(address, args[0]);
		}else{
			strcpy(address, args[0]);
		}
		if(strstr(args[0], "..")!=NULL){
			hasDotDot = 1;
		}
	}
	if(OLDPWD!=NULL){
		free(OLDPWD);
	}
	OLDPWD = malloc(sizeof(char)*bufsize);
	strcpy(OLDPWD, getenv("PWD"));

	int status;
	if(hasDotDot){
		status = chdir(args[0]);
	}else{
		chdir(address);
	}
	if(status == -1){		
		if(errno == EACCES){
			fprintf(stderr, "cd: cannot access \'%s\': access denied\n", args[0]);
		}else if(errno == ENOENT){
			fprintf(stderr, "shell: cd: %s: No such file or directory\n", args[0]);
		}else if(errno == ENOTDIR){
			fprintf(stderr, "cd: \'%s\': Not a directory\n", args[0]);
		}else{
			fprintf(stderr, "cd: some error occurred, errno %d\n",errno);
		}
	}else{
		if(hasDotDot){
			//handle overflow
			free(address);
			char *address = malloc(sizeof(char)*bufsize);
			getcwd(address, bufsize);
			setenv("PWD", address, 1);
		}else{
			if(L_enabled){
				setenv("PWD", address, 1);
			}else if(P_enabled){
				free(address);
				char *address = malloc(sizeof(char)*bufsize);
				getcwd(address, bufsize);
				setenv("PWD", address, 1);
			}
		}
		free(address);
		return 0;
	}	
}

int echo(char **args){
	//handle back slashes and quotes
	int counter = 0;
	int printNewLine = 1;
	int suppBackSlash = 1;
	if(args[0]!=NULL){
		if(strcmp(args[0],"-n")==0){
			args = args+1;
			printNewLine = 0;

		}else if(strcmp(args[0],"-e")==0){
			args = args+1;
			suppBackSlash = 0;
		}else if(strcmp(args[0],"-E")==0){
			args = args+1;
			suppBackSlash = 1;
		}
		// else if(args[0][0]==){
		// 	fprintf(stderr,"shell: echo: %s: invalid option\n", args[0]);
		// 	fprintf(stderr,"%s\n","echo: usage: echo [-neE]");
		// }
	}
	//get length
	int len = 0;
	while(args[counter]!=NULL){
		len+=strlen(args[counter++]);
	}
	char *toPrint = malloc(sizeof(char)*(++len));
	len = 0;		//length of toPrint
	counter = 0;		//restart the ptr
	int dubQuote = 0;	//within doublequote or not
	int sinQuote = 0;	//within singlequote or not

	while(args[counter]!=NULL){
		if(counter>0){
			toPrint[len++] = ' ';
		}
		int tmp = 0;
		while(args[counter][tmp]!='\0'){
			if(args[counter][tmp]=='\''){
				if(!dubQuote){
					sinQuote = (sinQuote+1)%2;
					tmp++;
					continue;
				}else{
					toPrint[len++] = args[counter][tmp++];
					continue;
				}
			}
			if(args[counter][tmp]=='"'){
				if(!sinQuote){
					dubQuote = (dubQuote+1)%2;
					tmp++;
					continue;
				}else{
					toPrint[len++] = args[counter][tmp++];
					continue;
				}
			}
			if(args[counter][tmp]=='\\'){
				if(!suppBackSlash){
					tmp++;
					if(!(sinQuote || dubQuote)){
						if(!args[counter][tmp]=='\\'){
							toPrint[len++] = args[counter][tmp++];
							continue;
						}else{
							tmp++;
						}
					}
					int index = 0;
					int matched = 0;
					while(index<10){
						if(args[counter][tmp]==ECHO_ESCAPES[0][index]){
							matched = 1;
							toPrint[len++] = ECHO_ESCAPES[1][index];
							break;
						}
						index++;
					}
					if(!matched){
						if(args[counter][tmp]=='c'){
							printf("%s", toPrint);
							free(toPrint);
							return 0;
						}else if(args[counter][tmp]=='0'){
							char ch[4];
							ch[0] = args[counter][++tmp];
							ch[1] = args[counter][++tmp];
							ch[2] = args[counter][++tmp];
							ch[3] = '\0';
							toPrint[len++] = (char)strtoimax(ch, NULL, 8);

						}else if(args[counter][tmp]=='x'){
							char ch[3];
							ch[0] = args[counter][++tmp];
							ch[1] = args[counter][++tmp];
							ch[2] = '\0';
							toPrint[len++] = (char)strtoimax(ch, NULL, 16);
						}
						else{
							toPrint[len++] = '\\';
							toPrint[len++] = args[counter][tmp];
						}
					}

				}
			}else{
				toPrint[len++] = args[counter][tmp];
			}
			tmp++;
		}
		counter++;
	}
	if(printNewLine){
		toPrint[len++] = '\n';
	}
	toPrint[len]='\0';
	if(sinQuote || dubQuote){
		fprintf(stderr, "echo: unbalanced quotes\n");
	}else{
		printf("%s", toPrint);
	}
	

	free(toPrint);
	return EXIT_SUCCESS;
}

int exitShell(char **args){
	// if(fileptr!=NULL){
	// 	fclose(fileptr);
	// }
	writeHist();
	if(!args[0]){
		exit(errno);					// exit with last error no.
	}else{		
		int n = atoi(args[0]);
		exit(n);
	}
}


int help(char **patterns){
	//if(pattern)
	//printf("help: no help topics match '%s'.Try 'man %s'",pattern, pattern);
	//	return 1;
	return 0;
}


int history(char **args){
	FILE *fileptr = fopen(HISTORY_FILE, "r");			//this is slow
	char *line = NULL;
	size_t lineSize = 0;
	int histCounter = 1;
	if(args[0]!=NULL){
		if(strcmp(args[0], "-c")==0 || strcmp(args[0], "-ac")==0 || strcmp(args[0], "-ca")==0){
			free(CUR_SESSION_HIST);
			CUR_SESSION_HIST = malloc(sizeof(char *)*100);
			NUM_CUR_SESS_HIST = 0;
			LAST_COMMAND = NULL;
			return 0;
		}else if(strcmp(args[0], "-a")==0){
			writeHist();
			return 0;
		}
		int counter = 0;
		while(args[0][counter]!='\0'){
			if(!isdigit(args[0][counter++])){
				fprintf(stderr, "shell: history: %s: numeric argument required\n",args[0]);
				return -1;
			}
		}
		int num2Print = atoi(args[0]);
		int numFileHist2print = 0;
		if(num2Print > NUM_CUR_SESS_HIST){
			numFileHist2print = num2Print - NUM_CUR_SESS_HIST;
		}
		int histTop = NUM_FILE_HISTORY - numFileHist2print;
		if(histTop<0){
			histTop = 0;
		}
		while(histCounter<=histTop && getline(&line, &lineSize, fileptr)!=-1){  //read until end of file
			histCounter++;
		}	
		while(getline(&line, &lineSize, fileptr)!=-1){
			printf("%6d %s", histCounter++, line);
			num2Print--;
		}
		int i = NUM_CUR_SESS_HIST - num2Print;
		if(i<0){
			i = 0;
		}
		while(i<NUM_CUR_SESS_HIST){
			printf("%6d %s\n", histCounter++, CUR_SESSION_HIST[i++]);
		}


	}else{		
		while(getline(&line, &lineSize, fileptr)!=-1){  //read until end of file
			printf("%6d %s", histCounter++, line);
		}
		int i;
		for(i = 0; i<NUM_CUR_SESS_HIST; i++){
			printf("%6d %s\n", histCounter++, CUR_SESSION_HIST[i]);

		}
	}
	fclose(fileptr);
	free(line);
	return 0;
}



int pwd(char **args){
	// support -L, -P
	char *cwd = getenv("PWD");			// get the environment variable pwd
	if(cwd!=0){
		if(!args[0] || strcmp(args[0],"-L")==0){
			printf("%s\n",cwd);

		}else if(strcmp(args[0],"-P")==0){			
			char *buf = malloc(bufsize*sizeof(char));
			memset(buf, 0, bufsize*sizeof(char));

			int numBytes = readlink(cwd, buf, bufsize);	

			if(numBytes==-1){				//error
				if(errno == EINVAL){
					//fprintf(stderr, "%s\n", "named file is not a symbolic link");
					printf("%s\n", cwd);

				}else if(errno == ENAMETOOLONG){
					fprintf(stderr, "%s\n", "pathname or some component of pathname is too long");
				}

			}else if(numBytes==bufsize){	// buffer was small so reallocate buffer
				bufsize = readlink(cwd, buf, 0);
				buf = realloc(buf,bufsize);
				readlink(cwd, buf, bufsize);
				char *cntr = cwd;
				char *tmp = strrchr(cwd,'/');
				while(cntr!=tmp){
					printf("%c", *cntr++);
				}
				printf("/");	
				printf("%s\n",buf);

			}else{							//success	
				char *cntr = cwd;
				char *tmp = strrchr(cwd,'/');
				while(cntr!=tmp){
					printf("%c", *cntr++);
				}			
				printf("/");	
				printf("%s\n",buf);
			}
			free(buf);
		}else{
			//error with arguments
			fprintf(stderr,"shell: pwd: %s: invalid option\n", args[0]);
			fprintf(stderr,"%s\n","pwd: usage: pwd [-LP]");
			return -1;
		}
		return 0;	//return 0 unless invalid option or current dir cant be read
	}else{
		fprintf(stderr, "%s\n","pwd: error getting $PWD");
		return -1;
	}
}
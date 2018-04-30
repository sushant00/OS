#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	FILE *msg = fopen(argv[1], "r");
	if(msg == 0){
		printf("error opening %s, errorcode: %d\n", argv[1], errno);
		return -1;
	}
	int fd = open("/dev/encdev", O_RDWR);
	if(fd < 0){
		printf("error opening encdev\n");
		return -1;
	}
	//write the random key
	int fd1 = open("/dev/urandom", O_RDONLY);
	if(fd1 < 0){
		printf("error opening /dev/urandom\n");
		return -1;
	}
	char key[16];
	int r = read(fd1, key, 16);
	if(r<0){
		printf("error reading key from /dev/urandom\n");	
		return -1;
	}
	
	
	r = write(fd, key, 16);
	if(r<0){
		printf("error writing key\n");
		return -1;
	}

	int c, count=0;
	char buffer[16];
	while( (c = fgetc(msg)) != EOF ){
		buffer[count++] = (char)c;
		if(count==16){
			r = write(fd, buffer, 16);
			if(r<0){
				printf("write unsuccessful\n");
			}
			count = 0;
		}
	}
	buffer[count] = '\0';
	r = write(fd, buffer, 16);
	if(r<0){
		printf("write unsuccessful\n");
	}
	fclose(msg);
	msg = fopen("key.txt", "w");
	if(msg == 0){
		return -1;
	}
	fprintf(msg, "%s", key);	
	fclose(msg);
	msg = fopen("encrypted.txt", "w" );
	if(msg==0){
		return -1;
	}
	
	while(read(fd, buffer, 16)!=0){	
		fprintf(msg,"%s", buffer);	
		//printf("%s\n",cipher);
	}
	return 0;
}

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[]){
	FILE *msg = fopen(argv[1], "r");
	if(msg == 0){
		printf("error opening %s, errorcode: %d\n", argv[1], errno);
		return -1;
	}
	int fd = open("/dev/decryptor", O_RDWR);
	if(fd < 0){
		printf("error opening decryptor\n");
		return -1;
	}
	char key[16];
	int fd1 = open("key.txt", O_RDONLY);
	int r = read(fd1, key, 16);
	if(r<0){
		printf("error reading key from key.txt\n");	
		perror("Error printed by perror");
		return -1;
	}
	
	
	r = write(fd, key, 16);
	if(r<0){
		printf("error writing key to decryptor\n");
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
	printf("Reached\n");
	buffer[count] = '\0';
	r = write(fd, buffer, 16);
	if(r<0){
		printf("write unsuccessful\n");
	}
	fclose(msg);
	msg = fopen("decrypted.txt", "w" );
	if(msg==0){
		return -1;
	}
	while(read(fd, buffer, 16)!=-1){	
		fprintf(msg,"%s", buffer);	
	}
	fclose(msg);
	close(fd);
	return 0;
}
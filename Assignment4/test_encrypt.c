#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
//#include <sys/random.h>

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
	FILE *fd1 = fopen("/dev/urandom", "r");
	if(fd1 < 0){
		printf("error opening /dev/urandom\n");
		return -1;
	}
	char key[17];
	int r = fread(key, 1, 16, fd1);
	if(r<0){
		printf("error reading key from /dev/urandom\n");	
		return -1;
	}
	printf("size of key %d\n", r);
	fclose(fd1);
//	int i =0;
//	for( i = 0; i<16; i++){
//		key[i] = '2';
//	}
	key[16] = '\0';
//	getrandom(key, 16, GRND_NONBLOCK);
	printf("random key:%s:\n", key);
	
	r = write(fd, key, 16);
	if(r<0){
		printf("error writing key\n");
		return -1;
	}

	int c, count=0;
	char buffer[17];
	while( (c = fgetc(msg)) != EOF ){
		printf("read character %c, %d\n",(char)c, (char)c );
		buffer[count++] = (char)c;
		if(count==16){
			buffer[16] = '\0';
			r = write(fd, buffer, 16);
			if(r<0){
				printf("write unsuccessful\n");
			}else{
				printf("written:%s: \n", buffer);		
			}
			count = 0;
		}
	}
	buffer[count] = '\0';
	r = write(fd, buffer, 16);
	if(r<0){
		printf("write unsuccessful\n");
	}else{
		printf("written:%s:\n", buffer);		
	}
	fclose(msg);
	msg = fopen("key.txt", "w");
	if(msg == 0){
		return -1;
	}
	fwrite(key,1, 16,msg);	
	fclose(msg);
	msg = fopen("encrypted.txt", "w" );
	if(msg==0){
		printf("could not open encrypted.txt\n");
		return -1;
	}
	
	while(read(fd, buffer, 16)!=0){	
		fwrite(buffer, 1, 16, msg);	
		printf("%s\n",buffer);
	}
	printf("done\n");
	fclose(msg);
	close(fd);
	return 0;
}

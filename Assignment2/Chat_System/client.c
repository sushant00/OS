#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
int main(void)
{
	int temp=0;
	int *myexit=&temp;

	//Creating and connecting to server socket
	int s, t, len;
	struct sockaddr_in remote;
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
	perror("socket");
	exit(1);
	}
	printf("Trying to connect...\n");
	remote.sin_family = PF_INET;
	remote.sin_port = (in_port_t)htons(30000);
	remote.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (connect(s, (struct sockaddr *)&remote, sizeof(remote)) == -1)
	{
		perror("Could not connect");
		exit(1);
	}

	printf("Connected to server. Enter user name\n");

	//Entering username
	int flag=0;
	while(flag==0)
	{
		char name[100];
		scanf("%s",name);
		send(s,name,strlen(name),0);
		char str[100];
	 	int t=recv(s, str, 100, 0);
		if (t > 0)
		{
			str[t] = '\0';
			if(strcmp(str,"Registered")==0)
			{	
				printf("Registered\n");
				flag=1;
			}
			else if(strcmp(str,"No")==0)
				printf("Username already exists, please enter another user name\n");
			else if(strcmp(str,"Not allowed")==0)
				printf("All is reserved, please enter another username\n");
			else if(strcmp(str,"Full")==0)
			{
				printf("Chat room is full, better luck next time.\n");
				exit(1);
			}
		}
		else
		{
			if (t < 0) 
					perror("recv");
			else 
					printf("Server closed connection\n");
			exit(1);
		}
	}

	//To recieve messages from server
	if(!fork())
	{
		while(1)
		{
		 	char str[1025];
		 	//printf("%s\n","Ready to receive");
		 	int t=recv(s, str, 1024, 0);
		 	//printf("%s\n","received something");
			if (t > 0)
			{
				str[t] = '\0';
				printf("%s\n", str);
			}
			else 
			{
				if (t < 0) 
					perror("recv");
				else 
					printf("Server closed connection\n");
				*myexit=1;
				exit(1);
			}
		}
	}

	//To send messsages to server
	else
	{
		while(1)
		{
			char msg[1024];
			//scanf("%s",msg);
			//scanf("%[^\n]",msg);
			fgets(msg,1024,stdin);
			send(s,msg,strlen(msg),0);
			if(*myexit==1)
				exit(1);
		}
	}
	close(s);
	return 0;
}

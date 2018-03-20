#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
	int num_user=0;
	int max=10; // max clients allowed
	char names[max][20]; //names of clients
	int sockets[max]; // socket descriptors of clients
	for(int i=0;i<max;i++)
		sockets[i]=0;

	//Creating socket
	int listener_d = socket(PF_INET, SOCK_STREAM, 0);
	if(listener_d==-1)
		error("Cant open socket");
	int reuse = 1;
	if (setsockopt(listener_d, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1)
	 	error("Can't set the reuse option on the socket");
	struct sockaddr_in name;
	name.sin_family = PF_INET;
	name.sin_port = (in_port_t)htons(30000);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	//Binding socket
	int b=bind(listener_d, (struct sockaddr *) &name, sizeof(name));
	if(b==-1)
		error("Can't bind");
	int l=listen(listener_d, 10);
	if(l==-1)
		error("Can't listen");
	puts("Waiting for connection");

	//Just FD set things
	fd_set readfds;
	int max_sd;
	int activity;

	char buffer[1025]; // Message is recieved in this

	while(1)
	{
		//CLearing FD set
		FD_ZERO(&readfds);

		FD_SET(listener_d, &readfds);  
        max_sd = listener_d;

        //Adding valid socket descriptors to FD set  
        for (int i = 0 ; i < num_user ; i++)  
        {  
            //printf("Checkpoint2\n");
            int sd = sockets[i];  
            if(sd > 0)  
                FD_SET(sd,&readfds);  
            if(sd > max_sd)  
                max_sd = sd;  
        }  
 
        activity=select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
      	
      	//New connection on server socket
        if (FD_ISSET(listener_d, &readfds))  
        {  
			//printf("Checkpoint3\n");
			struct sockaddr_storage client_addr;
			unsigned int address_size = sizeof(client_addr);
			int connect_d = accept(listener_d, (struct sockaddr *)&client_addr, &address_size);  

            if (connect_d<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  
           	//printf("Checkpoint4\n");

           	//Accepting username of connected user
			int flag=0;
			while(flag==0)
			{
				flag=1;
				char name[100];
				int t=recv(connect_d,name,100,0);
				name[t]='\0';
				if(strcmp(name,"All")==0)
				{
					char *msg="Not allowed";
					send(connect_d,msg,strlen(msg),0);
					flag=0;
				}
				else if(num_user==max)
				{
					char *msg="Full";
					send(connect_d,msg,strlen(msg),0);
					flag=0;
				}
				else
				{
					//Checking if username is unique
					for(int i=0;i<num_user;i++)
					{
						if(strcmp(name,names[i])==0)
						{
							flag=0;
							break;
						}		
					}
					if(flag==0)
					{
				        char *msg="No";
						send(connect_d,msg,strlen(msg),0);
			        }

			        //Registering user if username is unique
			        else
			        {
						if(name[0]!='\0')
						{
							printf("%s connected\n",name);
							char *msg="Registered";
							send(connect_d,msg,strlen(msg),0);
				            strcpy(names[num_user],name);
				            sockets[num_user]=connect_d;
				            num_user++;
				        }
			            //printf("Checkpoint 1\n");
			        }
			    }
		    }
        }

        //If IO operation on some other socket  
        else 
        {
	        for (int i = 0;i<num_user; i++)  
	        {  
	            //printf("Checkpoint5\n");
	            int sd = sockets[i];  
	                
	            if (FD_ISSET( sd , &readfds))  
	            {
	                //printf("Checkpoint6\n");
	                int valread;
	                if ((valread = read(sd,buffer,1024)) == 0)  
	                {  
	                    buffer[valread]='\0';
	                    for(int j=0;j<num_user;j++)
	                    {
	                    	if(sockets[j]==sd)
	                    	{
	                    		printf("%s disconnected\n",names[j]);
	                    		break;
	                    	}
	                    }     
	                    close(sd);  
	                    sockets[i] = 0; 
	                    for(int k=i;k<num_user-1;k++)
	                    {
	                    	sockets[k]=sockets[k+1];
	                    	strcpy(names[k],names[k+1]);
	                    }	 
	                    sockets[num_user-1]=0;
	                    strcpy(names[num_user-1],"");
	                    num_user--;
	                }
	                else
	                {  
	                    //printf("Checkpoint7\n");
	                    buffer[valread-1] = '\0';
	                    char *colon=strchr(buffer,':');
	                    char from[1024];
	                    for(int j=0;j<num_user;j++)
	                    {
	                    	if(sockets[j]==sd)
	                    	{
	                    		strcpy(from,names[j]);
	                    		strcat(from,":");
	                    		break;
	                    	}
	                    }     
	                    if(colon!=NULL)
	                    {
	                    	char *msg=colon;
	                    	msg++;
	                    	*colon='\0';
	                    	strcat(from,msg);
		                    //printf("Checkpoint8\n");
		                    if(strcmp(buffer,"All")==0)
		                    {
		                    	for(int j=0;j<num_user;j++)
		                    		if(sockets[j]!=sd)
		                    			send(sockets[j],from, strlen(from) , 0 );
		                    }
		                    else
		                    {
		                    	int flag=0;
		                    	for(int j=0;j<num_user;j++)
		                    	{
		                    		if(strcmp(names[j],buffer)==0)
		                    		{
		                    			send(sockets[j],from, strlen(from) , 0 );
		                    			flag=1;
		                    			break;
		                    		}
		                    	}
		                    	if(flag==0)
		                    	{
		                    		char *msg="No such username exists";
		                    		send(sd,msg,strlen(msg),0);
		                    	}
		                    }
		                }
	                }  
	            }  
	        }  
	    }
	}
	return 0;
}
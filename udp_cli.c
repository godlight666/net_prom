#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>
#define MAXDATASIZE 256
#define N 300
#define PORT 12345
typedef struct msg{
	char type;
	char name[32];
	char text[MAXDATASIZE];
}MSG;

int main(int argc, const char *argv[])
{
	MSG msg;  
	int sockfd;  
	struct sockaddr_in server;  
	socklen_t addrlen = sizeof(struct sockaddr);  
	int maxfd;
	fd_set infds;
	pid_t pid;
    struct hostent* he;

    if (argc != 2)
	{
		printf("Usage: %s  <IP address>\n", argv[0]);
		exit(-1);	}
	if ((he = gethostbyname(argv[1])) == NULL) {
		perror("gethostbyname error.");
		exit(1);
	}

    //第一步：创建一个套接字
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){  
        	perror("fail to socket");  
	}
	memset(&server,0,sizeof(server));
	server.sin_family = AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr = *((struct in_addr *) he->h_addr);
    	
	printf("Please input your name:");
	fgets(msg.name,32,stdin);
	msg.name[strlen(msg.name)-1]='\0';
	msg.type='L';
	msg.text[0]='\0';
	
	if (sendto(sockfd,&msg,sizeof(msg),0,(struct sockaddr *)&server,addrlen)<0){
		perror("fail to sendto");
	}
	
	pid=fork();
	
	//father
	if(pid>0){
		while(1){
			recvfrom(sockfd, &msg, sizeof(msg), 0, (struct sockaddr *)&server, &addrlen);   
			puts(msg.text);
		}
	}
	//child
	else if(pid==0){
		while(1){
			fgets(msg.text,MAXDATASIZE,stdin);
			msg.text[strlen(msg.text)-1]='\0';
			if(strncmp(msg.text,"quit",4)==0){  
				msg.type='Q';  
				if(sendto(sockfd,&msg,sizeof(msg),0,(struct sockaddr *)&server,addrlen)<0){  
					perror("fail to sendto");  
				}   
     				kill(getppid(),SIGKILL);  
     				exit(1);  
 			}
			else{  
				msg.type='S';  
				if(sendto(sockfd,&msg,sizeof(msg),0,(struct sockaddr *)&server,addrlen)<0){  
					perror("fail to sendto");  
  				}
			}
		}
	}
	else{
		perror("fail to fork");
		exit(-1);
	}
	close(sockfd);
	return 0;
}

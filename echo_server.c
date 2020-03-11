#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 	1234
#define BACKLOG	1
#define MAX	100

int main(void)
{
	int 	listenfd, connectfd;
	struct sockaddr_in server , client;
	int	sin_size;
	ssize_t n;
	char    readbuf[MAX];
	if((listenfd=socket(AF_INET, SOCK_STREAM, 0))==-1)
	{
		perror("Create socket failed.");
		exit(-1);	
	}
	int opt = SO_REUSEADDR;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listenfd,(struct sockaddr *)&server,sizeof(struct sockaddr)) == -1){
		perror("Bind error.");
		exit(-1);
	}
	if (listen(listenfd, BACKLOG) == -1) {
		perror("listen error.");
		exit(-1);
	}
	sin_size = sizeof(struct sockaddr_in);
	while(1) {
		if ((connectfd = accept(listenfd, (struct sockaddr *)&client, &sin_size)) == -1) {
			perror("accept error.");
			exit(-1);
		}
		printf("You get a connection from %s\n", inet_ntoa(client.sin_addr));
		send(connectfd, "Welcome to my server.\n", 22, 0);		
		while((n = recv(connectfd,readbuf,MAX,0))>0){
			send(connectfd,readbuf,n,0);
		}
		if (n<0)
			perror("echo:read error");
		close(connectfd);
	} /* while */
	close(listenfd);
}


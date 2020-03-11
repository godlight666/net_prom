#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define PORT 		12345
#define MAXDATASIZE	100
typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;
typedef unsigned short sa_family_t;

int main(int argc, char *argv[])
{
	int fd, numbytes;
	char sendbuf[MAXDATASIZE],readbuf[MAXDATASIZE];
	struct hostent *he;
	struct sockaddr_in server;
	if (argc != 2)
	{
		printf("Usage: %s  <IP address>\n", argv[0]);
		exit(-1);	}
	if ((he = gethostbyname(argv[1])) == NULL) {
		perror("gethostbyname error.");
		exit(1);
	}

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Create socket failed.");
		exit(1);
	}

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr = *((struct in_addr *) he->h_addr);
	if (connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
		perror("connect failed.");
		exit(1);
	}
	if( ((numbytes = recv(fd, readbuf, MAXDATASIZE, 0)) == -1) ){
		perror("recv error.");
		exit(1);
	}
		readbuf[numbytes]= '\0';
	printf("Server Message:%s\n",readbuf);
	while(fgets(sendbuf,MAXDATASIZE,stdin)!=NULL){
		send(fd,sendbuf,strlen(sendbuf),0);
		if( ((numbytes = recv(fd, readbuf, MAXDATASIZE, 0)) == -1) ){
			perror("recv error.");
			exit(1);
		}
		readbuf[numbytes]= '\0';
		printf("%s\n",readbuf);
	}
	//while(fgets(sendbuf,MAXDATASIZE,stdin)!=NULL){}
	
	close(fd);
}



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 12345
#define BACKLOG 10
int main(int argc,char **argv){
    struct sockaddr_in server;
    struct hostent *he;
    int connfd;

    if (argc!=2){
        printf("Usage: ./client [address of server]");
        _exit(-1);
    }
    else{
        if ((he = gethostbyname(argv[1]))==NULL){
            perror("Get host failed!");
            _exit(-1);
        }
        
    }
    
    //socket
    if ((connfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("Create socket error");
        _exit(-1);
    }
    //set socket
    // int opt = SO_REUSEADDR;
    // setsockopt(connfd)
    //set sockaddr_in
    bzero(&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr = *((struct in_addr *) he->h_addr);
    printf("the ip is %s",inet_ntoa(server.sin_addr));
    //connect
    if(connect(connfd,(struct sockaddr *)&server,sizeof(struct sockaddr))==-1){
        perror("Connect error!");
        _exit(-1);
    }
    close(connfd);
}
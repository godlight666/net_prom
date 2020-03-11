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
    int connfd,numBytes;
    char readBuf[100],writeBuf[100];
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
    bzero(&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr = *((struct in_addr *) he->h_addr);
    //connect
    if(connect(connfd,(struct sockaddr *)&server,sizeof(struct sockaddr))==-1){
        perror("Connect error!");
        _exit(-1);
    }
    if ((numBytes = read(connfd,readBuf,100))==-1){
        perror("read error:");
        _exit(-1);
    }
    readBuf[numBytes] = '\0';
    printf("Server Message:%s\n",readBuf);
    while (1){
        fgets(writeBuf,100,stdin);
        write(connfd,writeBuf,strlen(writeBuf));
        if ((numBytes = read(connfd,readBuf,100))==-1){
            perror("read error");
            _exit(-1);
        }
        readBuf[numBytes] = '\0';
        puts(readBuf);
        //printf("%s\n",readBuf);
    }
    
    close(connfd);
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
//select I/O server
#define PORT 12345
#define BACKLOG 100
int main(){
    int listenfd,connfd,sockfd,maxfd,maxi,i,slen;
    int number,nready,cli[FD_SETSIZE];//该数组用来限制连接的客户端不超过FD_SETSIZE
    fd_set rset,allset;
    char readBuf[100],writeBuf[100];
    struct sockaddr_in server,client;
    
    if ((listenfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("Create socket failed");
        _exit(-1);
    }
    int opt = SO_REUSEADDR;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    
    bzero(&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if ((bind(listenfd,(struct sockaddr*)&server,sizeof(struct sockaddr)))==-1){
        perror("Bind Error!");
        _exit(-1);
    }
    if((listen(listenfd,BACKLOG))==-1){
        perror("Listen error");
        _exit(-1);
    }
    for(i = 0;i<FD_SETSIZE;i++){
        cli[i] = -1;
    }
    
    FD_ZERO(&allset);
    FD_SET(listenfd,&allset);
    maxfd = listenfd;
    maxi = -1;
    while(1){
        rset = allset;
        //memcpy(&rset,&allset,sizeof(allset));
        nready = select(maxfd+1,&rset,NULL,NULL,NULL);
        slen = sizeof(struct sockaddr_in);
        if(FD_ISSET(listenfd,&rset)){
            if ((connfd = accept(listenfd,(struct sockaddr*)&client,&slen))==-1){
                perror("Accept error");
                _exit(-1);
            }
            printf("You get a connection from %s\n",inet_ntoa(client.sin_addr));
            write(connfd,"Welcome to my server!}",22);
            for(i = 0;i<FD_SETSIZE;i++){
                if(cli[i]<0){
                    cli[i] = connfd;
                    break;
                }
            }
            if (i == FD_SETSIZE){
                perror("Too many clients");
                exit(-1);
            }
            FD_SET(connfd,&allset);
            if (connfd > maxfd)
                maxfd = connfd;
            
            if (i>maxi)
                maxi = i;
            if (--nready<=0)
                continue;
            
        }
        for(i = 0;i<=maxi;i++){
            if((sockfd = cli[i])<0){
                continue;
            }
            if (FD_ISSET(sockfd,&rset)){
                if ((number = read(sockfd,readBuf,100))==0){
                    close(sockfd);
                    FD_CLR(sockfd,&allset);
                    cli[i] = -1;
                }
                else{
                    readBuf[number] = '\0';
                    strcpy(writeBuf,"Server message: ");
                    strcat(writeBuf,readBuf);
                    write(sockfd,writeBuf,strlen(writeBuf));
                }
                if ((--nready)<=0){
                    break;
                }
            }
        }
        
    }
    close(listenfd);
}
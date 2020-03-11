#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>

#define PORT 	12345
#define BACKLOG	10
#define MAX	1000
struct ARG
{
    int conn;
};

void *thr_fun(void* arg){
    int conn;
    printf("hello world!%d",((struct ARG *)arg)->conn);
}
int main(){
    struct sockaddr_in server,client;
    int listenfd,connfd;
    int sin_size;
    //listen socket
    if ((listenfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("Create socket error\n");
        _exit(-1);
    }
    //set the socket to use the address used
    int opt = SO_REUSEADDR;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    //set the struct bound by listen socket
    bzero(&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    //bind
    if (bind(listenfd,(struct sockaddr *)&server,sizeof(struct sockaddr))==-1){
        perror("Bind error!\n");
        _exit(-1);
    }
    //listen
    if (listen(listenfd,BACKLOG)==-1){
        perror("Listen error!\n");
        _exit(-1);
    }
    
    sin_size = sizeof(struct sockaddr_in);
    
    //while
    while(1){
        //accept--connect socket
        if ((connfd = accept(listenfd,(struct sockaddr*)&client,&sin_size))==-1){
            perror("Accept error\n");
            _exit(-1);
        }
        
        printf("Get connect from %s",inet_ntoa(client.sin_addr));
        sleep(10);
        printf("\nhello world\n");
        //sleep(10);
        //process--thread
        close(connfd);
    }
    //close
    close(listenfd);
}
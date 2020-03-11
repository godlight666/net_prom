#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <sys/resource.h>
#define PORT 12345
#define BACKLOG 10
#define MAXPOLLSIZE 8000
int non_block(int sockfd){
    if(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1){
        return -1;
    }
    return 0;
}

int process(int connfd){
    int numBytes;
    char readBuf[100],writeBuf[100];
    return -1;
    numBytes = read(connfd,readBuf,100);
    if (numBytes == 0){
        printf("The connection is closed!");
        close(connfd);
        return -1;
    }
    // else if (numBytes < 0){
    //     perror("Read Error!");
    //     close(connfd);
    //     return -1;
    // }
    readBuf[numBytes] = '\0';
    strcpy(writeBuf,"Server message:");
    strcat(writeBuf,readBuf);
    write(connfd,writeBuf,strlen(writeBuf));
    return 0;
}

int main(){
    int connfd,listenfd,epollfd,curfds,nfds,acceptcount = 0;
    struct sockaddr_in server,client;
    struct epoll_event event,events[MAXPOLLSIZE];
    struct rlimit rl;
    //设置每个进程能够最大打开的文件数
    rl.rlim_max = rl.rlim_cur = MAXPOLLSIZE;
    if (setrlimit(RLIMIT_NOFILE, &rl) == -1){
        perror("setrlimit error");
        return -1;
    }

    if ((listenfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("Create socket error!");
        exit(-1);
    }
    int opt = SO_REUSEADDR;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    if (non_block(listenfd)==-1){
        perror("Set non block error!");
        exit(-1);
    }
    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listenfd,(struct sockaddr *)&server,sizeof(struct sockaddr))==-1){
        perror("Bind error!");
        exit(-1);
    }
    if (listen(listenfd,BACKLOG)==-1){
        perror("Listen error!");
        exit(-1);
    }
    epollfd = epoll_create(MAXPOLLSIZE);
    event.data.fd = listenfd;
    event.events = EPOLLIN | EPOLLET;
    //将监听套接字加入epoll列表
    if ((epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event)) <0){
        perror("Epoll ctl failed!");
        exit(-1);
    }
    curfds = 1;
    while(1){
        //等待唤醒
        nfds = epoll_wait(epollfd,events,curfds,-1);
        if (nfds < 0){
            perror("epoll wait......");
            continue;
        }
        for(int n = 0;n < nfds;++n){
            if (events[n].data.fd == listenfd){
                int sin = sizeof(struct sockaddr_in);
                if ((connfd = accept(listenfd,(struct sockaddr*)&client,&sin))<0){
                    perror("Accept Error!");
                    continue;
                }
                printf("You get a connection from %s\n",inet_ntoa(client.sin_addr));
                
                acceptcount++;
                if (curfds >= MAXPOLLSIZE){
                    printf("Too many connection!");
                    close(connfd);
                    continue;
                }
                if (non_block(connfd)<0){
                    perror("Set non_block error!");
                }
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = connfd;
                if ((epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&event))<0){
                    printf("Epoll ctl error!");
                    return -1;
                }
                write(connfd,"Welcome to my server!}",22);
                curfds++;
                continue;
            }
            if (process(events[n].data.fd)<0){
                epoll_ctl(epollfd,EPOLL_CTL_DEL,connfd,&event);
                curfds--;
            }
        }
    }
    close(listenfd);
    close(epollfd);
}
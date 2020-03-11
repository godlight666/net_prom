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

void getTime(char *timeStr);
void *thr_fun(void* arg);
void process(int connfd,struct sockaddr_in client);
struct ARG
{
    int connfd;
    struct sockaddr_in client;
};
int main(){
    struct sockaddr_in server,client;
    struct ARG *arg;
    int listenfd,connfd;
    int sin_size;
    pthread_t tid;
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
        arg = (struct ARG *)malloc(sizeof(struct  ARG));
		arg -> connfd = connfd;
		memcpy((void *)&arg -> client, &client, sizeof(client));

        //printf("hell%s\n",inet_ntoa((arg->client).sin_addr));

        if (pthread_create(&tid,NULL,thr_fun,(void *)arg)){
            perror("Create thread failed!");
            exit(-1);
        }
        //printf("this is the thread %ld\n",tid);
        // if(pthread_join(tid,NULL)){
        //     perror("join error");
        //     abort();
        // }
        //printf("the thread end!\n");
    }
    //close
    close(listenfd);
}
void *thr_fun(void* arg){
    struct ARG *info;
    info = (struct ARG*)arg;
    process(info->connfd,info->client);
    free(arg);
    //pthread_exit(NULL);
}
void process(int connfd,struct sockaddr_in client){
    char readBuf[MAX],writeBuf[MAX];
    char timeStr[30];
    int numbytes;
    printf("You get connection from %s\n",inet_ntoa(client.sin_addr));
    //sleep(1);
    strcpy(writeBuf,"Welcome to my server!\nThe time is ");
	getTime(timeStr);
	strcat(writeBuf,timeStr);
    write(connfd,writeBuf,strlen(writeBuf));
    while(1){
        if ((numbytes = read(connfd,readBuf,MAX))==-1){
            perror("Read error!");
            _exit(-1);
        }
        readBuf[numbytes] = '\0';
        getTime(timeStr);
        strcpy(writeBuf,timeStr);
        strcat(writeBuf,readBuf);
        write(connfd,writeBuf,strlen(writeBuf));
        
    }
    close(connfd);
}
void getTime(char *timeStr){
	time_t tmpcal_ptr;
	struct tm *tmp_ptr = NULL;
	time(&tmpcal_ptr);
	tmp_ptr = localtime(&tmpcal_ptr);
	strftime(timeStr,128,"%F %T ",tmp_ptr);
}

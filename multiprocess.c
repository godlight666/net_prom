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

#define PORT 	12345
#define BACKLOG	10
#define MAX	1000

void getTime(char *timeStr);
void read_child_proc(int sig);

int main(){
	int listenfd,connectfd;
	struct sockaddr_in server,client;
	int sin_size;
	char readbuf[MAX],sendbuf[MAX];
	pid_t pid;
	ssize_t n;
	char timeStr[30];
	if ((listenfd = socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("Creat socket error:");
		exit(-1);
	}
	int opt = SO_REUSEADDR;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listenfd,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1){
		perror("bind error:");
		exit(-1);
	}
	if (listen(listenfd,BACKLOG)==-1){
		perror("listen error:");
		exit(-1);
	}
	sin_size = sizeof(struct sockaddr_in);
	for(;;){
		if ((connectfd = accept(listenfd,(struct sockaddr*)&client,&sin_size))==-1){
			perror("accept error:");
			exit(-1);
		}
		pid = fork();
		signal(SIGCHLD,read_child_proc);
		//father
		if (pid>0){
			close(connectfd);
			continue;
		}
		//son
		else if(pid==0){
			close(listenfd);
			printf("You get a connection from %s\n",inet_ntoa(client.sin_addr));
			strcpy(sendbuf,"Welcome to my server!\nThe time is ");
			getTime(timeStr);
			strcat(sendbuf,timeStr);
			send(connectfd,sendbuf,strlen(sendbuf),0);
			while ((n = recv(connectfd,readbuf,MAX,0))>0){
				readbuf[n] = '\0';
				getTime(timeStr);
				strcpy(sendbuf,timeStr);
				strcat(sendbuf," ");
				strcat(sendbuf,readbuf);
				send(connectfd,sendbuf,strlen(sendbuf),0);
			}
			if (n<0)
				perror("read error:");
			sleep(2);
			close(connectfd);
			_exit(1);
		}
		else
			printf("fork error.");
		close(listenfd);
	}
	return 0;
}
//get the local time and change to string
void getTime(char *timeStr){
	time_t tmpcal_ptr;
	struct tm *tmp_ptr = NULL;
	time(&tmpcal_ptr);
	tmp_ptr = localtime(&tmpcal_ptr);
	strftime(timeStr,128,"%F %T",tmp_ptr);
}
void read_child_proc(int sig){
	int status;
	pid_t pid = waitpid(-1,&status,WNOHANG);
	if (WIFEXITED(status))
		printf("child proc finish: %d %d\n",pid,WEXITSTATUS(status));
}


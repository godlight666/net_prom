#include <stdio.h>
#include <time.h>
int re();
int main(){
	time_t tmpcal_ptr;
	struct tm *tmp = NULL;
	char pcTime[100];
	time(&tmpcal_ptr);
	tmp = localtime(&tmpcal_ptr);
	strftime(pcTime,128,"%F %T",tmp);
	printf("the time is %s",pcTime);
}
int re(){
	return 5;
}

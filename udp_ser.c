//按照规则排序数组：规则为正数和负数交叉
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX 100
int main(){
	int big[MAX],small[MAX],output[MAX];
	int blen=0,slen=0,olen=0,b=0,s=0;
	char c;
	int j;
	
	//输入数组
	for (int i = 0;i < MAX;i++){
		scanf("%d",&j);
		c = getchar();
		if (j > 0 || j == 0)
			big[blen++] = j;
		else
			small[slen++] = j;
		if (c!=' '){
			break;
		}
	}
	if (blen>slen){
		for (int i = 0;i < MAX;i++){
			if (b<blen && s<slen){
				output[olen++] = big[b++];
				output[olen++] = small[s++]; 
			}
			else
			{
				break;
			}
		}
		for (;b<blen;b++){
			output[olen++]=big[b];
		}
	}
	else{
		for (int i = 0;i < MAX;i++){
			if (b<blen && s<slen){
				output[olen++] = small[s++];
				output[olen++] = big[b++];
			}
			else
			{
				break;
			}
		}
		for (;s<slen;s++){
			output[olen++]=small[s];
		}
	}
	//输出
	for (int i=0; i < olen; i++){
		printf("%d ",output[i]);
	}
}

/********************************************************************************
* @File name: prime2.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-15
* @Description: 计算100以内的素数
********************************************************************************/

#include <stdio.h>
#include "apilib.h"

#define MAX 10000

void Main () {
	int i, j;
	char *flag, s[8];
	apiInitMalloc();	// 应用程序栈中的变量若超过4KB则需要调用alloca
	flag = apiMalloc(MAX);	// 使用malloc暂时代替
	for (i = 0; i < MAX; i++) {
		flag[i] = 0;
	}
	for (i = 2; i < MAX; i++) {	// 素数筛
		if (flag[i] == 0) {
			sprintf(s, "%d ", i);
			apiPutstr0(s);
			for (j = i * 2; j < MAX; j+= i) {
				flag[j] = 1;
			}
		}
	}
	apiEnd();
}

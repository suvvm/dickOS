/********************************************************************************
* @File name: prime.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-15
* @Description: 计算1000以内的素数
********************************************************************************/

#include <stdio.h>
#include "apilib.h"

#define MAX 1000

void Main() {
	int i, j;
	char flag[MAX], s[8];
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

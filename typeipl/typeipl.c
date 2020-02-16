/********************************************************************************
* @File name: typeipl.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-15
* @Description: 显示ipl.nas文件内容
********************************************************************************/

#include "apilib.h"

void Main (void)
{
	int fileHandle;
	char c;
	fileHandle = apiFileOpen("ipl.nas");
	if (fileHandle != 0) {
		for (;;) {
			if (apiFileRead(&c, 1, fileHandle) == 0) {
				break;
			}
			apiPutchar(c);
		}
	}
	apiEnd();
}

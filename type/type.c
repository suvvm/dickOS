/********************************************************************************
* @File name: type.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-18
* @Description: c语言编写的应用程序 读取命令行指令以代替type指令
********************************************************************************/

#include "apilib.h"

void Main () {
	int fileHandle;	// 文件句柄
	char c, cmdline[30], *p;
	apiCmdline(cmdline, 30);
	for (p = cmdline; *p > ' '; p++) {}	// 跳过空格前的内容
	for (; *p == ' '; p++) {}; // 跳过所有空格
	fileHandle = apiFileOpen(p);	// 打开文件命为p指向字符串的程序
	if (fileHandle != 0) {
		for (;;) {
			if (apiFileRead(&c, 1, fileHandle) == 0) {
				break;
			}
			apiPutchar(c);
		}
	} else {
		apiPutstr0("File not found.\n");
	}
	// apiPutstr0("\nshow by type\n");
	apiEnd();
}

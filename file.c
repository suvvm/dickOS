#ifndef FILE_C
#define FILE_C
/********************************************************************************
* @File name: file.c
* @Author: suvvm
* @Version: 0.0.1
* @Date: 2020-02-06
* @Description: 实现文件读取相关函数
********************************************************************************/

#include "bootpack.h"

/*******************************************************
*
* Function name: readFat
* Description: 解压缩读取fat表存入内存
*	fat时经过压缩的，将两个扇区的信息压缩至了3个字节之中
* Parameter:
*	@fat	内存fat表记录指针	int *
*	@img	硬盘数据指针		unsigned char *
*
**********************************************************/
void readFat(int *fat, unsigned char *img) {
	int i, j = 0;
	for (i = 0; i < 2880; i += 2) {
		fat[i + 0] = (img[j + 0] | img[j + 1] << 8) & 0xfff;
		fat[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
		j += 3;
	}
};

/*******************************************************
*
* Function name: loadFile
* Description: 读取文件入内存
* Parameter:
*	@closterNum		文件所在扇区号		int
*	@size			文件大小			int
*	@buf			文件内存缓冲区指针	char *
*	@fat			fat表				int *
*	@img			硬盘数据指针		char *
*
**********************************************************/
void loadFile(int closterNum, int size, char *buf, int *fat, char *img) {
	int i;
	for (;;) {
		for (i = 0; i < size; i++) {	// 读取本扇区文件数据写入内存缓冲区
			buf[i] = img[closterNum * 512 + i];
		}
		if (size <= 512) {	// 文件大小在512字节以下（一个扇区内）	
			break;
		}
		// 文件大小在512字节以上（需要寻找下一个扇区）
		size -= 512;	// 文件大小减去一个扇区
		buf += 512;	// 内存缓冲区buf指针后移至刚刚读取的一个扇区数据之后
		closterNum = fat[closterNum];	// 在fat表记录中找到该文件下一个扇区
	}
}

#endif	// FILE_C

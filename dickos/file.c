#ifndef FILE_C
#define FILE_C
/********************************************************************************
* @File name: file.c
* @Author: suvvm
* @Version: 0.0.2
* @Date: 2020-02-07
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

/*******************************************************
*
* Function name: searchFile
* Description: 根据文件名在根目录寻找文件
* Parameter:
*	@name		文件名			char *
*	@fileInfo	fat16根目录		struct FILEINFO *
*	@max		最大查找文件数	int
*	@Return:
*	成功返回文件信息指针 失败返回0
*
**********************************************************/
struct FILEINFO *searchFile(char *name, struct FILEINFO *fileInfo, int max) {
	int i, j;
	char s[12];
	for (j = 0; j < 11; j++) {
		s[j] = ' ';
	}
	j = 0;
	for (i = 0; name[i] != 0; i++) {	// 获取文件名转为与fat16根目录文件名相同格式存入s
		if (j >= 11) {
			return 0;	// 没有找到文件
		}
		if (name[i] == '.' && j <= 8) {	// 找到文件名中的.
			j = 8;	// 开始准备获取扩展名
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z') {	// 小写字母转大写
				s[j] -= 0x20;
			}
			j++;
		}
	}
	// 寻找文件
	for (i = 0; i < max; ) {
		if (fileInfo[i].name[0] == 0x00) {	// 不包含任何文件信息
			break;
		}
		char flag = 0;
		if ((fileInfo[i].type & 0x18) == 0) { // 不为目录
			for (j = 0; j < 11; j++) {
				if (fileInfo[i].name[j] != s[j]) {
					flag = 1;
					break;
				}
			}
		}
		if (flag == 1) {
			i++;
			continue;
		}
		return fileInfo + i;
	}
	return 0;	// 没有找到文件
}

#endif	// FILE_C

# Dickos

这是一个汇编+C语言开发的操作系统

本项目为个人开发os的初次尝试，跟随川合秀实所提供资料进行开发。

当前运行后会读取磁盘10个柱面并执行dickos.sys加载boot引导程序包。

设定16色调色板

初始化GDT & IDT

调用函数boxFill8使用指针操作VRAM显示桌面GUI

在桌面打印DICKOS

进入HALT状态

# Build

windows：在!cons_nt.bat或!cons_9x.bat中输入make install（目标地址为a:）

# Run

windows：在!cons_nt.bat或!cons_9x.bat中输入make run即可调用QEMU运行

# Thanks

- 川合秀实
- RWFD
- 平木敬太郎
- Kiyoto

# LOGS

编写启动设置asmhead.nas

新建汇编函数文件func.nas添加函数io_hlt

新建笔记文件note.txt

新建README.md

新建bootpack.c添加主函数HariMain

bootpack.c -添加存储启动信息的结构体BOOTINFO并在主函数中获取启动信息

func.nas -添加中断操作_io_cli, _io_sti, _stihlt, _io_load_eflags, _io_store_eflags等函数

func.nas -添加指定端口读写函数_io_in8, _io_in16, _io_in32， _io_out8, _io_out16, _io_out32

bootpack.c -定义颜色码，添加16色调色板设置函数set-palette 调色板初始化函数init-palette

bootpack.c -添加矩形绘制函数(指针操作显存)boxFill8 在主函数中调用boxFill8绘制3个矩形

bootpack.c -添加GUI绘制函数init_GUI 主函数删除3个矩形的绘制并调用GUI初始化函数绘制GUI

新建font.txt -添加字体文件

bootpack.c -添加打印单个字符函数putFont8与打印字符串函数putFont8_asc

bootpack.c -主函数调用putFont8_asc在屏幕中打印字符串"DICKOS"

bootpack.c -添加鼠标图像信息初始化函数initMouseCursor8与显示图像的函数putblock8_8

bootpack.c -主函数调用initMouseCursor8与putblock8_8打印鼠标指针

func.nas -添加寄存器赋值函数loadGdtr, _loadIdtr

bootpack.c -添加 段描述符GDT结构体SEGMENT_DESCRIPTOR 门描述符结构体GATE_DESCRIPTOR

bootpack.c -函数段描述符设置函数setSegmdesc 中断描述符设置函数setGatedesc 内存分段初始化函数initGdtit

新建graphic.c -将bootpack.c中关于绘制图像的所有函数提取至graphic.c

新建desctab .c -将bootpack.c中关于描述符表的操作提取至desctab.c

新建bootpack.h -将函数与结构体声明提取至bootpack.h

note.txt -添加对段描述符属性的介绍

bootpack.h -完善对结构体SEGMENT_DESCRIPTOR的注释

desctab .c -修正函数setSegmdesc中不正确的描述

出现未知差错正在尝试修正

# Dickos

这是一个汇编+C语言开发的操作系统

本项目为个人开发os的初次尝试，跟随川合秀实所提供资料进行开发。

当前运行后会读取磁盘10个柱面并执行dickos.sys加载boot引导程序包。

设定16色调色板

初始化GDT & IDT

初始化可编程中断控制器PIC

调用函数boxFill8使用指针操作VRAM显示桌面GUI

在桌面打印DICKOS并显示鼠标坐标

当键盘发送中断时打印键盘中断编码

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

bootpack.c -[主函数调用putFont8_asc在屏幕中打印字符串"DICKOS"]

bootpack.c -[添加鼠标图像信息初始化函数initMouseCursor8与显示图像的函数putblock8_8]

bootpack.c -[主函数调用initMouseCursor8与putblock8_8打印鼠标指针](https://github.com/suvvm/dickOS/commit/4e7ee5b8cb235812928e4019be30c3c1d9bf8115)

func.nas -[添加寄存器赋值函数loadGdtr, _loadIdtr](https://github.com/suvvm/dickOS/commit/e6346fedea2f607c8e1f4f1f9c02e66b90e616db)

bootpack.c -[添加 段描述符GDT结构体SEGMENT_DESCRIPTOR 门描述符结构体GATE_DESCRIPTOR])(https://github.com/suvvm/dickOS/commit/70f7a714bf259abc90f012fd14a41265b125c9af)

bootpack.c -[函数段描述符设置函数setSegmdesc 中断描述符设置函数setGatedesc 内存分段初始化函数initGdtit](https://github.com/suvvm/dickOS/commit/70f7a714bf259abc90f012fd14a41265b125c9af)

新建graphic.c -[将bootpack.c中关于绘制图像的所有函数提取至graphic.c](https://github.com/suvvm/dickOS/commit/abffb82e79d4c1f08d8b728d57cb13b49dabae6f)

新建desctab .c -[将bootpack.c中关于描述符表的操作提取至desctab.c](https://github.com/suvvm/dickOS/commit/abffb82e79d4c1f08d8b728d57cb13b49dabae6f)

新建bootpack.h -[将函数与结构体声明提取至bootpack.h](https://github.com/suvvm/dickOS/commit/b94ce79028d2b250ec2fa310bb6e07942e4f392b)

note.txt -[添加对段描述符属性的介绍](https://github.com/suvvm/dickOS/commit/aa2cbe8cc0c2cee2e1321eacf879cb380e1fd41f)

bootpack.h -[完善对结构体SEGMENT_DESCRIPTOR的注释](https://github.com/suvvm/dickOS/commit/ccf0c68d014a0d2e7481a540411e00bc8a754e5c)

desctab .c -修正函数setSegmdesc中不正确的描述
出现未知bug正在尝试修正

bug已发现，为结构体内存对齐问题，正在修正

bootpack.h -[修正结构体SEGMENT_DESCRIPTOR属性顺序保障内存对齐正确](https://github.com/suvvm/dickOS/commit/6eecd80836e4792602bb820352820b75ccadc848)

tools/com_mak.txt -[修正内部文件路径配置](https://github.com/suvvm/dickOS/commit/faaabd52fed411fe9e7e796c69737b22e57c12c2)

tools/edimgopt.txt -[修正内部文件路径配置](https://github.com/suvvm/dickOS/commit/faaabd52fed411fe9e7e796c69737b22e57c12c2)

tools/guigui00/guigui00.rul -[修正内部文件路径配置](https://github.com/suvvm/dickOS/commit/faaabd52fed411fe9e7e796c69737b22e57c12c2)

tools/include/haribote.rul -[修正内部文件路径配置](https://github.com/suvvm/dickOS/commit/faaabd52fed411fe9e7e796c69737b22e57c12c2)

[bootpack.h](https://github.com/suvvm/dickOS/commit/61759c5fa0ee6740f44d30c24f03125436e20758)&[graphic.c](https://github.com/suvvm/dickOS/commit/16a8121a85b36279bba4533504703671e72a748b) -将色号信息的宏定义转至bootpack.h

[bootpack.h](https://github.com/suvvm/dickOS/commit/61759c5fa0ee6740f44d30c24f03125436e20758)&desctab.c -将要初始描述符信息宏定义

bootpack.h -[宏定义PIC端口信息](https://github.com/suvvm/dickOS/commit/61759c5fa0ee6740f44d30c24f03125436e20758)

新建interrupt.c -[定义初始化可编程中断编译器信息](https://github.com/suvvm/dickOS/commit/2aa01d048739a47087f3fe323dd05fe284a90711)

bootpack.c -[调用interrupt.c的init_pic()初始化可编程中断编译器](https://github.com/suvvm/dickOS/commit/2e5d77cef384efbb4265d0937bfb27bfd277405c)

bootpack.h -[为SEGMENT_DESCRIPTOR添加内存对齐设置](https://github.com/suvvm/dickOS/commit/d3fd9595cba8af2d65d36862ea916ff438441b99)

interrupt.c -[添加中断处理函数interruptHandler21 interruptHandler2c interruptHandler21](https://github.com/suvvm/dickOS/commit/855249dcb46720d68d9d76dd235ccf67a58cb679)

func.nas -[添加函数asm_interruptHandler21 _asm_interruptHandler27 _asm_interruptHandler2c 执行IRETD](https://github.com/suvvm/dickOS/commit/4873750961109c36fa26f3bb8a8852a805ffaeab)

desctab.c -[initGdtit中添加对IRQ1 IRQ7 IRQ12 的IDT设置](https://github.com/suvvm/dickOS/commit/74c62829af333212bffd684927b1d3787e569d47)

bootpack.c -[io_sti()](https://github.com/suvvm/dickOS/commit/b78ab7ef6429e130e92d335b6763ed3c6581e4f7)

func.nas - [修正了_io_stihlt函数名不正确的bug](https://github.com/suvvm/dickOS/commit/5629d1f75e91c2999df93d949cd7e0240e0db84e) 

bootpack.h -[添加部分func.nas函数声明 添加结构体KEYBUF声明全局变量keybuf作为键盘输入缓冲区](https://github.com/suvvm/dickOS/commit/de9a1b44bf18f0b3aae143bab3e41f0739374eab) 

interrupt.c -[修改键盘中断处理函数interruptHandler21由直接处理改为将键盘码存入缓冲区](https://github.com/suvvm/dickOS/commit/63372c8085814a753f2ff70a4193963a5d9f7b10)

bootpack.c -[添加对缓冲区数据显示的处理](https://github.com/suvvm/dickOS/commit/cd739e36d7921c8f14097de8600efb82213ebdc6)

bootpack.h -[ 修改结构体KEYBUF，将缓冲区data设为长度32的数组，并用next记录data中下一个空闲位置](https://github.com/suvvm/dickOS/commit/812bdc064f938bab842500224ec4b6e8694c4eda)

interrupt.c -[ 修改键盘中断处理函数interruptHandler21](https://github.com/suvvm/dickOS/commit/4f434b57bb4eb132d51f3a437ac5b55f8910e5a0)

bootpack.c - [修改键盘缓冲区处理操作](https://github.com/suvvm/dickOS/commit/e98fb67cb073b17f437db9fc91c602b868bf04e1)


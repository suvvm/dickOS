# Dickos

这是一个汇编+C语言开发的操作系统

本项目为个人开发os的初次尝试，跟随川合秀实所提供资料进行开发。

当前运行后会读取磁盘10个柱面并执行dickos.sys加载boot引导程序包。

设定16色调色板

初始化GDT & IDT

初始化可编程中断控制器PIC

调用函数boxFill8使用指针操作VRAM显示桌面GUI

在桌面打印DICKOS并显示鼠标坐标

初始化键盘控制电路并激活鼠标

在鼠标与键盘产生中断时打印相应数据

鼠标移动实现

空闲时进入HALT状态

# Build

windows：在!cons_nt.bat或!cons_9x.bat中输入make install（目标地址为a:）

# Run

windows：在!cons_nt.bat或!cons_9x.bat中输入make run即可调用QEMU运行

#  Command-line instruction 

```makefile
make img 		# 生成映像文件dickos.img
make run		# 生成必要文件并启动qemu模拟器
make install	# 生成必要文件并将其安装至a驱动器
make clean		# 删除中间文件
make src_only	# 删除中间文件并删除dickos.img映像
```

# Memory distribution

```
0x00000000 ~ 0x000fffff : 启动中使用 之后变为空(1MB) 部分被VRAM与BIOS使用，并不全空
0x00100000 ~ 0x00267fff : 保存软盘内容(1440KB)
0x00268000 ~ 0x0026f7ff : 空(30KB)
0x0026f800 ~ 0x0026ffff : IDT(2KB)
0x00270000 ~ 0x0027ffff : GDT(64KB)
0x00280000 ~ 0x002fffff : bootpack.hrb(512KB)
0x00300000 ~ 0x003fffff : 栈及其他(1MB)
0x00400000 ~			: 空
```

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

bootpack.h - [通过循环使用数组优化键盘缓冲区结构体KEYBUF](https://github.com/suvvm/dickOS/commit/bf1593b1cc62b78e3fd55ca37584f83eb5341bb5) 

interrupt.c -[ 修改键盘中断处理函数interruptHandler21](https://github.com/suvvm/dickOS/commit/4b6a3932cc0a7ab614a16c9d6953c238bfb6a0ea)

bootpack.c -[ 修改键盘缓冲区处理操作](https://github.com/suvvm/dickOS/commit/d1ac64801d47e1898e23542c2739993082994088)

make.exe出现重大问题正在寻找错误根源

问题已解决 -重新安装了MinGw与msys

bootpack.h -[ 添加结构体QUEUE模拟队列，添加队列函数声明](https://github.com/suvvm/dickOS/commit/182583217b521efd566711949354405051efe67a)

新建文件queue.h -[保存队列函数定义](https://github.com/suvvm/dickOS/commit/eacc560978c84c0d2553f7de864d94a1e4930290)

interrupt.c -[ 改用队列作为键盘缓冲区](https://github.com/suvvm/dickOS/commit/66bca37b8aaf02e8f6392e16559a4ac6fac8ac29)

bootpack.c - [初始化键盘缓冲区队列，修改键盘缓冲区处理操作](https://github.com/suvvm/dickOS/commit/5a73656723ece0100b4bcb30547debbcdcfcec86) 

[tools](https://github.com/suvvm/dickOS/tree/master/tools)/[include](https://github.com/suvvm/dickOS/tree/master/tools/include)/haribote.rul - [修改入口为Main](https://github.com/suvvm/dickOS/commit/89e6db42bf25880885d8537c8b78e34e5e5de452)

bootpack.c - [修改主函数名为Main](https://github.com/suvvm/dickOS/commit/87ba4b179b51903aacaf7b9cfc2c5a3a27a8dd69)

bootpack.h - [添加键盘控制电路与鼠标相关函数声明与宏定义](https://github.com/suvvm/dickOS/commit/7b09a16e57ac316961f9da950c36f3a4f65622d2#diff-fac193a65494545d15781ecfd834143e) 

新建keyboard.c -[ 设置键盘控制电路](https://github.com/suvvm/dickOS/commit/e2bef556e1f25632f3a614f3c0f55a1e145b0935)

新建mouse.c - [设置鼠标相关](https://github.com/suvvm/dickOS/commit/2e24491459e0ea1b7cd25565d4cbecf2c2e81fbe)

note.txt -[ 队列函数定义](https://github.com/suvvm/dickOS/commit/eacc560978c84c0d2553f7de864d94a1e4930290)

bootpack.h -[声明QUEUE类型变量mousebuf作为鼠标缓冲区](https://github.com/suvvm/dickOS/commit/7717e27e66a42960303225da7c1ff452bff1fa36) 

interrupt.c -[ 修改鼠标中断处理函数interruptHandler2c将鼠标中断产生的数据存入缓冲区](https://github.com/suvvm/dickOS/commit/a44e0949ce8fd15f89be303289cabb136cda0372)

bootpack.c -[ 初始化鼠标缓冲区mousebuf并添加对其的处理](https://github.com/suvvm/dickOS/commit/d707e714be19f7e2952411d9cc7e9690b0d825c9)

mouse.c - [添加文件结束符](https://github.com/suvvm/dickOS/commit/ac636a0e0d414d951ea9f3972c81c4e6895f7953) 

bootpack.c -[ 添加对鼠标传输的信息处理](https://github.com/suvvm/dickOS/commit/eff4fe86dfae485493119b9545813417a616f11e)

bootpack.h -[ 添加结构体MouseDec用来存储鼠标信息](https://github.com/suvvm/dickOS/commit/80b36c468fabd1748cecc1966a05ee8882eff4ff)

mouse.c -[ 函数enableMouse接收保存鼠标信息的结构体并将其设为等待鼠标激活回复状态，添加函数mouseDecode接收鼠标数据并对鼠标信息进行设置 ]( https://github.com/suvvm/dickOS/commit/05df1a37b8a8c5f7de2e491169a3245d30f6e9d7 )

bootpack.c -[ 修改鼠标信息处理步骤](https://github.com/suvvm/dickOS/commit/5c0f3a404242b212cd09dbddc89586ffda81964a)

bootpack.c - [添加鼠标移动相关操作](https://github.com/suvvm/dickOS/commit/681e182f73285212aa73ed71e0b993eef187f050) 

mouse.c -[ 添加文件结束符](https://github.com/suvvm/dickOS/commit/fa2f0a5acc1c8eb564f7e517d12ccb7ec79ff31c)

queue.h - [添加文件结束符](https://github.com/suvvm/dickOS/commit/fa2f0a5acc1c8eb564f7e517d12ccb7ec79ff31c) 

asmhead.nas -[ 添加针对部分方法的详细注释](https://github.com/suvvm/dickOS/commit/c3fa9e05780cf28b7a58e840c31059585147970b)

note.txt -[ 添加指令JZ IMUL SUB的介绍](https://github.com/suvvm/dickOS/commit/c8af1eb1c5887dc91a2d369ff95b30ab70e0b560)

README.md -[ 添加内存分布描述](https://github.com/suvvm/dickOS/commit/9372a8bcd8c564cf21b492b8a4479942f0ae3ca6)

README.md -[ 添加命令行指令描述](https://github.com/suvvm/dickOS/commit/a618b407eab4b8cb3ccffef3d089fb6e7b1f1a81)

func.nas - [添加针对寄存器cr0的读写函数](https://github.com/suvvm/dickOS/commit/8d1aa75f0fe5fbc858f540e428e687edca6da68c) 

bootpack.h -[ 添加func.nas的函数声明，添加关于eflagAC位与cr0中启用或禁止缓存的宏定义](https://github.com/suvvm/dickOS/commit/bb42c2c1934a87fa8f2a1354489ea2147b7ef4ce)

bootpack.c - [添加判断内存大小相关函数](https://github.com/suvvm/dickOS/commit/81e0428810334e312e81b70863e4beacb103f08e) 

# 许可协议原文

```开源协议原文
通俗地说：“这是自由软件，使用前使用后都无须支付任何费用，可以自由使用，也可以复制、修改，以及用于商业目的，但是作者对软件的bug以及因此所造成的损失不负任何责任。”

为了保护软件使用者以及软件修改者、参考者的利益，即便作者将来为该软件中所使用的算法申请专利，也保证不会向使用者收取授权费用。

本协议并不代表作者放弃著作权（仅放弃独家复制发布的权利），使用者可以自由复制并 再次发布本软件的一部分甚至全部内容。使用时无须支付任何费用，对本软件的解析、修改也无须支付任何费用。

适用于本协议的软件，可以无条件地用作商业目的，甚至可以将软件直接有偿出售。

对于以适用于本协议的软件的一部分或全部内容为基础，在此之上所开发的软件（下称“派生物”），可以赋予任何种类的许可协议。即，派生物的作者可以禁止对派生物进行复制，也可以只允许派生物以有偿形式发布。派生物的著作权属于该派生物的生成者，适用于本协 议的软件作者不会对其派生物主张著作权。

在派生物的文档中，没有义务对原始软件的作者进行介绍，但这句话，理所当然地，不意味着禁止这一行为。

在发布派生物时，没有义务取得原始软件作者的同意，但这句话，理所当然地，不意味着禁止这一行为。

作者不对软件的质量作出任何保证，因此，由于使用本软件所造成的损害，或者没有达到所期望的结果，作者不负任何责任。

对于适用于本协议的软件中所使用的技术，除了事先取得作者授权的人以外，禁止其他人对其申请专利。但如果附加了新的技术并生成了派生物，允许对于追加的部分申请专利。作者在将来有可能对软件中的技术申请专利，但保证不会向派生物以及软件的使用收取授权费 用。本保证不仅适用于申请专利之前所生成的派生物，也适用于专利取得后所生成的派生物。

将解析结果汇总并申请专利，需要事先取得作者的同意，但对于申请专利以外的行为，没有任何限制。
```


# Dickos

这是一个汇编+C语言开发的操作系统

本项目为个人开发os的初次尝试，跟随川合秀实所提供资料进行开发。

当前运行后会读取磁盘10个柱面并执行dickos.sys加载boot引导程序包使用指针操作VRAM显示全屏条纹进入HALT状态

# Build

windows：在!cons_nt.bat或!cons_9x.bat中输入make install（目标地址为a:）

# Run

windows：在!cons_nt.bat或!cons_9x.bat中输入make run即可调用QEMU运行

# Thanks

- 川合秀实
- RWFD
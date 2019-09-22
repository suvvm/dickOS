# 文件生成规则
ipl.bin : ipl.nas Makefile
	tools/nask.exe ipl.nas ipl.bin ipl.lst

dickos.img : ipl.bin Makefile
	tools/edimg.exe	imgin:tools/fdimg0at.tek wbinimg src:ipl.bin len:512 from:0 to:0	imgout:dickos.img

#命令
asm :
	tools/make.exe -r ipl.bin

img :
	tools/make.exe r dickos.img

run : 
	tools/make.exe img
	copy dickos.img tools\qemu\fdimage0.bin
	tools/make.exe -C tools/qemu

install :
	tools/make.exe img
	tools/imgtol.com w a: dickos.img

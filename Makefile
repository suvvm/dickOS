TOOLPATH = tools/
INCPATH  = tools/include/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1		 = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(TOOLPATH)include/haribote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del

# 默认动作

default :
	$(MAKE) img

# 文件生成规则

ipl.bin : ipl.nas Makefile
	$(NASK) ipl.nas ipl.bin ipl.lst
	
asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin asmhead.lst
	
bootpack.gas : bootpack.c Makefile
	$(CC1) -o bootpack.gas bootpack.c

bootpack.nas : bootpack.gas Makefile
	$(GAS2NASK) bootpack.gas bootpack.nas

bootpack.obj : bootpack.nas Makefile
	$(NASK) bootpack.nas bootpack.obj bootpack.lst

func.obj : func.nas Makefile
	$(NASK) func.nas func.obj func.lst
	
font.bin : font.txt Makefile
	$(MAKEFONT) font.txt font.bin
	
font.obj : font.bin Makefile
	$(BIN2OBJ) font.bin  font.obj _font

bootpack.bim : bootpack.obj func.obj font.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		bootpack.obj func.obj font.obj
# 3MB+64KB=3136KB
bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

dickos.sys :  asmhead.bin bootpack.hrb Makefile
	copy /B asmhead.bin+bootpack.hrb dickos.sys
	
dickos.img : ipl.bin dickos.sys Makefile
	$(EDIMG)   imgin:tools/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0 \
		copy from:dickos.sys to:@: \
		imgout:dickos.img

# 命令

img :
	$(MAKE) dickos.img

run :
	$(MAKE) img
	$(COPY) dickos.img tools\qemu\fdimage0.bin
	$(MAKE) -C tools/qemu

install :
	$(MAKE) img
	$(IMGTOL) dickos.img a:

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) dickos.sys

src_only :
	$(MAKE) clean
	-$(DEL) dickos.img

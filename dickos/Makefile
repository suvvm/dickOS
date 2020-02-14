TOOLPATH = ../tools/
INCPATH  = ../tools/include/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(TOOLPATH)include/haribote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
GOLIB    = $(TOOLPATH)golib00.exe 
COPY     = copy
DEL      = del

# Ĭ�϶���

default :
	$(MAKE) ipl.bin
	$(MAKE) dickos.sys

# �ļ����ɹ���

ipl.bin : ipl.nas Makefile
	$(NASK) ipl.nas ipl.bin ipl.lst

asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin asmhead.lst
	
bootpack.gas : bootpack.c Makefile
	$(CC1) -o bootpack.gas bootpack.c

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

# һ�����

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

# ����

clean :
	-$(DEL) asmhead.bin
	-$(DEL) font.bin
	-$(DEL) *.lst
	-$(DEL) *.obj
	-$(DEL) *.map
	-$(DEL) *.bim
	-$(DEL) *.hrb

src_only :
	$(MAKE) clean
	-$(DEL) ipl.bin
	-$(DEL) dickos.sys
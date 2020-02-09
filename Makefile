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
	
helloC.gas : helloC.c Makefile
	$(CC1) -o helloC.gas helloC.c
	
bug1.gas : bug1.c Makefile
	$(CC1) -o bug1.gas bug1.c

bug2.gas : bug2.c Makefile
	$(CC1) -o bug2.gas bug2.c

bug3.gas : bug3.c Makefile
	$(CC1) -o bug3.gas bug3.c
	
helloCS.gas : helloCS.c Makefile
	$(CC1) -o helloCS.gas helloCS.c
	
winHelo.gas : winHelo.c Makefile
	$(CC1) -o winHelo.gas winHelo.c

winHelo2.gas : winHelo2.c Makefile
	$(CC1) -o winHelo2.gas winHelo2.c

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

hello.hrb : hello.nas Makefile
	$(NASK) hello.nas hello.hrb hello.lst

helloStr.hrb : helloStr.nas Makefile
	$(NASK) helloStr.nas helloStr.hrb helloStr.lst

helloC.bim : helloC.obj helloCFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:helloC.bim map:helloC.map helloC.obj helloCFunc.obj

helloC.hrb : helloC.bim Makefile
	$(BIM2HRB) helloC.bim helloC.hrb 0
	
bug1.bim : bug1.obj helloCFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug1.bim map:bug1.map bug1.obj helloCFunc.obj

bug1.hrb : bug1.bim Makefile
	$(BIM2HRB) bug1.bim bug1.hrb 0

bug2.bim : bug2.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug2.bim map:bug2.map bug2.obj

bug2.hrb : bug2.bim Makefile
	$(BIM2HRB) bug2.bim bug2.hrb 0

bug3.bim : bug3.obj helloCFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug3.bim map:bug3.map bug3.obj helloCFunc.obj

bug3.hrb : bug3.bim Makefile
	$(BIM2HRB) bug3.bim bug3.hrb 0
	
helloCS.bim : helloCS.obj helloCFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:helloCS.bim stack:1k map:helloCS.map \
		helloCS.obj helloCFunc.obj
	
helloCS.hrb : helloCS.bim Makefile
	$(BIM2HRB) helloCS.bim helloCS.hrb 0
	
winHelo.bim : winHelo.obj helloCFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winHelo.bim stack:1k map:winHelo.map \
		winHelo.obj helloCFunc.obj

winHelo.hrb : winHelo.bim Makefile
	$(BIM2HRB) winHelo.bim winHelo.hrb 0

winHelo2.bim : winHelo2.obj helloCFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winHelo2.bim stack:1k map:winHelo2.map \
		winHelo2.obj helloCFunc.obj

winHelo2.hrb : winHelo2.bim Makefile
	$(BIM2HRB) winHelo2.bim winHelo2.hrb 0

dickos.sys :  asmhead.bin bootpack.hrb Makefile
	copy /B asmhead.bin+bootpack.hrb dickos.sys
	
dickos.img : ipl.bin dickos.sys Makefile \
		hello.hrb helloStr.hrb helloC.hrb winHelo.hrb\
		bug1.hrb bug2.hrb bug3.hrb winHelo2.hrb\
		helloCS.hrb
	$(EDIMG)   imgin:tools/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0 \
		copy from:dickos.sys to:@: \
		copy from:ipl.nas to:@: \
		copy from:make.bat to:@: \
		copy from:hello.hrb to:@: \
		copy from:helloStr.hrb to:@: \
		copy from:helloC.hrb to:@: \
		copy from:bug1.hrb to:@: \
		copy from:bug2.hrb to:@: \
		copy from:bug3.hrb to:@: \
		copy from:helloCS.hrb to:@: \
		copy from:winHelo.hrb to:@: \
		copy from:winHelo2.hrb to:@: \
		imgout:dickos.img

# 通用规则
%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas
	
%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

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
	-$(DEL) *.map
	-$(DEL) *.bim
	-$(DEL) *.hrb
	-$(DEL) dickos.sys

src_only :
	$(MAKE) clean
	-$(DEL) dickos.img

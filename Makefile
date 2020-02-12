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
	
winHelo3.gas : winHelo3.c Makefile
	$(CC1) -o winHelo3.gas winHelo3.c

star1.gas : star1.c Makefile
	$(CC1) -o star1.gas star1.c
	
star2.gas : star2.c Makefile
	$(CC1) -o star2.gas star2.c

stars.gas : stars.c Makefile
	$(CC1) -o stars.gas stars.c

line.gas : line.c Makefile
	$(CC1) -o line.gas line.c
	
walk.gas : walk.c Makefile
	$(CC1) -o walk.gas walk.c
	
noodle.gas : noodle.c Makefile
	$(CC1) -o noodle.gas noodle.c 

beepDown.gas : beepDown.c Makefile
	$(CC1) -o beepDown.gas beepDown.c 

color.gas : color.c Makefile
	$(CC1) -o color.gas color.c

color2.gas : color2.c Makefile
	$(CC1) -o color2.gas color2.c	


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

helloC.bim : helloC.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:helloC.bim map:helloC.map helloC.obj apiFunc.obj

helloC.hrb : helloC.bim Makefile
	$(BIM2HRB) helloC.bim helloC.hrb 0
	
bug1.bim : bug1.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug1.bim map:bug1.map bug1.obj apiFunc.obj

bug1.hrb : bug1.bim Makefile
	$(BIM2HRB) bug1.bim bug1.hrb 0

bug2.bim : bug2.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug2.bim map:bug2.map bug2.obj

bug2.hrb : bug2.bim Makefile
	$(BIM2HRB) bug2.bim bug2.hrb 0

bug3.bim : bug3.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bug3.bim map:bug3.map bug3.obj apiFunc.obj

bug3.hrb : bug3.bim Makefile
	$(BIM2HRB) bug3.bim bug3.hrb 0
	
helloCS.bim : helloCS.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:helloCS.bim stack:1k map:helloCS.map \
		helloCS.obj apiFunc.obj
	
helloCS.hrb : helloCS.bim Makefile
	$(BIM2HRB) helloCS.bim helloCS.hrb 0
	
winHelo.bim : winHelo.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winHelo.bim stack:1k map:winHelo.map \
		winHelo.obj apiFunc.obj

winHelo.hrb : winHelo.bim Makefile
	$(BIM2HRB) winHelo.bim winHelo.hrb 0

winHelo2.bim : winHelo2.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winHelo2.bim stack:1k map:winHelo2.map \
		winHelo2.obj apiFunc.obj

winHelo2.hrb : winHelo2.bim Makefile
	$(BIM2HRB) winHelo2.bim winHelo2.hrb 0
	
winHelo3.bim : winHelo3.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:winHelo3.bim stack:1k map:winHelo3.map \
		winHelo3.obj apiFunc.obj

winHelo3.hrb : winHelo3.bim Makefile
	$(BIM2HRB) winHelo3.bim winHelo3.hrb 40k

star1.bim : star1.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:star1.bim stack:1k map:star1.map \
		star1.obj apiFunc.obj

star1.hrb : star1.bim Makefile
	$(BIM2HRB) star1.bim star1.hrb 47k
	
star2.bim : star2.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:star2.bim stack:1k map:star2.map \
		star2.obj apiFunc.obj

star2.hrb : star2.bim Makefile
	$(BIM2HRB) star2.bim star2.hrb 47k

stars.bim : stars.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:stars.bim stack:1k map:stars.map \
		stars.obj apiFunc.obj

stars.hrb : stars.bim Makefile
	$(BIM2HRB) stars.bim stars.hrb 47k

line.bim : line.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:line.bim stack:1k map:line.map \
		line.obj apiFunc.obj

line.hrb : line.bim Makefile
	$(BIM2HRB) line.bim line.hrb 48k

walk.bim : walk.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:walk.bim stack:1k map:walk.map \
		walk.obj apiFunc.obj

walk.hrb : walk.bim Makefile
	$(BIM2HRB) walk.bim walk.hrb 48k
	
noodle.bim : noodle.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:noodle.bim stack:1k map:noodle.map \
		noodle.obj apiFunc.obj

noodle.hrb : noodle.bim Makefile
	$(BIM2HRB) noodle.bim noodle.hrb 40k
	
beepDown.bim : beepDown.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:beepDown.bim stack:1k map:beepDown.map \
		beepDown.obj apiFunc.obj

beepDown.hrb : beepDown.bim Makefile
	$(BIM2HRB) beepDown.bim beepDown.hrb 40k
	
color.bim : color.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:color.bim stack:1k map:color.map \
		color.obj apiFunc.obj

color.hrb : color.bim Makefile
	$(BIM2HRB) color.bim color.hrb 56k

color2.bim : color2.obj apiFunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:color2.bim stack:1k map:color2.map \
		color2.obj apiFunc.obj

color2.hrb : color2.bim Makefile
	$(BIM2HRB) color2.bim color2.hrb 56k

dickos.sys :  asmhead.bin bootpack.hrb Makefile
	copy /B asmhead.bin+bootpack.hrb dickos.sys
	
dickos.img : ipl.bin dickos.sys Makefile \
		hello.hrb helloStr.hrb helloC.hrb winHelo.hrb line.hrb color.hrb color2.hrb \
		bug1.hrb bug2.hrb bug3.hrb winHelo2.hrb star2.hrb walk.hrb \
		helloCS.hrb winHelo3.hrb star1.hrb stars.hrb noodle.hrb beepDown.hrb
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
		copy from:winHelo3.hrb to:@: \
		copy from:star1.hrb to:@: \
		copy from:stars.hrb to:@: \
		copy from:star2.hrb to:@: \
		copy from:line.hrb to:@: \
		copy from:walk.hrb to:@: \
		copy from:noodle.hrb to:@: \
		copy from:beepDown.hrb to:@: \
		copy from:color.hrb to:@: \
		copy from:color2.hrb to:@: \
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

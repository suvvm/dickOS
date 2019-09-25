TOOLPATH = tools/
MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del

# デフォルト動作

default :
	$(MAKE) img

# ファイル生成規則

ipl.bin : ipl.nas Makefile
	$(NASK) ipl.nas ipl.bin ipl.lst

dickos.img : ipl.bin Makefile
	$(EDIMG)   imgin:tools/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0   imgout:dickos.img

# コマンド

asm :
	$(MAKE) ipl.bin

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
	-$(DEL) ipl.bin
	-$(DEL) ipl.lst

src_only :
	$(MAKE) clean
	-$(DEL) dickos.img

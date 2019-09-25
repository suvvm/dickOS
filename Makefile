TOOLPATH = tools/
MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del

# ‡“??çÏ

default :
	$(MAKE) img

# ï∂åèê∂ê¨??

ipl.bin : ipl.nas Makefile
	$(NASK) ipl.nas ipl.bin ipl.lst

dickos.sys : dickos.nas Makefile
	$(NASK) dickos.nas dickos.sys dickos.lst
	
dickos.img : ipl.bin dickos.sys Makefile
	$(EDIMG)   imgin:tools/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0 \
		copy from:dickos.sys to:@: \
		imgout:dickos.img

# ñΩóﬂ

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
	-$(DEL) dickos.sys
	-$(DEL) dickos.lst

src_only :
	$(MAKE) clean
	-$(DEL) dickos.img

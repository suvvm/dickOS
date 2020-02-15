TOOLPATH = tools/
INCPATH  = tools/include/

MAKE     = $(TOOLPATH)make.exe -r
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del

# 默认动作

default :
	$(MAKE) dickos.img

# 文件生成规则

dickos.img : dickos/ipl.bin dickos/dickos.sys Makefile \
		helloC/helloC.hrb winHelo/winHelo.hrb line/line.hrb color/color.hrb color2/color2.hrb \
		winHelo2/winHelo2.hrb star2/star2.hrb walk/walk.hrb prime/prime.hrb \
		helloCS/helloCS.hrb winHelo3/winHelo3.hrb star1/star1.hrb stars/stars.hrb noodle/noodle.hrb beepDown/beepDown.hrb
	$(EDIMG)   imgin:tools/fdimg0at.tek \
		wbinimg src:dickos/ipl.bin len:512 from:0 to:0 \
		copy from:dickos/dickos.sys to:@: \
		copy from:dickos/ipl.nas to:@: \
		copy from:make.bat to:@: \
		copy from:helloC/helloC.hrb to:@: \
		copy from:helloCS/helloCS.hrb to:@: \
		copy from:winHelo/winHelo.hrb to:@: \
		copy from:winHelo2/winHelo2.hrb to:@: \
		copy from:winHelo3/winHelo3.hrb to:@: \
		copy from:star1/star1.hrb to:@: \
		copy from:stars/stars.hrb to:@: \
		copy from:star2/star2.hrb to:@: \
		copy from:line/line.hrb to:@: \
		copy from:walk/walk.hrb to:@: \
		copy from:noodle/noodle.hrb to:@: \
		copy from:beepDown/beepDown.hrb to:@: \
		copy from:color/color.hrb to:@: \
		copy from:color2/color2.hrb to:@: \
		copy from:prime/prime.hrb to:@: \
		imgout:dickos.img
# 命令

run :
	$(MAKE) dickos.img
	$(COPY) dickos.img tools\qemu\fdimage0.bin
	$(MAKE) -C tools/qemu

install :
	$(MAKE) dickos.img
	$(IMGTOL) dickos.img a

full :
	$(MAKE) -C dickos
	$(MAKE) -C apilib
	$(MAKE) -C helloC
	$(MAKE) -C helloCS
	$(MAKE) -C winHelo
	$(MAKE) -C winHelo2
	$(MAKE) -C winHelo3
	$(MAKE) -C star1
	$(MAKE) -C stars
	$(MAKE) -C star2
	$(MAKE) -C line
	$(MAKE) -C walk
	$(MAKE) -C noodle
	$(MAKE) -C beepDown
	$(MAKE) -C color
	$(MAKE) -C color2
	$(MAKE) -C prime
	$(MAKE) dickos.img

run_full :
	$(MAKE) full
	$(COPY) dickos.img tools\qemu\fdimage0.bin
	$(MAKE) -C tools/qemu

install_full :
	$(MAKE) full
	$(IMGTOL) dickos.img a:

run_os :
	$(MAKE) -C dickos
	$(MAKE) run

clean :

src_only :
	$(MAKE) clean
	-$(DEL) dickos.img

clean_full :
	$(MAKE) -C dickos		clean
	$(MAKE) -C apilib		clean
	$(MAKE) -C helloC		clean
	$(MAKE) -C helloCS		clean
	$(MAKE) -C winHelo		clean
	$(MAKE) -C winHelo2		clean
	$(MAKE) -C winHelo3		clean
	$(MAKE) -C star1		clean
	$(MAKE) -C stars		clean
	$(MAKE) -C star2		clean
	$(MAKE) -C line			clean
	$(MAKE) -C walk			clean
	$(MAKE) -C noodle		clean
	$(MAKE) -C beepDown		clean
	$(MAKE) -C color		clean
	$(MAKE) -C color2		clean
	$(MAKE) -C prime		clean

src_only_full :
	$(MAKE) -C dickos		src_only
	$(MAKE) -C apilib		src_only
	$(MAKE) -C helloC		src_only
	$(MAKE) -C helloCS		src_only
	$(MAKE) -C winHelo		src_only
	$(MAKE) -C winHelo2		src_only
	$(MAKE) -C winHelo3		src_only
	$(MAKE) -C star1		src_only
	$(MAKE) -C stars		src_only
	$(MAKE) -C star2		src_only
	$(MAKE) -C line			src_only
	$(MAKE) -C walk			src_only
	$(MAKE) -C noodle		src_only
	$(MAKE) -C beepDown		src_only
	$(MAKE) -C color		src_only
	$(MAKE) -C color2		src_only
	$(MAKE) -C prime		src_only
	-$(DEL) dickos.img

refresh :
	$(MAKE) full
	$(MAKE) clean_full
	-$(DEL) dickos.img

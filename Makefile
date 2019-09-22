# 文件生成规则
ipl.bin : ipl.nas Makefile
	tools/nask.exe ipl.nas ipl.bin ipl.lst

dickos.img : ipl.bin Makefile
	tools/edimg.exe	imgin:tools/fdimg0at.tek wbinimg src:ipl.bin len:512 from:0 to:0	imgout:dickos.img
#!Makefile
#author: wei-coder
#date:	2017-12
#purpose:	kernel的Makefile,默认使用的C语言编译器是 GCC、汇编语言编译器是 nasm
#

# patsubst 函数的目的是根据某种模式替换字符串
#下面代码的目的是定义C源码文件，object文件，其中C文件是通过查找*.c获取，而object文件则是文件名与源文件相同，后缀改为.o
C_SOURCES = $(shell find . -name "*.c")
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
S_SOURCES = $(shell find . -name "*.s")
S_OBJECTS = $(patsubst %.s, %.o, $(S_SOURCES))

CC = gcc
LD = ld
ASM = nasm

C_FLAGS = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-builtin -fno-stack-protector -I include
LD_FLAGS = -T kernel.ld -m elf_i386 -nostdlib -Map=walnut.map
ASM_FLAGS = -f elf -g -F stabs

all: $(S_OBJECTS) $(C_OBJECTS) link update_image

# The automatic variable `$<' is just the first prerequisite
.c.o:
	@echo 编译代码文件 $< ...
	$(CC) $(C_FLAGS) $< -o $@

.s.o:
	@echo 编译汇编文件 $< ...
	$(ASM) $(ASM_FLAGS) $<

link:
	@echo 链接内核文件...
	$(LD) $(LD_FLAGS) $(S_OBJECTS) $(C_OBJECTS) -o walnut

.PHONY:clean
clean:
	$(RM) $(S_OBJECTS) $(C_OBJECTS) walnut

.PHONY:update_image
update_image:
	sudo mount walnut.img /mnt/kernel
	sudo cp walnut /mnt/kernel/walnut
	sleep 1
	sudo umount /mnt/kernel

.PHONY:mount_image
mount_image:
	sudo mount walnut.img /mnt/kernel

.PHONY:umount_image
umount_image:
	sudo umount /mnt/kernel	

.PHONY:qemu
qemu:
	qemu -fda walnut.img -boot a

.PHONY:bochs
bochs:
	bochs -f scripts/bochsrc.txt

.PHONY:debug
debug:
	qemu-system-i386 -S -s -fda walnut.img -boot a &
	sleep 1
	gdb -x gdbinit

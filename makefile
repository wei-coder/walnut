#!Makefile
#author: wei-coder
#date:	2017-12
#purpose:	kernel��Makefile,Ĭ��ʹ�õ�C���Ա������� GCC��������Ա������� nasm
#

# patsubst ������Ŀ���Ǹ���ĳ��ģʽ�滻�ַ���
#��������Ŀ���Ƕ���CԴ���ļ���object�ļ�������C�ļ���ͨ������*.c��ȡ����object�ļ������ļ�����Դ�ļ���ͬ����׺��Ϊ.o
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
	@echo ��������ļ� $< ...
	$(CC) $(C_FLAGS) $< -o $@

.s.o:
	@echo �������ļ� $< ...
	$(ASM) $(ASM_FLAGS) $<

link:
	@echo �����ں��ļ�...
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

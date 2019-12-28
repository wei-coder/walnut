#!Makefile
#author: wei-coder
#date:	2017-12
#purpose:	kernelµƒMakefile,ƒ¨»œ π”√µƒC”Ô—‘±‡“Î∆˜ « GCC°¢ª„±‡”Ô—‘±‡“Î∆˜ « nasm
#

CC = gcc
LD = ld
ASM = nasm

ROOT := $(shell pwd)
INCLUDE := $(ROOT)/include
BUILD_DIR = $(ROOT)/build

SUBDIR = init boot driver IPC vfs entry init lib mm task util

MOD_OBJS = $(wildcard $(BUILD_DIR)/*.a)

LD_FLAGS = -T kernel.ld -m elf_i386 -nostdlib -Map=walnut.map

all: build update_image

.PHONY:build
build:
	-rm -rf $(BUILD_DIR)
	-mkdir $(BUILD_DIR)
	for subdir in $(SUBDIR); do \
		make -C $$subdir OUTPUT=$(BUILD_DIR) INCDIR=$(INCLUDE); \
	done
	@echo "*******start link target******"
	$(LD) $(LD_FLAGS) $(BUILD_DIR)/*.o -o walnut

.PHONY:clean
clean:
	$(RM) walnut
	$(RM) -rf build/
	for subdir in $(SUBDIR); do \
		make clean -C $$subdir OUTPUT=$(BUILD_DIR) INCDIR=$(INCLUDE); \
	done

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
	qemu -S -s -fda walnut.img -hda ../share/walnut_hd.vhd -boot a &
	sleep 1
	gdb -x gdbinit

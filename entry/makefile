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
ASM = nasm

C_FLAGS = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-builtin -fno-stack-protector -I $(INCDIR)
ASM_FLAGS = -f elf -g -F stabs

all: $(S_OBJECTS) $(C_OBJECTS) copy

# The automatic variable `$<' is just the first prerequisite
.c.o:
	@echo compile source file $< ...
	$(CC) $(C_FLAGS) $<

.s.o:
	@echo compile assemble file $< ...
	$(ASM) $(ASM_FLAGS) $<

copy:
	@echo link target lib...
	cp $(S_OBJECTS) $(C_OBJECTS) $(OUTPUT)

.PHONY:clean
clean:
	$(RM) $(S_OBJECTS) $(C_OBJECTS)


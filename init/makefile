#!Makefile
#author: wei-coder
#date:	2017-12
#purpose:	kernel的Makefile,默认使用的C语言编译器是 GCC、汇编语言编译器是 nasm
#

# patsubst 函数的目的是根据某种模式替换字符串
#下面代码的目的是定义C源码文件，object文件，其中C文件是通过查找*.c获取，而object文件则是文件名与源文件相同，后缀改为.o
C_SOURCES = $(shell find . -name "*.c")
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))

CC = gcc

C_FLAGS = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-builtin -fno-stack-protector -I $(INCDIR)

all: $(C_OBJECTS) copy

# The automatic variable `$<' is just the first prerequisite
.c.o:
	@echo compile source file $< ...
	$(CC) $(C_FLAGS) $<

copy:
	@echo copy object to build...
	cp $(C_OBJECTS) $(OUTPUT)

.PHONY:clean
clean:
	$(RM) $(C_OBJECTS)

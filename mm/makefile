#!Makefile
#author: wei-coder
#date:	2017-12
#purpose:	kernel��Makefile,Ĭ��ʹ�õ�C���Ա������� GCC��������Ա������� nasm
#

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
	@echo copy objects to build...
	cp $(S_OBJECTS) $(C_OBJECTS) $(OUTPUT)

.PHONY:clean
clean:
	$(RM) $(S_OBJECTS) $(C_OBJECTS)


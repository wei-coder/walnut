#!Makefile
#author: wei-coder
#date:	2017-12
#purpose:	kernel��Makefile,Ĭ��ʹ�õ�C���Ա������� GCC��������Ա������� nasm
#

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
	@echo copy objects to build...
	cp $(C_OBJECTS) $(OUTPUT)

.PHONY:clean
clean:
	$(RM) $(C_OBJECTS)


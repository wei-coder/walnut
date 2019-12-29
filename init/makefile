#!Makefile
#author: wei-coder
#date:	2017-12
#purpose:	kernel��Makefile,Ĭ��ʹ�õ�C���Ա������� GCC��������Ա������� nasm
#

# patsubst ������Ŀ���Ǹ���ĳ��ģʽ�滻�ַ���
#��������Ŀ���Ƕ���CԴ���ļ���object�ļ�������C�ļ���ͨ������*.c��ȡ����object�ļ������ļ�����Դ�ļ���ͬ����׺��Ϊ.o
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
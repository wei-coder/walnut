#!Makefile
#author: wei-coder
#date:	2017-12
#purpose:	boot��Makefile,Ĭ��ʹ�õ�C���Ա������� GCC��������Ա������� nasm
#

S_SOURCES = $(shell find . -name "*.s")
S_OBJECTS = $(patsubst %.s, %.o, $(S_SOURCES))

ASM = nasm

ASM_FLAGS = -f elf -g -F stabs

all: $(S_OBJECTS) copy

# The automatic variable `$<' is just the first prerequisite
.s.o:
	@echo compile assemle file $< ...
	$(ASM) $(ASM_FLAGS) $<

copy:
	@echo copy objects to build ...
	cp $(S_OBJECTS) $(OUTPUT)

.PHONY:clean
clean:
	$(RM) $(S_OBJECTS)


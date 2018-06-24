/*
filename:	kern_debug.h
author:		wei-coder
date:		2017-12
purpose:	�����ں˵��Ժ�����������ݽṹ����������
*/

#ifndef _KERN_DEBUG_H
#define _KERN_DEBUG_H

#include "types.h"

#define ELF32_ST_TYPE(i) ((i)&0xf)

#define assert(x, info)		\
	do {					\
		if (!(x)) {			\
			panic(info);	\
		}					\
	} while (0)

typedef struct elf_section_header_t
{
	u32 name;
	u32 type;
	u32 flags;
	u32 addr;
	u32 offset;
	u32 size;
	u32 link;
	u32 info;
	u32 addralign;
	u32 entsize;
} __attribute__((packed))elf_sechd_t;

typedef struct ELF_SYMBOL
{
	u32	name;
	u32	value;
	u32	size;
	u32	info;
	u32	other;
	u32	shndx;
} __attribute__((packed))elf_symbol_t;


typedef struct ELF_struct
{
	
	elf_symbol_t	*symtab;
	u32				symtabsz;
	const char		*strtab;
	u32				strtabsz;
}elf_t;

void init_debug();

void print_stack_trace();

void panic(const char * msg);

#endif

/*
filename:	kern_debug.c
author:		wei-coder
date:		2017-12
purpose:	内核调试功能的相关函数定义
*/
#include <string.h>
#include <kio.h>
#include <multiboot.h>
#include "../util/logging.h"
#include "../mm/mm.h"
#include "kern_debug.h"

static elf_t kernel_elf;
extern multiboot_t *glb_mboot_ptr;

elf_t elf_from_multiboot(multiboot_t *mb)
{
	int i;
	elf_t elf;
	elf_sechd_t *sh = (elf_sechd_t *)mb->addr;

	u32 shstrtab = sh[mb->shndx].addr;
	for (i = 0; i < mb->num; i++)
	{
		const char *name = (const char *)(shstrtab + sh[i].name) + PAGE_OFFSET;
		if (strcmp(name, ".strtab") == 0)
		{
			elf.strtab = (const char *)sh[i].addr + PAGE_OFFSET;
			elf.strtabsz = sh[i].size;
		}
		if (strcmp(name, ".symtab") == 0)
		{
			elf.symtab = (elf_symbol_t *)(sh[i].addr + PAGE_OFFSET);
			elf.symtabsz = sh[i].size;
		}
	}

	return elf;
}

void init_debug()
{
	kernel_elf = elf_from_multiboot(glb_mboot_ptr);
	logging("init debug success!\n");
}

const char *elf_lookup_symbol(u32 addr, elf_t *elf)
{
	int i;

	for (i = 0; i < (elf->symtabsz / sizeof(elf_symbol_t)); i++)
	{
		if (ELF32_ST_TYPE(elf->symtab[i].info) != 0x2)
		{
		      continue;
		}

		if ( (addr >= elf->symtab[i].value) && (addr < (elf->symtab[i].value + elf->symtab[i].size)) )
		{
			return (const char *)((u32)elf->strtab + elf->symtab[i].name);
		}
	}

	return NULL;
};


void print_cur_status()
{
	static int round = 0;
	u16 reg1, reg2, reg3, reg4;

	asm volatile ( 	"mov %%cs, %0;"
			"mov %%ds, %1;"
			"mov %%es, %2;"
			"mov %%ss, %3;"
			: "=m"(reg1), "=m"(reg2), "=m"(reg3), "=m"(reg4));

	printf("%d: @ring %d\n", round, reg1 & 0x3);
	printf("%d:  cs = %x\n", round, reg1);
	printf("%d:  ds = %x\n", round, reg2);
	printf("%d:  es = %x\n", round, reg3);
	printf("%d:  ss = %x\n", round, reg4);
	++round;
}

void panic(const char *msg)
{
	printf("*** System panic: %s\n", msg);
	print_stack_trace();
	printf("***\n");
}

void print_stack_trace()
{
	u32 *ebp, *eip;

	asm volatile ("mov %%ebp, %0" : "=r" (ebp));
	while (ebp)
	{
		eip = ebp + 1;
		printf("   [0x%x] %s\n", *eip, elf_lookup_symbol(*eip, &kernel_elf));
		ebp = (u32*)*ebp;
	}
}




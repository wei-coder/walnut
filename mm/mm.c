
#include <kio.h>
#include <string.h>
#include <system.h>
#include "../entry/kern_debug.h"
#include "../entry/trap_gate.h"
#include "../entry/pm.h"
#include "../util/logging.h"
#include "../task/sched.h"
#include "memory.h"
#include "heap.h"

// �ں�δ������ҳ����ǰ�ݴ��ָ��
multiboot_t *glb_mboot_ptr;

/*ʹ��linux0.11�ķ����������ֽ�����������ڴ�ռ��*/
static u8* mem_map = NULL;


/*��¼ʵ�ʹ��������ҳ����*/
u32 page_count = 0;

/*��¼ʵ�ʹ��������ҳ����ߵ�ַ*/
u32 end_addr = 0;

/*��¼ʵ�ʹ�������ҳ����ʼ��ַ*/
u32 start_addr = 0;

/*�ں�ȫ��ҳĿ¼��*/
pdt_t* pdt = NULL;


#define invalidate() asm volatile("mov %0,%%cr3"::"r" (current->pdt))

// ����1 ҳ�ڴ棨4K �ֽڣ���
#define copy_page(from,to) \
asm volatile("cld ; rep ; movsb"::"S" (from),"D" (to),"c" (4096):)

void show_mem_map()
{
	u32 mmap_addr = glb_mboot_ptr->mmap_addr;
	u32 mmap_length = glb_mboot_ptr->mmap_length;

	printf("Memory map:\n");

	mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
	for (mmap = (mmap_entry_t *)mmap_addr; (u32)mmap < mmap_addr +mmap_length; mmap++)
	{
		printf("base_addr = 0x%X%08X, length = 0x%X%08X, type = 0x%X\n",(u32)mmap->base_addr_high , (u32)mmap->base_addr_low ,
		(u32)mmap->length_high , (u32)mmap->length_low , (u32)mmap->type);
	}
}

void init_pmm()
{
	/*grub̽����������ڴ汣����mmap_entry_t���У�����׵�ַΪmultiboot�ṹ���mmap_addr
	�ṹ�壬�������Ϊmmap_length.  �˴�Ҫ����mmap_entry_t��������ȡ�������ڴ����Ϣ
	��ҳ����г�ʼ����*/
	mmap_entry_t* mmap_entry = (mmap_entry_t*)(glb_mboot_ptr->mmap_addr);
	u32 mmap_length = glb_mboot_ptr->mmap_length;

	while(mmap_length-- )
	{	
		/*type = 1��ʾ�ǿ��������ڴ�*/
		if((mmap_entry->type == 1) && (NOMAL_MEM_ADDR == mmap_entry->base_addr_low))
		{
			/*mem_map����kern_end֮��*/
			mem_map = (u8*)kern_end;
			/*����ҳ����*/
			page_count = mmap_entry->length_low/PAGE_SIZE;
			/*mem_mapռ�õ�����ҳ����*/
			int map_size = page_count/PAGE_SIZE;
			if(page_count%PAGE_SIZE)
			{
				map_size ++;
			}
			/*�ɷ����ڴ��mem_map�����ĵ�ַ��ʼ����Ҫע��ҳ����*/
			start_addr = (u32)kern_end + map_size*PAGE_SIZE;

			//Ԥ�����ں�ҳĿ¼��Ŀռ䡣
			pdt = (pdt_t*)start_addr;
			start_addr += PAGE_SIZE;

			//Ԥ��������ӳ������Ӧ��ҳ��ռ�, ����ӳ����Ϊ0xc0000000~0xF8000000,�ܹ�896M.
			start_addr += PTE_COUNT*PAGE_SIZE;
			
			/*�ں˼�mem_map�Լ��ں�ҳ��ռ�õĿռ���ΪUSED*/
			int i = 0;
			for( ; i<(kern_end- kern_start)/PAGE_SIZE + map_size + PTE_COUNT + 1; i++)
			{
				mem_map[i] = USED;
			}
			printf("there are %d page used, and start_addr is 0x%08x\n",i, start_addr);

			for(; i<page_count; i++)
			{
				/*�ں�ӳ��֮��������ڴ棬���ΪFREE*/
				mem_map[i] = FREE;
			}
		}
		mmap_entry++;
	}
	logging("init phyisc memory success!\n");
}

/*�˺�������ҳ�������ַ�ͷ�һ������ҳ*/
void free_page(u32 addr )
{
	//�����Ե�ַת��Ϊ�����ַ��phy_min��ʾ�ɷ��������ڴ����С��ַ��
	u32 phy_min = start_addr - PAGE_OFFSET;
	//phy_start�ǿɹ���������ڴ����ʼ��ַ����0x100000���ϵ�ַ�ڹ���Χ�ڣ�
	//����1M�������ڴ治�ڹ���Χ��
	u32 phy_start = NOMAL_MEM_ADDR;
	
	if(phy_min > addr)
	{
		return;
	}
	addr -= phy_start;
	addr >>= 12;

	if(mem_map[addr]--)
	{
		return;
	};

	mem_map[addr] = FREE;
	printf("fault: trying to free free page!\n");
}

/*�˺�������һ������ҳ�������ַ*/
void* alloc_page()
{
	u32 ret = NULL;

	int i = 0;
	for(i = 0; i < page_count; i++)
	{
		if(mem_map[i] == FREE) 
		{
			ret = NOMAL_MEM_ADDR + i*PAGE_SIZE;
			mem_map[i] ++;
			return (void *)ret;
		}
	}
	return (void *)ret;
}

#if 1

/*ȱҳ�쳣�Ĵ�����*/
void do_no_page(u32 error_code, u32 address)
{
	if(!(error_code & 0x1))
	{
		return;
	}
	/*ҳ����*/
	address &= 0xfffff000;
	u32 page = (u32)alloc_page();
	map(pdt,address,page,PAGE_FLAG);
	return;
}

/*ȡ��д����������*/
void un_wp_page(u32 * table_entry)
{
	u32 old_page,new_page;

	old_page = 0xfffff000 & *table_entry;
	old_page  += PAGE_OFFSET;
	if (old_page >= start_addr && mem_map[MAP_INDEX(old_page)]==1)
	{
		//���ҳ�����Ѿ����ڣ����޸Ŀ�дȨ�޼���
		*table_entry |= 2;
		invalidate();
		return;
	}

	///�����ҳ�ĵ�ַС��0x100000�����߸�ҳ��δ��ʹ��
	//������һ������ҳ�����޸Ķ�Ӧ����
	new_page = (u32)alloc_page();
	if (!new_page)
	{
		printf("there is no free page;\n");
	}
// ���ԭҳ������ڴ�Ͷˣ�����ζ��mem_map[]>1��ҳ���ǹ���ģ�����ԭҳ���ҳ��ӳ��
// ����ֵ�ݼ�1��Ȼ��ָ��ҳ�������ݸ���Ϊ��ҳ��ĵ�ַ�����ÿɶ�д�ȱ�־(U/S, R/W, P)��
// ˢ��ҳ�任���ٻ��塣���ԭҳ�����ݸ��Ƶ���ҳ�档    
	if (old_page >= start_addr)
	{
		mem_map[MAP_INDEX(old_page)]--;
	}
	new_page += PAGE_OFFSET;
	copy_page(old_page,new_page);

	new_page -= PAGE_OFFSET;
	*table_entry = new_page | 7;
	invalidate();
}

/*д�����쳣������*/
void do_wp_page(u32 error_code,u32 address)
{
	pdt_t* tmp_pdt = (pdt_t*)((u32)current->pdt + PAGE_OFFSET);
	pte_t* pte = (pte_t*)(tmp_pdt[PDT_INDEX(address)]);
	pte = (pte_t*)(((u32)pte & 0xFFFFF000) + PTE_INDEX(address) * 4 + PAGE_OFFSET);
	un_wp_page(pte);
}

void * get_virt_page()
{
	u32 page = (u32)alloc_page();
	if(NULL != page)
	{
		return (void*)(page+PAGE_OFFSET);
	}
	return NULL;
}

void free_virt_page(u32 p)
{
	free_page(p-PAGE_OFFSET);
}

#endif

void init_vmm()
{
	pte_t* pte = (pte_t*)((u32)pdt+PAGE_SIZE);
	/*���߶��ڴ����µ�����ҳӳ�䵽���Կռ�*/
	int i = 0;
	int j = 0;
	int k = 0;
	for(i=KERN_PDT_INDEX; i < KERN_PDT_INDEX + PTE_COUNT; i++)
	{
		/*pte����PDT����*/
		pdt[i] = ((u32)pte - PAGE_OFFSET) | PDT_FLAG;

		/*���ں˿ռ�ӳ�䵽0xC000 0000 ~0xF800 0000*/
		for(j = 0; j<PTE_LEN; j++)
		{
			/*�������ַ0x0~0x38000000ӳ�䵽0xC000 0000 ~0xF800 0000*/
			pte[j] =  (k << 12) | PAGE_FLAG;
			k++;
		}
		pte = (pte_t*)((u32)pte + PAGE_SIZE);
	}

	set_trap_gate(14,&page_fault);
	
	u32 pdt_phy_addr = (u32)pdt - PAGE_OFFSET;
	asm volatile ("mov %0,%%cr3" : : "r" (pdt_phy_addr));
	logging("init virtual memory success!\n");
};

void map(pdt_t* pdt_now, u32 va, u32 pa, u32 flags)
{ 	
	u32 pdt_idx = PDT_INDEX(va);
	u32 pte_idx = PTE_INDEX(va); 
	
	pte_t *pte = (pte_t *)((u32)pdt_now[pdt_idx] & PAGE_MASK);
	
	if (!pte)
	{
		/*ҳ��Ϊ�գ�������һ������ҳ��Ϊҳ��*/
		pte = (pte_t *)alloc_page();
		/*ʹҳĿ¼ָ��ҳ��*/
		pdt_now[pdt_idx] = (pdt_t)((u32)pte | PDT_FLAG);

		memset((void*)((u32)pte + PAGE_OFFSET), 0, PAGE_SIZE);
	}
	else
	{
		// ת�����ں����Ե�ַ
		pte = (pte_t *)((u32)pte + PAGE_OFFSET);
	}

	pte[pte_idx] = (pte_t)((pa & PAGE_MASK) | flags);
	// ֪ͨ CPU ����ҳ����
	asm volatile ("invlpg (%0)" : : "a" (va));
}

void unmap(pdt_t * pdt_now, u32 va)
{
	u32 pgd_idx = PDT_INDEX(va);
	u32 pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)((u32)pdt_now[pgd_idx] & PAGE_MASK);

	if (!pte)
	{
		return;
	}

	// ת�����ں����Ե�ַ
	pte = (pte_t *)((u32)pte + PAGE_OFFSET);

	pte[pte_idx] = (pte_t)0;

	// ֪ͨ CPU ����ҳ����
	asm volatile ("invlpg (%0)" : : "a" (va));
}

u32 get_mapping(pdt_t *pdt_now, u32 va, u32 *pa)
{
	u32 pgd_idx = PDT_INDEX(va);
	u32 pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)((u32)pdt_now[pgd_idx] & PAGE_MASK);
	if (!pte)
	{
		return 0;
	}
	
	// ת�����ں����Ե�ַ
	pte = (pte_t *)((u32)pte + PAGE_OFFSET);

	// �����ַ��Ч����ָ�벻ΪNULL���򷵻ص�ַ
	if ((u32)pte[pte_idx] != 0 && pa)
	{
		*pa = (u32)pte[pte_idx] & PAGE_MASK;
		return 1;
	}

	return 0;
}

int copy_page_tables (u32* from_pdt, u32* to_pdt)
{
	u32* from_pte = NULL;
	u32* to_pte = NULL;
	u32 this_page = 0;
	
	if((NULL == from_pdt) ||(NULL == to_pdt))
	{
		return FALSE;
	}

	//�ȸ���PDT���Լ�PTE��
	int i = 0;
	int j = 0;
	for(i = 0; i < PDT_LEN; i++)
	{
		if(from_pdt[i] & 1)
		{
			to_pte = alloc_page();
			//copy������ҳ��
			to_pdt[i] = (u32)to_pte | PAGE_FLAG;
			from_pte = (u32*)((from_pdt[i]&PAGE_MASK)+PAGE_OFFSET);
			to_pte = (u32*)((u32)to_pte + PAGE_OFFSET);
			for(j=0; j<PTE_LEN; j++)
			{
				this_page = from_pte[j];
				if(this_page & 1)
				{
					if(this_page > NOMAL_MEM_ADDR)
					{
						this_page &= ~2;
					}
					to_pte[j] = this_page;
					if(from_pte[j] > start_addr)
					{
						from_pte[j] = this_page;
						mem_map[MAP_INDEX(this_page)]++;
					}
				}
			}
		}
	}

	invalidate();
	return true;
}

int free_page_tables(u32 pdt_addr)
{
	u32* proc_pdt = (u32*)(pdt_addr + PAGE_OFFSET);

	if(NULL == proc_pdt) 
	{
		return FALSE;
	}

	//�ͷ�ҳ��
	int i = 0;
	for(; i < PDT_LEN; i++)
	{
		if(proc_pdt[i] & 0x01)
		{
			free_page(proc_pdt[i]&PAGE_MASK);
		}
	}

	free_page(pdt_addr);

	invalidate(); 
	return 0;
}

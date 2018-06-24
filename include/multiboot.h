/*
filename:	multiboot.h
���ļ�������GRUB��ص����ݽṹ����Ҫ������hurlex��ʵ��
*/

#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H


#include "types.h"

/**
 * ��������32λ�ں˽���㣬����״̬���£�
 *   1. CS ָ�����ַΪ 0x00000000���޳�Ϊ4G �C 1�Ĵ������������
 *   2. DS��SS��ES��FS �� GS ָ�����ַΪ0x00000000���޳�Ϊ4G �C 1�����ݶ���������
 *   3. A20 ��ַ���Ѿ��򿪡�
 *   4. ҳ���Ʊ���ֹ��
 *   5. �жϱ���ֹ��
 *   6. EAX = 0x2BADB002
 *   7. ϵͳ��Ϣ��������Ϣ������Ե�ַ������ EBX�У��൱��һ��ָ�룩��
 *      ���¼�Ϊ�����Ϣ��Ľṹ
 */

typedef struct multiboot_t
{
	u32 flags;			// Multiboot �İ汾��Ϣ
	/** 
	 * �� BIOS ��֪�Ŀ����ڴ�
	 *
	 * mem_lower��mem_upper�ֱ�ָ���˵Ͷ˺͸߶��ڴ�Ĵ�С����λ��K��
	 * �Ͷ��ڴ���׵�ַ��0���߶��ڴ���׵�ַ��1M��
	 * �Ͷ��ڴ��������ֵ��640K��
	 * �߶��ڴ��������ֵ�����ֵ��ȥ1M����������֤�����ֵ��
	 */
	u32 mem_lower;
	u32 mem_upper;

	u32 boot_device;				// ָ������������ĸ�BIOS�����豸�����OSӳ��
	u32 cmdline;					// �ں�������
	u32 mods_count;				// boot ģ���б�
	u32 mods_addr;
	
	/**
	 * ELF ��ʽ�ں�ӳ���sectionͷ��
	 * ����ÿ��Ĵ�С��һ���м����Լ���Ϊ�����������ַ�����
	 */
	u32 num;
	u32 size;
	u32 addr;
	u32 shndx;

	/**
	 * ��������ָ��������BIOS�ṩ���ڴ�ֲ��Ļ������ĵ�ַ�ͳ���
	 * mmap_addr�ǻ������ĵ�ַ��mmap_length�ǻ��������ܴ�С
	 * ��������һ�����߶������Ĵ�С/�ṹ�� mmap_entry_t ���
	 */
	u32 mmap_length;		
	u32 mmap_addr;
	
	u32 drives_length; 			// ָ����һ���������ṹ�������ַ	
	u32 drives_addr; 			// ָ����һ������������ṹ�Ĵ�С
	u32 config_table; 			// ROM ���ñ�
	u32 boot_loader_name; 		// boot loader ������
	u32 apm_table; 	    			// APM ��
	u32 vbe_control_info;
	u32 vbe_mode_info;
	u32 vbe_mode;
	u32 vbe_interface_seg;
	u32 vbe_interface_off;
	u32 vbe_interface_len;
} __attribute__((packed)) multiboot_t;

/**
 * size����ؽṹ�Ĵ�С����λ���ֽڣ������ܴ�����Сֵ20
 * base_addr_low��������ַ�ĵ�32λ��base_addr_high�Ǹ�32λ��������ַ�ܹ���64λ
 * length_low���ڴ������С�ĵ�32λ��length_high���ڴ������С�ĸ�32λ���ܹ���64λ
 * type����Ӧ��ַ��������ͣ�1�������RAM������������ֵ����������
 */
typedef struct mmap_entry_t {
	u32 size; 		// ���� size �ǲ��� size ��������Ĵ�С
	u32 base_addr_low;
	u32 base_addr_high;
	u32 length_low;
	u32 length_high;
	u32 type;
} __attribute__((packed)) mmap_entry_t;

// ����ȫ�ֵ� multiboot_t * ָ��
// �ں�ҳ�������ָ��
extern multiboot_t *mboot_ptr_tmp;

#endif

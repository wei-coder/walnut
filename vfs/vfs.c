/*
filename: vfs.c
author:wei-coder
time:  2018-09-12
purpose: 虚拟文件系统的实现文件
*/

#include "vfs.h"

sb_t * g_sblk;

void init_vfs()
{
}

int register_filesystem(struct file_system_type * fs)
{
	sb_t * new_sb = kmalloc(sizeof(sb_t));
	if(NULL == new_sb)
	{
		logging("[ERROR]: memory alloc for super block failed!");
		return -1;
	}
	memset(new_sb,0,sizeof(sb_t));
	struct list_head * tail = getlast_lh((struct list_head *)g_sblk);
	if(NULL == tail)
	{
		g_sblk = new_sb;
	}
	else
	{
		tail->next = new_sb->s_list;
		new_sb->s_list->prev = tail;
	}
	new_sb->s_type = fs;
	new_sb->s_type->read_super(new_sb, new_sb->s_type->name, new_sb->s_type->fs_flag);
}

struct file_system_type * find_filesystem(char * fs_name, int name_len)
{
	struct file_system_type * tmp = g_sblk->s_type;
	while(NULL != tmp)
	{
		if(0 == strcmp(fs_name, tmp->name))
		{
			return tmp;
		}
		tmp = (struct file_system_type *)(tmp->s_list->next);
	}
	return NULL;
}

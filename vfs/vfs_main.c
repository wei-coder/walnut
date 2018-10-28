/*
filename: vfs_main.c
author:wei-coder
time:  2018-09-12
purpose: 虚拟文件系统的实现文件
*/

#include "vfs.h"

sb_t * g_sblk;
fs_type_t * g_fslist;
dentry_t * g_dentry;

int sys_mount(char * dev_name, char * dir_name, char * type, unsigned long flags, void * data)
{
	dentry_t* s_dentry = vfs_get_dentry(dev_name);
	if(NULL == s_dentry)
	{
		return VFS_FAIL;
	}
	dentry_t * d_dentry = vfs_get_dentry(dir_name);
	if(NULL == d_dentry)
	{
		return VFS_FAIL;
	}
	s_dentry->d_sb->s_type->mount(d_dentry->d_sb->s_type, flags, dir_name, data);
	return VFS_OK;
}

int sys_umount(const char * specialfile)
{
	return 0;
}

int sys_mknod(const char * filename, mode_t mode, dev_t dev)
{
	return 0;
}

int sys_chmod(const char * filename, mode_t mode)
{
	return 0;
}

int sys_chown(const char * filename, uid_t owner, gid_t group)
{
	return 0;
}

int sys_link(const char * filename1, const char * filename2)
{
	return 0;
}

int sys_unlink(const char * filename)
{
	return 0;
}

int sys_chdir(const char * filename)
{
	return 0;
}

int sys_mkdir(const char *pathname, mode_t mode)
{
	return 0;
}

int sys_rmdir(const char *pathname)
{
	return 0;
}

inode_t *find_inode(u32 inode_no)
{
	sb_t * tmp_sblk = g_sblk;
	while(tmp_sblk)
	{
		inode_t * tmp_inode = (inode_t *)(tmp_sblk->s_inodes);
		while(tmp_inode)
		{
			if(tmp_inode->i_ino == inode_no)
			{
				return tmp_inode;
			}
			tmp_inode = (inode_t *)(tmp_inode->i_list->next);
		}
		tmp_sblk = (sb_t *)(tmp_sblk->s_list->next);
	}
	return NULL;
}


int register_filesystem(fs_type_t* fs)
{
	struct list_head * tail = getlast_lh(g_fslist->s_list);
	if(NULL == tail)
	{
		g_fslist = fs;
		fs->s_list->next = NULL;
		fs->s_list->prev = NULL;
	}
	else
	{
		if(NULL == find_filesystem(fs->name, 0))
		{
			tail->next = fs->s_list;
			fs->s_list->prev = tail;
			fs->s_list->next = NULL;
		}
	}
	return KER_SUC;
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

void init_fs()
{

}

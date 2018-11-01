/*
filename: vfs_main.c
author:wei-coder
time:  2018-09-12
purpose: 虚拟文件系统的实现文件
*/

#include "vfs.h"

sb_t * g_sblk;
fs_type_t * g_fslist;
vfsmount_t * g_vfsmount;

int search_subdir(path_t * root, char * dir)
{
	ulong flag = 0;
	dentry_t * d_root = root->p_dentry;
	struct list_head * sub_dir = d_root->d_subdirs.next;
	while(NULL != sub_dir)
	{
		if(0 == strcmp(VFS_CHILD_DENTRY(sub_dir)->d_iname, dir))
		{
			flag = VFS_CHILD_DENTRY(sub_dir)->d_flags;
			if(DENTRY_FLAG_MNT == flag&(~DENTRY_FLAG_MNT))
			{
				root->p_vmount = VFS_CHILD_MOUNTS(root->p_vmount->m_mount.next);
				root->p_dentry = root->p_vmount->m_root;
			}
			else
			{
				root->p_dentry = (dentry_t *)sub_dir;
			}
			return VFS_OK;
		}
		sub_dir = sub_dir->next;
	}
	return VFS_FAIL;
}

int vfs_get_path(char * dir, path_t * cpath)
{
	char sub_dir[DNAME_LEN_MAX] = NULL;
	if('/' == *dir)
	{
		cpath->p_vmount = g_vfsmount;
		cpath->p_dentry = g_vfsmount->m_root;
	}
	else
	{
		cpath->p_vmount = current->path->p_vmount;
		cpath->p_dentry = current->path->p_dentry;
	}
	char * sub_dir = strtok(dir,"/");
	while(NULL != sub_dir)
	{
		if(VFS_FAIL == search_subdir(cpath,sub_dir))
		{
			return VFS_FAIL;
		}
		sub_dir = strtok(NULL, "/");
	}
	return VFS_OK;
}

int sys_mount(char * dev_name, char * dir_name, char * type, unsigned long flags, void * data)
{
	dentry_t * root = NULL;
	path_t * s_path = vfs_get_path(dev_name);
	if(NULL == s_path)
	{
		return VFS_FAIL;
	}
	path_t * d_path = vfs_get_path(dir_name);
	if(NULL == d_path)
	{
		return VFS_FAIL;
	}
	
	vfsmount_t * new_mnt = kmalloc(sizeof(vfsmount_t));
	if(NULL == new_mnt)
	{
		return VFS_FAIL;
	}
	new_mnt->m_parent = d_path->p_vmount;
	new_mnt->m_mntpoint = d_path->p_dentry;
	new_mnt->m_root = s_path->p_dentry->d_sb->s_type->mount(d_path->p_dentry->d_sb->s_type, flags, dir_name, data);
	new_mnt->m_sb = new_mnt->m_root->d_sb;
	struct list_head * tail =  getlast_lh(&(d_path->p_vmount->m_mount));
	tail->next = &(new_mnt->m_child);
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

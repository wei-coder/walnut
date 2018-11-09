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

vfsmount_t * search_vfsmnt(char * dir)
{
	vfsmount_t * vmnt = g_vfsmount;
	vfsmount_t * sub_vmnt = VFS_CHILD_MOUNTS(vmnt->m_mount.next);
	dlist_t * pdlist = create_dlist(NULL,NULL);
	while(sub_vmnt)
	{		if(0 == strcmp(sub_vmnt->m_mntpoint->d_iname, dir))
		{			pdlist->destroy(pdlist);
			return sub_vmnt;
		}

		if(NULL != sub_vmnt->m_child.next)
		{			lnode_t * tmp_node1 = kmalloc(sizeof(lnode_t));
			tmp_node1->pNode = (void *)(VFS_CHILD_MOUNTS(sub_vmnt->m_child.next));
			pdlist->push(pdlist->head, tmp_node1);
		}

		if(NULL != sub_vmnt->m_mount.next)
		{			lnode_t * tmp_node2 = kmalloc(sizeof(lnode_t));
			tmp_node2->pNode = (void *)(VFS_CHILD_MOUNTS(sub_vmnt->m_mount.next));
			pdlist->push(pdlist->head, tmp_node2);
		}

		sub_vmnt = (vfsmount_t *)pdlist->stack_pop(&(pdlist->head));
	}
	free(pdlist);
	return NULL;
}

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
	path_t s_path = {0};
	if(VFS_FAIL == vfs_get_path(dev_name, &s_path))
	{
		return VFS_FAIL;
	}
	path_t d_path = {0};
	if(VFS_FAIL == vfs_get_path(dir_name, &d_path))
	{
		return VFS_FAIL;
	}
	
	vfsmount_t * new_mnt = kmalloc(sizeof(vfsmount_t));
	if(NULL == new_mnt)
	{
		return VFS_FAIL;
	}
	new_mnt->m_parent = d_path.p_vmount;
	new_mnt->m_mntpoint = d_path.p_dentry;
	new_mnt->m_root = s_path.p_dentry->d_sb->s_type->mount(d_path.p_dentry->d_sb->s_type, flags, dir_name, data);
	new_mnt->m_sb = new_mnt->m_root->d_sb;
	struct list_head * tail =  getlast_lh(&(d_path.p_vmount->m_mount));
	tail->next = &(new_mnt->m_child);
	new_mnt->m_child.prev = tail;
	new_mnt->m_child.next = NULL;
	return VFS_OK;
}

void destroy_dentry(dentry_t * pdentry)
{
	free(pdentry);
}

void destroy_vfsmnt(vfsmount_t * vmnt)
{
	dentry_t * root = vmnt->m_root;
	vmnt->m_child.prev->next = vmnt->m_child.next;
	if(NULL != vmnt->m_child.next)
	{
		vmnt->m_child.next->prev = vmnt->m_child.prev;
	}
	free(vmnt);
	vmnt = NULL;

	dentry_t * sub_dentry = VFS_CHILD_DENTRY(root->d_subdirs.next);
	dlist_t * pdlist = create_dlist(NULL,NULL);
	while(sub_dentry)
	{		if(NULL != sub_dentry->d_child.next)
		{			lnode_t * tmp_node1 = kmalloc(sizeof(lnode_t));
			tmp_node1->pNode = (void *)(VFS_CHILD_DENTRY(sub_dentry->d_child.next));
			pdlist->push(pdlist->head, tmp_node1);
		}

		if(NULL != sub_dentry->d_subdirs.next)
		{			lnode_t * tmp_node2 = kmalloc(sizeof(lnode_t));
			tmp_node2->pNode = (void *)(VFS_CHILD_MOUNTS(sub_dentry->d_subdirs.next));
			pdlist->push(pdlist->head, tmp_node2);
		}

		if((NULL == sub_dentry->d_child.next)&&(NULL != sub_dentry->d_subdirs.next))
		{
			destroy_dentry(sub_dentry);
		}

		sub_dentry = (vfsmount_t *)(pdlist->stack_pop(&(pdlist->head)));
	}
	free(pdlist);
	destroy_dentry(root);
}

int sys_umount(const char * specialfile)
{
	vfsmount_t * tmp_vmnt = search_vfsmnt(specialfile);
	tmp_vmnt->m_mntpoint->d_flags &= (~DENTRY_FLAG_MNT);
	tmp_vmnt->m_child.prev->next = tmp_vmnt->m_child.next;
	tmp_vmnt->m_child.next->prev = tmp_vmnt->m_child.prev;
	vfsmount_t * sub_vmnt = VFS_CHILD_MOUNTS(tmp_vmnt->m_mount.next);
	dlist_t * pdlist = create_dlist(NULL,NULL);
	while(sub_vmnt)
	{		if(NULL != sub_vmnt->m_child.next)
		{			lnode_t * tmp_node1 = kmalloc(sizeof(lnode_t));
			tmp_node1->pNode = (void *)(VFS_CHILD_MOUNTS(sub_vmnt->m_child.next));
			pdlist->push(pdlist->head, tmp_node1);
		}

		if(NULL != sub_vmnt->m_mount.next)
		{			lnode_t * tmp_node2 = kmalloc(sizeof(lnode_t));
			tmp_node2->pNode = (void *)(VFS_CHILD_MOUNTS(sub_vmnt->m_mount.next));
			pdlist->push(pdlist->head, tmp_node2);
		}

		if((NULL == sub_vmnt->m_child.next)&&(NULL != sub_vmnt->m_mount.next))
		{
			destroy_vfsmnt(sub_vmnt);
		}

		sub_vmnt = (vfsmount_t *)(pdlist->stack_pop(&(pdlist->head)));
	}
	free(pdlist);
	destroy_vfsmnt(tmp_vmnt);
	return 0;
}

int sys_mknod(const char * filename, mode_t mode, dev_t dev)
{
	
	return 0;
}

int sys_chmod(const char * filename, mode_t mode)
{
	path_t targ_path = {0};
	if(VFS_FAIL == search_subdir(filename, &targ_path))
	{
		return -1;
	}
	targ_path.p_dentry.d_inode->i_mode = mode;
	return 0;
}

int sys_chown(const char * filename, uid_t owner, gid_t group)
{
	path_t targ_path = {0};
	if(VFS_FAIL == search_subdir(filename, &targ_path))
	{
		return -1;
	}
	targ_path.p_dentry.d_inode->i_uid = owner;
	targ_path.p_dentry.d_inode->i_gid = group;
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
	path_t * here = current->path;
	dentry_t * new_dentry = (dentry_t*)kmalloc(sizeof(dentry_t));
	if(NULL == new_dentry)
	{
		logging("[ERROR]kmalloc new dentry.\r\n");
		return -1;
	}
	new_dentry->d_parent = here->p_dentry;
	
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

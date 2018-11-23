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

char * get_name_from_path(const char * pathname)
{
	char * name = NULL;
	int path_len = strlen(pathname);
	for(int i=path_len-1; i>=0; i--)
	{
		if('/' == pathname[i])
		{
			name = pathname+i+1;
			return name;
		}
	}
	return pathname;
}

sb_t * sget(struct file_system_type * fstype)
{
	if(NULL == fstype)
	{
		return NULL;
	}
	sb_t * s = g_sblk;
	do
	{
		if(strcmp(fstype->name,s->s_type->name) == 0)
		{
			return s;
		}
		s = (sb_t *)(s->s_list.next);
	}
	while(s != g_sblk)
	return NULL;
}

vfsmount_t * search_vfsmnt(char * dir)
{
	vfsmount_t * vmnt = g_vfsmount;
	vfsmount_t * sub_vmnt = VFS_CHILD_MOUNTS(vmnt->m_mount.next);
	dlist_t * pdlist = create_dlist(NULL,NULL);
	while(sub_vmnt)
	{
		if(0 == strcmp(sub_vmnt->m_mntpoint->d_iname, dir))
		{
			pdlist->destroy(pdlist);
			return sub_vmnt;
		}

		if(NULL != sub_vmnt->m_child.next)
		{
			lnode_t * tmp_node1 = kmalloc(sizeof(lnode_t));
			tmp_node1->pNode = (void *)(VFS_CHILD_MOUNTS(sub_vmnt->m_child.next));
			pdlist->push(pdlist->head, tmp_node1);
		}

		if(NULL != sub_vmnt->m_mount.next)
		{
			lnode_t * tmp_node2 = kmalloc(sizeof(lnode_t));
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

int vfs_get_path(char * dir, path_t * cpath, int flag)
{
	int dir_len = 0;
	char * pointer = dir;
	if('/' == *dir)
	{
		pointer += 1;
		cpath->p_vmount = g_vfsmount;
		cpath->p_dentry = g_vfsmount->m_root;
	}
	else
	{
		cpath->p_vmount = current->pwd.p_vmount;
		cpath->p_dentry = current->pwd.p_dentry;
	}
	char catalog[DNAME_LEN_MAX] = {0};
	char * sub_dir = strchar(pointer, "/");
	if(NULL == sub_dir)
	{
		dir_len = strlen(pointer);
	}
	else
	{
		dir_len = (int)(sub_dir-pointer);
	}
	strncpy(catalog,pointer,dir_len);
	while(NULL != catalog)
	{
		if(VFS_FAIL == search_subdir(cpath,catalog))
		{
			if(GET_PATH_FLAG_MEM == flag)
			{
				return VFS_FAIL;
			}
			else
			{
				dentry_t * td = NULL;
				inode_t * tn = (inode_t *)kmalloc(sizeof(inode_t));
				memset(td,0,sizeof(dentry_t));
				dir_len = strlen(pointer);
				strncpy(td->d_iname,catalog,dir_len<DNAME_LEN_MAX?dir_len:(DNAME_LEN_MAX-1));
				if(td = cpath->p_dentry->d_inode->i_op->lookup(tn,sub_dir))
				{
					cpath->p_dentry = td;
					return VFS_OK;
				}
			}
		}
		pointer = sub_dir;
		if(NULL == pointer)
		{
			break;
		}
		sub_dir = strchar(pointer, "/");
		memset(catalog,0,DNAME_LEN_MAX);
		strncpy(catalog,pointer,(size_t)(sub_dir-pointer));
	}
	return VFS_OK;
}

int sys_mount(char * dev_name, char * dir_name, char * type, unsigned long flags, void * data)
{
	dentry_t * root = NULL;
	path_t s_path = {0};
	if(VFS_FAIL == vfs_get_path(dev_name, &s_path, GET_PATH_FLAG_MEM))
	{
		return VFS_FAIL;
	}
	path_t d_path = {0};
	if(VFS_FAIL == vfs_get_path(dir_name, &d_path, GET_PATH_FLAG_MEM))
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
	struct file_system_type * fstype = find_filesystem(type);
	if(NULL == fstype)
	{
		kfree(new_mnt);
		return VFS_FAIL;
	}
	new_mnt->m_root = fstype->mount(fstype, flags, dir_name, data);
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
	{
		if(NULL != sub_dentry->d_child.next)
		{
			lnode_t * tmp_node1 = kmalloc(sizeof(lnode_t));
			tmp_node1->pNode = (void *)(VFS_CHILD_DENTRY(sub_dentry->d_child.next));
			pdlist->push(pdlist->head, tmp_node1);
		}

		if(NULL != sub_dentry->d_subdirs.next)
		{
			lnode_t * tmp_node2 = kmalloc(sizeof(lnode_t));
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
	{
		if(NULL != sub_vmnt->m_child.next)
		{
			lnode_t * tmp_node1 = kmalloc(sizeof(lnode_t));
			tmp_node1->pNode = (void *)(VFS_CHILD_MOUNTS(sub_vmnt->m_child.next));
			pdlist->push(pdlist->head, tmp_node1);
		}

		if(NULL != sub_vmnt->m_mount.next)
		{
			lnode_t * tmp_node2 = kmalloc(sizeof(lnode_t));
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
	inode_t * pInode = (inode_t *)kmalloc(sizeof(inode_t));
	dentry_t * pDentry = (dentry_t *)kmalloc(sizeof(dentry_t));
	pDentry->d_inode = pInode;
	path_t path = {0};
	vfs_get_path(filename1,&path, GET_PATH_FLAG_MEM)
	return current->pwd.p_dentry->d_inode->i_op->link(path.p_dentry,pInode,pDentry);
}

int sys_unlink(const char * filename)
{
	path_t path = {0};
	vfs_get_path(filename,&path, GET_PATH_FLAG_MEM);
	return path.p_dentry->d_inode->i_op->unlink(path.p_dentry->d_inode, path.p_dentry);
}

int sys_chdir(const char * filename)
{
	path_t path = {0};
	vfs_get_path(filename,&path, GET_PATH_FLAG_MEM);
	current->pwd.p_dentry = path.p_dentry;
	current->pwd.p_vmount = path.p_vmount;
	return 0;
}

int sys_mkdir(const char *pathname, int mode)
{
	path_t path = {0};
	if(vfs_get_path(pathname,&path,GET_PATH_FLAG_ALL) == VFS_FAIL)
	{
		return -1;
	}
	dentry_t * pDentry = (dentry_t *)kmalloc(sizeof(dentry_t));
	if(NULL == pDentry)
	{
		logging("kmalloc dentry is failed!\r\n");
		return -1;
	}
	memset(pDentry,0,sizeof(dentry_t));
	char * dirname = get_name_from_path(pathname);
	int namelen = strlen(dirname);
	strncpy(pDentry->d_iname,dirname,(namelen>DNAME_LEN_MAX?DNAME_LEN_MAX:namelen));
	pDentry->d_flags = mode;
	pDentry->d_parent = path.p_dentry;
	pDentry->d_op = path.p_dentry->d_op;
	pDentry->d_sb = path.p_dentry->d_sb;
	pDentry->d_time = time(0);
	push_ring_list(&(path.p_dentry->d_subdirs),&(pDentry->d_child));
	init_ring_list_head(&(pDentry->d_subdirs));
	return current->pwd.p_dentry->d_inode->i_op->mkdir(path.p_dentry->d_inode,pDentry,mode);
}

int sys_rmdir(const char *pathname)
{
	path_t path = {0};
	if(VFS_FAIL == vfs_get_path(pathname,&path, GET_PATH_FLAG_ALL))
	{
		return -1;
	}
	return path.p_dentry->d_inode->i_op->rmdir(path.p_dentry->d_inode, path.p_dentry);
}

int sys_rename(const char *oldpath, const char * newpath)
{
	path_t path = {0};
	if(VFS_FAIL == vfs_get_path(oldpath, &path,GET_PATH_FLAG_ALL))
	{
		logging("get %s path for rename is failed!/r/n",oldpath);
		return -1;
	}

	char * newname = get_name_from_path(newpath);
	int name_len = strlen(newname);
	strncpy(path.p_dentry->d_iname,newname,name_len<DNAME_LEN_MAX?newname:DNAME_LEN_MAX);
	return path.p_dentry->d_inode->i_op->rename(path.p_dentry->d_inode, path.p_dentry);
}

inode_t *find_inode(u32 inode_no)
{
	sb_t * tmp_sblk = g_sblk;
	while(tmp_sblk)
	{
		inode_t * tmp_inode = (inode_t *)(tmp_sblk->s_inodes.next);
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

struct file_system_type * find_filesystem(char * fs_name)
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

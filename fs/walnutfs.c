/*
filename:	walnutfs.c
author:	  	wei-coder
time:		2018-09-15
purpose:	walnutfs文件系统的函数实现。
*/

#include "vfs.h"
#include "walnutfs.h"

static struct super_operations wfs_op = {
	.alloc_inode = alloc_inode_wf,
	.destroy_inode = destroy_inode_wf,
	.dirty_inode = NULL,
	.write_inode = NULL,
	.delete_inode = NULL,
	.read_super = read_super_wf,
	.write_super = NULL,
};

static fs_type_t walnutfs_type = {
	.name = "walnutfs",
	.fs_flag = 0,
	.mount = wfs_mount,
	.kill_sb = wfs_kill_sb
};

static inode_operations walnutfs_inode_op = {
	.create = create_wf,
	.lookup = lookup_wf,
	.link = NULL,
	.unlink = NULL,
	.symlink = NULL,
	.mkdir = mkdir_wf,
	.rmdir = rmdir_wf,
	.mknod = NULL,
	.rename = rename_wf,
	.readlink = NULL,
	.truncate = NULL,
	.permission = NULL,
	.setattr = NULL,
	.getattr = NULL,
	.setxattr = NULL,
	.getxattr = NULL,
	.listxattr = NULL,
	.removexattr = NULL
};

static struct dentry_operations walnutfs_dentry_op = {
	.d_revalidate = NULL,
	.d_hash = NULL,
	.d_compare = NULL,
	.d_delete = wfs_deldentry,
	.d_release = NULL,
	.d_iput = NULL
};

static struct file_operations walnutfs_file_op = {
	.read = wfs_read,
	.write = wfs_write,
	.ioctl = wfs_ioctl,
	.open = wfs_open,
	.close = wfs_close,
	.lseek = wfs_seek,
	.mmap = wfs_mmap
	.flush = NULL,
	.release = NULL,
	.fsync = NULL,
	.fasync = NULL,
	.check_flags = NULL,
};

static vfsmount_t walnutfs_vfsmnt = {0};


/*super block的初始化函数，对walnutfs来说，需要从硬盘读取超级块*/
int read_super_wf(sb_t * pSblk)
{
	if(NULL == pSblk)
	{
		return KER_FAIL;
	}
	pSblk->s_dev = 0;
	pSblk->s_blocksize_bits = 8;
	pSblk->s_blocksize = 4096;
	pSblk->s_files = NULL;
	pSblk->s_inodes = NULL;
	pSblk->s_magic = WALNUT_MAGIC_NUM;
	pSblk->s_root = NULL;
	pSblk->s_op = &wfs_op;
	return KER_SUC;
}

/*super block的操作函数，用于初始化一个inode对象*/
inode_t * alloc_inode_wf(sb_t * psb)
{
	inode_t * new_inode = (inode_t*)kmalloc(sizeof(inode_t));
	if(NULL == new_inode)
	{
		return NULL;
	}
	memset(new_inode,0,sizeof(inode_t));
	new_inode->i_ctime = time(0);
	new_inode->i_blkbits = psb->s_blocksize_bits;
	new_inode->i_blocks = psb->s_blocksize;
	
	new_inode->i_ino = 0xFFFFFFFF;
	new_inode->i_count = 0;
	new_inode->i_op = &walnutfs_inode_op;
	new_inode->i_fop = &walnutfs_file_op;
	new_inode->i_mapping = NULL;
	
	push_ring_list(&(psb->s_inodes),&(new_inode->i_list));
	return new_inode;
}

/*super block用于释放inode的函数*/
void destroy_inode_wf(inode_t * pinode)
{
	if(NULL != pinode)
	{
		if(pinode->i_private)
		{
			kfree(pinode->i_private);
		}
		kfree(pinode);
	}
}

inode_t* create_wf(inode_t * dir, dentry_t * pDentry, int mode)
{
	if((NULL == dir)&&(NULL == pDentry))
	{
		return NULL;
	}
	inode_t * pinode = pDentry->d_sb->s_op->alloc_inode(pDentry->d_sb);
	pinode->i_count = 1;
	pinode->i_dirty = 1;
	pinode->i_uid = current->uid;
	pinode->i_gid = current->gid;
	pinode->i_size = 0;
	pinode->i_mode = mode;
	if(mode & F_MODE_DIR)
	{
		pinode->i_private = NULL;
	}
	else if(mode & F_MODE_FILE)
	{
		pinode->i_private = kmalloc(sizeof(struct wfs_file_data));
		struct wfs_file_data * pdata = (struct wfs_file_data *)(pinode->i_private);
		pdata->fblk_h = NULL;
		pdata->filelen = 0;
		strcpy(pdata->fname, pDentry->d_iname);
	}
	pDentry->d_inode = pinode;
	return pinode;
}

//创建目录。 1，创建inode。2，创建dentry，3，创建file。4，加入链表。
//输入：pnode：父目录inode。pDentry：本目录。mode：读写权限。
int mkdir_wf(inode_t * pNode, dentry_t * pDentry, mode_t mode )
{
	struct list_head * tmp = NULL;
	sb_t * s = pDentry->d_sb;
	inode_t * new_node = alloc_inode_wf(s);
	if(NULL == new_node)
	{
		return -1;
	}
	pDentry->d_inode = new_node;
	return 0;
}

//如果存在子目录，则将其下的全部子目录及文件删除
int rmdir_wf(inode_t * pinode, dentry_t * pdentry)
{
	dentry_t * tmpDentry = NULL;
	struct list_head * plist = pdentry->d_subdirs.prev;
	while(plist != &(pdentry->d_subdirs))
	{
		tmpDentry = VFS_CHILD_DENTRY(plist)
		plist = plist->prev;
		rmdir_wf(tmpDentry->d_inode, tmpDentry);
	}
	sb_t * s = pdentry->d_sb;
	pinode->i_list.next->prev = pinode->i_list.prev;
	pinode->i_list.prev->next = pinode->i_list.next;
	s->s_op->destroy_inode(pinode);
	pdentry->d_child.next->prev = pdentry->d_child.prev;
	pdentry->d_child.prev->next = pdentry->d_child.next;
	pdentry->d_op->d_delete(pdentry);
	return 0;
}

int rename_wf(inode_t * pinode, dentry_t * pdentry)
{
	struct wfs_file_data * pdata = (struct wfs_file_data *)(pinode->i_private);
	strcpy(pdata->fname, pdentry->d_iname);
	return 0;
}

/* 输入：1，dir 在此目录下进行查找。
		2，pdentry 查找的目标索引对应于pdentry,该dentry仅指定了名称
   输出：目标索引对应的dentry。
   对于walnutfs来说，所有的inode和dentry都在内存中，不需要该函数*/
dentry_t * lookup_wf(inode_t * dir, dentry_t * pdentry)
{
	return pdentry->d_inode;
}

dentry_t * create_dentry_wf(char * name, u32 flag)
{
	dentry_t * pDentry = kmalloc(sizeof(dentry_t));
	if(NULL == pDentry)
	{
		logging("[FATAL]:kmalloc dentry is failed!\r\n");
		return NULL;
	}
	strncpy(pDentry->d_iname, name, DNAME_LEN_MAX);
	pDentry->d_flags = flag;
	return pDentry;
}

void wfs_deldentry(dentry_t * pDentry)
{
	kfree(pDentry);
}


//从文件file偏移量offset处，读取count字节的数据到buf中
int wfs_read(file_t * pfile, char * buf, u32 count, int offset)
{
	int readsize = 0;
	struct wfs_file_data * fdata = (struct wfs_file_data *)pfile->f_dentry->d_inode->i_private;
	wfs_fblk_t * fblk = fdata->fblk_h;
	int bufsize = fblk->blksize;
	offset += pfile->f_pos;
	while((bufsize < offset)&&(NULL != fblk->next))
	{
		fblk = fblk->next;
		bufsize += fblk->blksize;
	}

	if(bufsize < offset)
	{
		return 0;
	}
	int blkoffset = (fblk->blksize - (bufsize-offset));
	
	if(fblk->blksize > (blkoffset+count))
	{
		memcpy(buf,(void *)(fblk->data+blkoffset),count);
		pfile->f_pos += count;
		return count;
	}
	else
	{
		while(count >0)
		{
			int readblksize = 0;
			if(fblk->blksize < (blkoffset+count))
			{
				readblksize = fblk->blksize-blkoffset;
			}
			else if(fblk->blksize < count)
			{
				readblksize = fblk->blksize;
			}
			else
			{
				readblksize = count;
			}
			memcpy(buf,(void *)(fblk->data+blkoffset),readblksize);
			readsize += readblksize;
			count -= readblksize;
			pfile->f_pos += readblksize;
			blkoffset =0;
			fblk = fblk->next;
			if(NULL == fblk)
			{
				return readsize;
			}
		}
	}
	return readsize;
}

//被系统调用write调用，从buf中读取数据，以file的offset为起始位置，写count字节数据
int wfs_write(file_t * pfile, char * buf, u32 count, int offset)
{
	wfs_fblk_t * cusor = NULL;
	struct wfs_file_data * pdata = (struct wfs_file_data *)(pfile->f_dentry->d_inode->i_private);
	if(pfile->f_pos + offset == pdata->filelen)
	{
		wfs_fblk_t * newblk = (wfs_fblk_t *)kmalloc(sizeof(wfs_fblk_t)+count);
		newblk->blksize = count;
		cusor = pdata->fblk_h;
		while(cusor->next)
		{
			cusor = cusor->next;
		}
		cusor->next = newblk;
		newblk->next = NULL;
		memcpy(newblk->data,buf,count);
		pfile->f_dentry->d_inode->i_blocks += count;
		pdata->filelen += count;
	}
	wfs_fblk_t * fblk = pdata->fblk_h;
	int bufsize = fblk->blksize;
	if(NULL != fblk->next)
	{
		bufsize += fblk->next->blksize;
		offset += pfile->f_pos;
		while(bufsize < offset)
		{
			fblk = fblk->next;
			if(NULL != fblk->next)
			{
				bufsize += fblk->next->blksize;
			}
			else
			{
				return -1;
			}
		}
	}
	if(bufsize < offset)
	{
		return -1;
	}
	int blkoffset = (fblk->next->blksize - (bufsize-offset));
	wfs_fblk_t * newblk = (wfs_fblk_t *)kmalloc(sizeof(wfs_fblk_t)+blkoffset+count);
	newblk->blksize = blkoffset+count;
	newblk->next = NULL;
	memcpy(newblk->data,fblk->next->data,blkoffset);
	memcpy(newblk->data+blkoffset,buf,count);
	cusor = fblk->next;
	fblk->next = newblk;
	pdata->filelen += count;
	while(cusor)
	{
		wfs_fblk_t * tmp = cusor;
		cusor = cusor->next;
		kfree(tmp);
	}
	return count;
}

/*被系统调用ioctl调用，将一个命令和一系列参数发送给一个设备。
当file是一个打开的设备节点时会使用此函数，调用者必须持有BKL(Big Kernel Lock)*/
int wfs_ioctl(inode_t * pinode, file_t * pfile, u32 cmd, ulong arg)
{
	return 0;
}

int wfs_open()
{
	return 0;
}

int wfs_close()
{
	return 0;
}

int wfs_seek()
{
	return 0;
}

int wfs_mmap(file_t * pfile, struct vm_area_struct * vma)
{
	return 0;
}

dentry_t * walnutfs_mount(struct file_system_type * fs_type, int flags, const char * fs_name, void * data)
{
	dentry_t * new_dentry = (dentry_t *)kmalloc(sizeof(dentry_t));
	if(NULL == new_dentry)
	{
		logging("[FATAL] kmalloc walnutfs new dentry failed!\r\n");
		return NULL;
	}

	memset(new_dentry,0,sizeof(dentry_t));
	int name_len = strlen(fs_name);
	strncpy(new_dentry->d_iname,fs_name, DNAME_LEN_MAX>name_len?name_len:(DNAME_LEN_MAX-1));
	sb_t * psb = sget(fs_type);
	inode_t *root_node = alloc_inode_wf(psb);
	if(NULL == root_node)
	{
		return NULL;
	}
	new_dentry->d_inode = root_node;
	new_dentry->d_mounted = ;
	new_dentry->d_flags = flags;
	new_dentry->d_parent = NULL;
	new_dentry->d_op = &walnutfs_dentry_op;
	new_dentry->d_sb = psb;
	new_dentry->d_time = time(0);
	new_dentry->d_fsdata = data;
	return new_dentry;
}

void * wfs_kill_sb(sb_t * psblk)
{
	if(list_head_del_node(psblk->s_list))
	{
		file_t * delfile = (inode_t *)(psblk->s_files);
		while(delfile)
		{
			psblk->s_files = delfile->f_list->next;
			free(delfile);
		}
		inode_t * delnode = (inode_t *)(psblk->s_inodes);
		while(delnode)
		{
			psblk->s_inodes = delnode->i_list->next;
			psblk->s_op->destroy_inode(delnode));
			delnode = (inode_t *)(psblk->s_inodes);
		}
		kfree(psblk);
	}
}

void init_walnutfs()
{
	register_filesystem(&walnutfs_type);
	g_vfsmount = (vfsmount_t *)kmalloc(sizeof(vfsmount_t));
	if(NULL == g_vfsmount)
	{
		logging("[ERROR] kmalloc vfsmount for g_vfsmount is failed!");
		return;
	}
	g_vfsmount->m_parent = NULL;
	g_vfsmount->m_root = (dentry_t *)kmalloc(sizeof(dentry_t));
	memset(g_vfsmount->m_root,0,sizeof(dentry_t));
	g_vfsmount->m_mntpoint = NULL;
	init_line_list_head(&(g_vfsmount->m_mount));
	init_line_list_head(&(g_vfsmount->m_child));

	g_sblk = (sb_t *)kmalloc(sizeof(sb_t));
	memset(g_sblk,0,sizeof(sb_t));
	g_sblk->s_type = &walnutfs_type;
	g_sblk->s_dev = 0;
	g_sblk->s_op = &walnutfs_op;
	g_sblk->s_flags = 0;
	g_sblk->s_magic = 0;
	g_sblk->s_root = g_vfsmount->m_root;
	g_sblk->s_umount = 0;
	g_sblk->s_count = 1;
	g_sblk->s_active = 1;
	g_sblk->s_fs_info = NULL;
	g_sblk->s_blocksize_bits = 8;
	g_sblk->s_blocksize = 1;
	g_sblk->s_maxbytes = 10;
	init_ring_list_head(&(g_sblk->s_inodes));

	g_vfsmount->m_sb = g_sblk;
	init_line_list_head(&(g_vfsmount->m_root->d_subdirs));
	init_line_list_head(&(g_vfsmount->m_root->d_child));

	inode_t * root = g_sblk->s_op->alloc_inode(g_sblk);
	g_vfsmount->m_root->d_inode = root;
	sprintf(g_vfsmount->m_root->d_iname,"/");
	g_vfsmount->m_root->d_mounted = 0;
	g_vfsmount->m_root->d_flags = DENTRY_FLAG_RO;
	g_vfsmount->m_root->d_parent = NULL;
	g_vfsmount->m_root->d_op = &walnutfs_dentry_op;
	g_vfsmount->m_root->d_sb = g_sblk;
	g_vfsmount->m_root->d_time = time(0);
	g_vfsmount->m_root->d_fsdata = NULL;
	
	logging("[INFO] init walnutfs success!\r\n");
	return;
}


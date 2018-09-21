/*filename: rootfs.cauthor:   wei-codertime:	  2018-09-15purpose:  rootfs文件系统的相关函数实现*/#include "vfs.h"static struct super_operations rootfs_op = {	.alloc_inode = alloc_inode_rf,	.destroy_inode = destroy_inode_rf,	.dirty_inode = NULL,	.write_inode = NULL,	.read_super = read_super_rf,	.write_super = NULL,};static fs_type_t rootfs_type = {	.name = "rootfs",	.fs_flag = 0,	.mount = rootfs_mount,	.kill_sb = rootfs_kill_sb};static inode_operations rootfs_inode_op = {	.create = create_inode_rf,	.mkdir = mkdir_rf,	.rmdir = rmdir_rf,	.mknod = NULL,	.rename = rename_rf,	.lookup = lookup_rf};	static dentry_t root_dentry = {	.d_inode = NULL;	.d_iname = "/";	.d_mounted = 0;};static file_t rootfs_file_op = {	.read = rootfs_read;	.write = rootfs_write;	.open = rootfs_open;	.close = rootfs_close;	.seek = rootfs_seek;	.mmap = rootfs_mmap;};/*super block的初始化函数，对rootfs来说，就是申请内存，赋初值*/int read_super_rf(sb_t * pSblk){	if(NULL == pSblk)	{		return KER_FAIL;	}	pSblk->s_dev = 0;	pSblk->s_blocksize = 4096;	pSblk->s_files = NULL;	pSblk->s_inodes = NULL;	pSblk->s_magic = ROOTFS_MAGIC_NUM;	pSblk->s_root = NULL;	pSblk->s_op = &rootfs_op;	return KER_SUC;}/*super block的操作函数，用于初始化一个inode对象*/inode_t * alloc_inode_rf(){}/*super block用于释放inode的函数*/bool destroy_inode_rf(u32 inode_no){	inode_t * tmp = find_inode(inode_no);	if(NULL == tmp)	{		logging("[WARN]: this inode number has no found!\r\n");		return false;	}	list_head_del_node(tmp->i_list);	free(tmp);	return true;}/*在内存中申请一个inode对象*/inode_t* new_inode_rf(){	inode_t * inode_rf = kmalloc(sizeof(inode_t));	if(NULL == inode_rf)	{		logging("[ROOTFS] alloc inode failed!\r\n");		return NULL;	}	memset(inode_rf, 0, sizeof(inode_t));	inode_rf->i_rdev = 0;	inode_rf->i_size = 0;	inode_rf->i_atime = 0;	inode_rf->i_ctime = 0;	inode_rf->i_blocks = 4096;	inode_rf->i_blkbits = 0;	inode_rf->i_mode = 0;	inode_rf->i_ino = 0xFFFFFFFF;	inode_rf->i_count = 0;	inode_rf->i_op = &rootfs_inode_op;	inode_rf->i_fop = NULL;	inode_rf->i_mapping = NULL;	return inode_rf;}inode_t* create_inode_rf(inode_t * fNode, dentry_t * pDentry, int flag, void * name){	return NULL;}//创建目录。 1，创建inode。2，创建dentry，3，创建file。4，加入链表。//输入：pnode：父目录inode。pDentry：本目录。mode：读写权限。int mkdir_rf(inode_t * pNode, dentry_t * pDentry, mode_t mode ){#if 0	struct list_head * tmp = NULL;	inode_t * new_node = new_inode_rf();	struct list_head * tail = getlast_lh(pNode->i_list);	tail->next = new_node->i_list;	new_node->i_list->next = NULL;	new_node->i_list->prev = tail;	pDentry->d_inode = new_node;	pDentry->d_child.next = NULL;	pDentry->d_child.prev = NULL;	tmp = getlast_lh(pDentry->d_parent->d_child);	tmp->next = pDentry->d_child;	pDentry->d_subdirs = NULL;#endif}int rmdir_rf(inode_t *, dentry_t *){}int rename_rf(inode_t *, dentry_t *){}dentry_t * lookup_rf(inode_t *, dentry_t *, void *){}dentry_t * create_dentry_rf(char * name, u32 flag){	dentry_t * pDentry = kmalloc(sizeof(dentry_t));	if(NULL == pDentry)	{		logging("[FATAL]:kmalloc dentry is failed!\r\n");		return NULL;	}	strncpy(pDentry->d_iname, name, DNAME_LEN_MAX);	pDentry->d_flags = flag;	return pDentry;}int rootfs_read(){	return 0;}int rootfs_write(){	return 0;}int rootfs_open(){	return 0;}int rootfs_close(){	return 0;}int rootfs_seek(){	return 0;}int rootfs_mmap(){	return 0;}dentry_t * lookup_dentry(char * name){	dentry_t * d_tmp = g_dentry;	queue_t * d_queue = create_queue();	while(d_tmp)	{		if(0 == strcmp(d_tmp->d_iname, name))		{			return d_tmp;		}		dentry_t * subdir = (dentry_t *)(d_tmp->d_subdirs.next);		if(NULL != subdir)		{			lnode_t * tmp_node1 = kmalloc(sizeof(lnode_t));			tmp_node1->pNode = (void *)subdir;			push(d_queue, tmp_node1);		}		dentry_t * child = (dentry_t *)(d_tmp->d_child.next - 1);		if(NULL != subdir)		{			lnode_t * tmp_node2 = kmalloc(sizeof(lnode_t));			tmp_node2->pNode = (void *)child;			push(d_queue, tmp_node2);		}		d_tmp = (dentry_t *)pop(d_queue);	}}file_t * create_file(char * father, char * name, u32 flag){	file_t * f_new = kmalloc(sizeof(file_t));	if(NULL == f_new)	{		logging("[FATAL]kmalloc file is failed!\r\n");		return NULL;	}	dentry_t * d_new = create_dentry_rf(name, flag);	if(NULL == d_new)	{		kfree(f_new);		logging("[FATAL]kmalloc dentry is failed!\r\n");		return NULL;	}	f_new->f_count = 1;	f_new->f_op = &rootfs_file_op;	f_new->f_dentry = d_new;	f_new->f_dirty = 0;	f_new->f_mode = 0x777;		if(g_dentry->d_subdirs.next)	{		dentry_t * son = (dentry_t *)(g_dentry->d_subdirs->next);		struct list_head * child_tail = getlast_lh(&(son->d_child)); 		child_tail->next = &(d_new->d_child);		d_new->d_child.prev = child_tail;	}	else	{		g_dentry->d_subdirs.next = d_new->d_subdirs;		d_new->d_subdirs.prev = g_dentry->d_subdirs;	}	return f_new;}dentry_t * rootfs_mount(struct file_system_type * fs_type, int flags, const char * fs_name, void * data){	sb_t * rootfs_sb = (sb_t *)kmalloc(sizeof(sb_t));	if(NULL == rootfs_sb)	{		logging("[FATAL] kmalloc rootfs super block failed!\r\n");		return NULL;	}	struct list_head * tail = getlast_lh(g_sblk->s_list);	if(NULL == tail)	{		g_sblk = rootfs_sb;		g_sblk->s_list->next = NULL;		g_sblk->s_list->prev = NULL;	}	else	{		tail->next = rootfs_sb->s_list;		rootfs_sb->s_list->prev = tail;		rootfs_sb->s_list->next = NULL;	}	if(read_super_rf(rootfs_sb) == KER_FAIL)	{		logging("[ERROR] rootfs:read super block failed!");		return NULL;	}	inode_t *root_node = new_inode_rf();	rootfs_sb->s_root = &root_dentry;	rootfs_sb->s_root->d_inode = root_node;	rootfs_sb->s_root->d_parent = NULL;	rootfs_sb->s_root->d_sb = rootfs_sb;	g_dentry = &root_dentry;	return &root_dentry;}void * rootfs_kill_sb(sb_t * psblk){	if(list_head_del_node(psblk->s_list))	{		file_t * delfile = (inode_t *)(psblk->s_files);		while(delfile)		{			psblk->s_files = delfile->f_list->next;			free(delfile);		}		inode_t * delnode = (inode_t *)(psblk->s_inodes);		while(delnode)		{			psblk->s_inodes = delnode->i_list->next;			psblk->s_op->destroy_inode(delnode));			delnode = (inode_t *)(psblk->s_inodes);		}		kfree(psblk);	};}void init_rootfs(){	register_filesystem(&rootfs_type);	rootfs_type.mount(&rootfs_type, 0, "rootfs", NULL);	logging("[INFO] init rootfs success!\r\n");	return;}/*创建根目录以及几个基本的目录项*/void init_mnt_tree(){	file_t * f_boot = create_file("boot"， 0);	inode_t * i_boot = new_inode_rf();	dentry_t * d_boot = f_boot->f_dentry;	struct list_head * inode_tail = getlast_lh(&(g_sblk->s_inodes));	inode_tail->next = &(i_boot->i_list);	i_boot->i_list->prev = inode_tail;		d_boot->d_inode = i_boot;}
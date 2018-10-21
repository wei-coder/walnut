/*
filename: vfs.h
author:		wei-coder
time:		2018-09-12
purpose:	vfs的相关的数据结构定义及函数声明
*/

#ifndef __VFS_H
#define __VFS_H

#include "type.h"

#define DNAME_LEN_MAX 36

#define ROOTFS_MAGIC_NUM	0x1

#define VFS_OK		0
#define VFS_FAIL	-1

struct inode_operations
{
	int (*create)(inode_t *, dentry_t *, int, void *);
	int (*mkdir)(inode_t *, dentry_t *, mode_t);
	int (*rmdir)(inode_t *, dentry_t *);
	int (*mknod)(inode_t *, dentry_t *, int, ulong);
	int (*rename)(inode_t *, dentry_t *);
	dentry_t * (*lookup)(inode_t *, dentry_t *, void *);
};

struct address_space_operation
{
	int (*write_page)(struct page *, struct wbc_t);
	int	(*read_page)(file_t *, struct page *);
	ulong (*direct_io)();
};

struct address_space
{
	const struct address_space_operation * a_ops;
};

typedef struct index_node
{
	struct list_head i_list;
	dev_t i_rdev;
	ulong i_size;
	ulong i_atime;
	ulong i_ctime;
	ulong i_blkbits;
	ulong i_blocks;
	u32	  i_mode;
	u32	  i_ino;
	u32	  i_count;
	u32	  i_dirty;
	struct inode_operations *i_op;
	struct file_operations * i_fop;
	struct address_space *	 i_mapping;
	void * i_private;
}inode_t;

struct super_operations
{
	inode_t *(*alloc_inode)();
	void (*destroy_inode)(inode_t *);
	void (*dirty_inode)(inode_t *);
	int (*write_inode)(inode_t *);
	int (*read_super)(sb_t *);
	int (*write_super)(sb_t *);
};

struct dentry_operations
{
};

/*
root
|
boot-bin-lib
|
xxx-yyy-zzz
如上图所示结构，d_child指向本目录的兄弟目录链表
			d_subdirs指向本目录的子目录。
			以此形成一个树状结构。
			增加子节点时需要先根据d_subdirs指针找到第一个子节点，
			然后得到该节点的dentry指针，再根据d_child指针作为链表头，找到表尾并插入。
*/
typedef struct dentry
{
	struct list_head  d_subdirs;
	struct list_head  d_child;
	inode_t * d_inode;
	u8 d_iname[DNAME_LEN_MAX];
	int	d_mounted;
	unsigned int d_flags;
	struct dentry *d_parent;
	const struct dentry_operations *d_op;
	struct super_block *d_sb;
	unsigned long d_time;
	void *d_fsdata;
}dentry_t;

typedef struct file
{
	struct list_head f_list;
	u32	f_count;
	dentry_t *	f_dentry;
	struct file_operations * f_op;
	u32	f_mode;
	ulong f_pos;
	u32	f_dirty;
	void * f_private;
	struct address_space * f_mapping;
}file_t;

typedef struct file_system_type
{
	struct list_head s_list;
	char * name;
	int	fs_flag;
	dentry_t * (*mount)(struct file_system_type *, int, const char *, void *);
	void (*kill_sb)(sb_t*);
}fs_type_t;

/*存储一个已经安装的文件系统信息，当系统初始化时，
会通过读取磁盘的特定位置来加载文件系统的控制信息*/
typedef struct super_block
{
	struct list_head	s_list;
	dev_t			 	s_dev;
	u8					s_dirt;
	u8					s_blocksize_bits;
	ulong				s_blocksize;
	ulong				s_maxbytes;
	struct file_system_type    		*s_type;
	const struct super_operations	*s_op;
	ulong				s_flags;//安装标识
	ulong				s_magic;
	struct dentry		*s_root;
	ulong    			s_umount;
	ulong				s_lock;
	int 		   		s_count;
	ulong				s_active;//引用计数
	struct list_head	s_inodes;
	struct list_head	s_files;
	struct list_head	s_mounts;
	void				*s_fs_info;
	unsigned int		s_max_links;
	u32					s_time_gran;
}sb_t;

extern sb_t * g_sblk;
extern fs_type_t * g_fslist;

#endif

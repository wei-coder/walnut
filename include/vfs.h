/*
filename: vfs.h
author:		wei-coder
time:		2018-09-12
purpose:	vfs����ص����ݽṹ���弰��������
*/

#ifndef __VFS_H
#define __VFS_H

#include "type.h"

#define DNAME_LEN_MAX 36

#define ROOTFS_MAGIC_NUM	0x1

struct file_operations
{
	int(*read)(int,const void*, u32);
	int(*write)(int,const void*, u32);
	int (*open)(const char *, int);
	int (*close)(int);
	ulong (*seek)(file_t *, ulong, int);
	int (*mmap)(file_t *,ulong);
};

struct inode_operations
{
	int (*create)(inode_t *, dentry_t *, int, void *);
	int (*mkdir)(inode_t *, dentry_t *, int);
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
	dev_t i_rdev;
	ulong i_size;
	ulong i_atime;
	ulong i_ctime;
	ulong i_blkbits;
	ulong i_blocks;
	u32	  i_mode;
	u32	  i_ino;
	u32	  i_count;
	struct inode_operations *i_op;
	struct file_operations * i_fop;
	struct address_space *	 i_mapping;
}inode_t;

struct super_operations
{
	inode_t *(*alloc_inode)(sb_t *);
	void (*destroy_inode)(inode_t *);
	void (*dirty_inode)(inode_t *);
	int (*write_inode)(inode_t *);
	int (*read_super)(sb_t *);
	int (*write_super)(sb_t *);
};

typedef struct dentry
{
	inode_t * d_inode;
	u8 d_iname[DNAME_LEN_MAX];
	int	d_mounted;
}dentry_t;

typedef struct file
{
	struct file_operations * f_op;
	u32	f_mode;
	ulong f_pos;
	struct address_space * f_mapping;
}file_t;

typedef struct file_system_type
{
	struct list_head * s_list;
	char * name;
	int	fs_flag;
	dentry_t * (*mount)(struct file_system_type *, int, const char *, void *);
	void (*kill_sb)(sb_t*);
}fs_type_t;

/*�洢һ���Ѿ���װ���ļ�ϵͳ��Ϣ����ϵͳ��ʼ��ʱ��
��ͨ����ȡ���̵��ض�λ���������ļ�ϵͳ�Ŀ�����Ϣ*/
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
	ulong				s_flags;//��װ��ʶ
	ulong				s_magic;
	struct dentry		*s_root;
	ulong    			s_umount;
	ulong				s_lock;
	int 		   		s_count;
	ulong				s_active;//���ü���
	struct list_head	s_inodes;
	struct list_head	s_files;
	struct list_head	s_mounts;
	void				*s_fs_info;
	unsigned int		s_max_links;
	u32					s_time_gran;
}sb_t;

#endif
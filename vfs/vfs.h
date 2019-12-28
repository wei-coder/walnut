/*
filename: vfs.h
author:		wei-coder
time:		2018-09-12
purpose:	vfs����ص����ݽṹ���弰��������
*/

#ifndef __VFS_H
#define __VFS_H

#include <types.h>
#include <klib.h>
#include "../mm/mm.h"

#define DNAME_LEN_MAX 36

#define ROOTFS_MAGIC_NUM	0x10101010
#define WALNUT_MAGIC_NUM	0x01010101


#define VFS_OK		0
#define VFS_FAIL	-1

#define DENTRY_FLAG_MNT		0x1
#define DENTRY_FLAG_RO		0x2
#define DENTRY_FLAG_WO		0x4

#define GET_PATH_FLAG_MEM	0x1		//���ڻ����в���·��
#define GET_PATH_FLAG_ALL	0x2		//���ڴ��Ӳ���в���·��

#define O_R			0x1
#define O_W			0x2
#define O_CREATE	0x4

#define INODE_FLAG_DIR		0x1
#define INODE_FLAG_FILE		0x2
#define INODE_FLAG_WO		0x4
#define INODE_FLAG_RO		0x8

typedef struct super_block 	sb_t;
typedef struct index_node	inode_t;
typedef struct dentry		dentry_t;
typedef struct vfs_mount_s	vfsmount_t;
typedef struct file			file_t;

struct super_operations
{
	inode_t *(*alloc_inode)(sb_t *);
	void (*destroy_inode)(inode_t *);
	void (*dirty_inode) (inode_t *);
	int (*write_inode) (inode_t *, int);
	void (*delete_inode) (inode_t *);
	void (*put_super) (sb_t *);
	void (*write_super) (sb_t *);
};

/*�洢һ���Ѿ���װ���ļ�ϵͳ��Ϣ����ϵͳ��ʼ��ʱ��
��ͨ����ȡ���̵��ض�λ���������ļ�ϵͳ�Ŀ�����Ϣ*/
typedef struct super_block
{
	struct list_head	s_list;						//ָ�򳬼��������ָ��
	dev_t			 	s_dev;						//�豸��ʶ��
	u8					s_dirt;						//�޸ı�־
	u8					s_blocksize_bits;			//��bitΪ��λ�Ŀ��С
	ulong				s_blocksize;				//���ֽ�Ϊ��λ�Ŀ��С
	ulong				s_maxbytes;					//�ļ�����󳤶�
	struct file_system_type    		*s_type;
	const struct super_operations	*s_op;
	ulong				s_flags;					//��װ��ʶ
	ulong				s_magic;					//ħ��
	dentry_t			*s_root;					//��Ŀ¼
	ulong    			s_umount;					//ж�����õ��ź���
	ulong				s_lock;						//�������ź���
	int 		   		s_count;					//���ü���
	ulong				s_active;					//�μ����ü���
	struct list_head	s_inodes;					//���������ڵ������
	struct list_head	s_files;					//�ļ���������
	struct list_head	s_instance;					//����ָ���ļ�ϵͳ�ĳ���������
	void				*s_fs_info;					//ָ���ض��ļ�ϵͳ�ĳ�������Ϣָ��
	u32					s_time_gran;				//���뼶��ʱ�������
}sb_t;

struct inode_operations
{
	int (*create) (struct index_node *, struct dentry *,int);
	struct dentry * (*lookup) (struct index_node *, struct dentry *);
	int (*link) (struct dentry *, struct index_node *, struct dentry *);
	int (*unlink) (struct index_node *, struct dentry *);
	int (*symlink) (struct index_node *, struct dentry *, const char *);
	int (*mkdir) (struct index_node *, struct dentry *, int);
	int (*rmdir) (struct index_node *, struct dentry *);
	int (*mknod) (struct index_node *, struct dentry *, int, dev_t);
	int (*rename) (struct index_node *, struct dentry *);
	int (*readlink) (struct dentry *, char *, int);
	void (*truncate) (struct index_node *);
	int (*permission) (struct index_node *, int);
	int (*setattr) (struct dentry *, void *);
	int (*getattr) (vfsmount_t *, struct dentry *, void *);
	int (*setxattr) (struct dentry *, const char *,	const void *, size_t, int);
	size_t (*getxattr) (struct dentry *, const char *, void *, size_t);
	size_t (*listxattr) (struct dentry *, char *, size_t);
	int (*removexattr) (struct dentry *, const char *);
};

typedef struct index_node
{
	struct list_head i_list;
	u32 i_rdev;
	ulong i_size;		//?�件大小
	ulong i_atime;		//上一次�?��??��?
	ulong i_mtime;		//上一次修?�时�?
	ulong i_ctime;		//?�建时�?
	ulong i_blkbits;	//?�大?
	ulong i_blocks;		//?�件�?��???
	u32	  i_mode;		//访�????
	u32	  i_ino;
	u32	  i_count;		//�?��?
	u32	  i_dirty;
	uid_t i_uid;
	gid_t i_gid;
	sb_t * i_sb;
	struct inode_operations *i_op;
	struct file_operations * i_fop;
	struct address_space *	 i_mapping;
	void * i_private;
}inode_t;

struct dentry_operations
{
	int (*d_revalidate)(struct dentry *);				//ʹdentry��Ч
	int (*d_hash) (struct dentry *, char *);			//��hash���м�һ��dentry
	int (*d_compare) (struct dentry *, char *, char *);	//���ú��ʾ��inode��ʹ�ø�dentry
	void (*d_delete)(struct dentry *);					//
	void (*d_release)(struct dentry *);					//�������һ��dentry
	void (*d_iput)(struct dentry *, inode_t *);			//����dentry�ͷ�����inode
};

/*
root
|
boot-bin-lib
|
xxx-yyy-zzz
����ͼ��ʾ�ṹ��d_childָ��Ŀ¼���ֵ�Ŀ¼����
			d_subdirsָ��Ŀ¼����Ŀ¼��
			�Դ��γ�һ����״�ṹ��
			�����ӽڵ�ʱ��Ҫ�ȸ���d_subdirsָ���ҵ���һ���ӽڵ㣬
			Ȼ��õ��ýڵ��dentryָ�룬�ٸ���d_childָ����Ϊ����ͷ���ҵ���β�����롣
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
#define VFS_CHILD_DENTRY(child) ((dentry_t *)((char *)(child) - sizeof(struct list_head)))

struct file_operations
{
	u32 (*lseek) (file_t *, u32, int);//??����??�ļ�??????��дλ??	
	u32 (*read) (file_t *, char *, int);//����??????��???ȡ????   
	u32 (*write) (file_t *, const char *, int);//??��??????????	
	//u32 (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, u32);//??ʼ??һ??�첽�Ķ�ȡ??��   
	//u32 (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, u32);//??ʼ??һ??�첽��????����   
	//int (*readdir) (struct file *, void *, filldir_t);//��????��ȡ??¼����??��????��??��???��ΪNULL   
	//u32 (*poll) (struct file *, struct poll_table_struct *); //??ѯ����????????????????��??��????????��д??????  
	int (*ioctl) (inode_t *, file_t *, u32, unsigned long); //????��??I/O??������	 
	int (*mmap) (file_t *, struct vm_area_struct *); //????��??����????��??��????��??ַ��?? 
	int (*open) (inode_t *, file_t *); //??��   
	int (*flush) (struct file *);	 
	int (*release) (inode_t *, struct file *); //????	 
	int (*fsync) (file_t *, struct dentry *, int); //??��??��????????	
	//int (*aio_fsync) (struct kiocb *, int); //�첽ˢ��??��????????   
	int (*fasync) (int, file_t *, int); //?????????FASYNC??־????????	
	//int (*lock) (struct file *, int, struct file_lock *);   
	int (*check_flags)(int);	 
	//int (*flock) (struct file *, int, struct file_lock *);  
};

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

typedef struct fd
{
	struct list_head fdnode;
	int fno;
	file_t * pfile;
}fd_t;


struct address_space_operation
{
	int (*write_page)();
	int	(*read_page)(file_t *);
	ulong (*direct_io)();
};

struct address_space
{
	const struct address_space_operation * a_ops;
};

typedef struct file_system_type
{
	struct list_head s_list;
	char * name;
	int	fs_flag;
	dentry_t * (*mount)(struct file_system_type *, int, const char *, void *);
	void (*kill_sb)(sb_t*);
}fs_type_t;


/*vfs_mount�ṹ������ָʾһ�����ص�*/
typedef struct vfs_mount_s
{
	struct vfs_mount_s *	m_parent;
	dentry_t * 				m_root;
	sb_t *					m_sb;
	dentry_t *				m_mntpoint;
	struct list_head		m_mount;
	struct list_head		m_child;
}vfsmount_t;
#define VFS_CHILD_MOUNTS(child)  ((vfsmount_t *)((char*)(child) + sizeof(struct list_head) - sizeof(vfsmount_t)))


typedef struct path_s
{
	dentry_t * p_dentry;
	vfsmount_t * p_vmount;
}path_t;

int vfs_get_path(char * dir, path_t * cpath, int flag);

#endif

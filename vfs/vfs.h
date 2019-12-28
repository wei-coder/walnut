/*
filename: vfs.h
author:		wei-coder
time:		2018-09-12
purpose:	vfs的相关的数据结构定义及函数声明
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

#define GET_PATH_FLAG_MEM	0x1		//仅在缓存中查找路径
#define GET_PATH_FLAG_ALL	0x2		//在内存和硬盘中查找路径

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

/*存储一个已经安装的文件系统信息，当系统初始化时，
会通过读取磁盘的特定位置来加载文件系统的控制信息*/
typedef struct super_block
{
	struct list_head	s_list;						//指向超级块链表的指针
	dev_t			 	s_dev;						//设备标识符
	u8					s_dirt;						//修改标志
	u8					s_blocksize_bits;			//以bit为单位的块大小
	ulong				s_blocksize;				//以字节为单位的块大小
	ulong				s_maxbytes;					//文件的最大长度
	struct file_system_type    		*s_type;
	const struct super_operations	*s_op;
	ulong				s_flags;					//安装标识
	ulong				s_magic;					//魔数
	dentry_t			*s_root;					//根目录
	ulong    			s_umount;					//卸载所用的信号量
	ulong				s_lock;						//超级块信号量
	int 		   		s_count;					//引用计数
	ulong				s_active;					//次级引用计数
	struct list_head	s_inodes;					//所有索引节点的链表
	struct list_head	s_files;					//文件对象链表
	struct list_head	s_instance;					//用于指定文件系统的超级块链表
	void				*s_fs_info;					//指向特定文件系统的超级块信息指针
	u32					s_time_gran;				//纳秒级的时间戳粒度
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
	ulong i_size;		//?囦欢澶у皬
	ulong i_atime;		//涓婁竴娆¤?块??堕?
	ulong i_mtime;		//涓婁竴娆′慨?规椂闂?
	ulong i_ctime;		//?涘缓鏃堕?
	ulong i_blkbits;	//?楀ぇ?
	ulong i_blocks;		//?囦欢鐨?椾???
	u32	  i_mode;		//璁块????
	u32	  i_ino;
	u32	  i_count;		//寮?ㄦ?
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
	int (*d_revalidate)(struct dentry *);				//使dentry生效
	int (*d_hash) (struct dentry *, char *);			//向hash表中加一个dentry
	int (*d_compare) (struct dentry *, char *, char *);	//调用后表示无inode在使用该dentry
	void (*d_delete)(struct dentry *);					//
	void (*d_release)(struct dentry *);					//用于清除一个dentry
	void (*d_iput)(struct dentry *, inode_t *);			//用于dentry释放它的inode
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
#define VFS_CHILD_DENTRY(child) ((dentry_t *)((char *)(child) - sizeof(struct list_head)))

struct file_operations
{
	u32 (*lseek) (file_t *, u32, int);//??来修??文件??????读写位??	
	u32 (*read) (file_t *, char *, int);//从设??????步???取????   
	u32 (*write) (file_t *, const char *, int);//??设??????????	
	//u32 (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, u32);//??始??一??异步的读取??作   
	//u32 (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, u32);//??始??一??异步的????操作   
	//int (*readdir) (struct file *, void *, filldir_t);//仅????读取??录，对??设????件??该???段为NULL   
	//u32 (*poll) (struct file *, struct poll_table_struct *); //??询函数????????????????以??行????????读写??????  
	int (*ioctl) (inode_t *, file_t *, u32, unsigned long); //????设??I/O??制命令	 
	int (*mmap) (file_t *, struct vm_area_struct *); //????请??将设????存??射????程??址空?? 
	int (*open) (inode_t *, file_t *); //??开   
	int (*flush) (struct file *);	 
	int (*release) (inode_t *, struct file *); //????	 
	int (*fsync) (file_t *, struct dentry *, int); //??新??处????????	
	//int (*aio_fsync) (struct kiocb *, int); //异步刷新??处????????   
	int (*fasync) (int, file_t *, int); //?????????FASYNC??志????????	
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


/*vfs_mount结构，用于指示一个挂载点*/
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

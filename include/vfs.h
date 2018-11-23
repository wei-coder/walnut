/*
filename: vfs.h
author:		wei-coder
time:		2018-09-12
purpose:	vfsµÄÏà¹ØµÄÊı¾İ½á¹¹¶¨Òå¼°º¯ÊıÉùÃ÷
*/

#ifndef __VFS_H
#define __VFS_H

#include "type.h"

#define DNAME_LEN_MAX 36

#define ROOTFS_MAGIC_NUM	0x1

#define VFS_OK		0
#define VFS_FAIL	-1

#define DENTRY_FLAG_MNT		0x1
#define DENTRY_FLAG_RO		0x2
#define DENTRY_FLAG_WO		0x4

#define GET_PATH_FLAG_MEM	0x1		//½öÔÚ»º´æÖĞ²éÕÒÂ·¾¶
#define GET_PATH_FLAG_ALL	0x2		//ÔÚÄÚ´æºÍÓ²ÅÌÖĞ²éÕÒÂ·¾¶

#define O_R			0x1
#define O_W			0x2
#define O_CREATE	0x4

#define INODE_FLAG_DIR		0x1
#define INODE_FLAG_FILE		0x2
#define INODE_FLAG_WO		0x4
#define INODE_FLAG_RO		0x8

struct inode_operations
{
	int (*create) (inode_t *, struct dentry *,int);
	struct dentry * (*lookup) (inode_t *, struct dentry *);
	int (*link) (struct dentry *, inode_t *, struct dentry *);
	int (*unlink) (inode_t *, struct dentry *);
	int (*symlink) (inode_t *, struct dentry *, const char *);
	int (*mkdir) (inode_t *, struct dentry *, int);
	int (*rmdir) (inode_t *, struct dentry *);
	int (*mknod) (inode_t *, struct dentry *, int, dev_t);
	int (*rename) (inode_t *, struct dentry *,	inode_t *, struct dentry *);
	int (*readlink) (struct dentry *, char *, int);
	void (*truncate) (inode_t *);
	int (*permission) (inode_t *, int);
	int (*setattr) (struct dentry *, struct iattr *);
	int (*getattr) (vfsmount_t *, struct dentry *, struct kstat *);
	int (*setxattr) (struct dentry *, const char *,	const void *, size_t, int);
	size_t (*getxattr) (struct dentry *, const char *, void *, size_t);
	size_t (*listxattr) (struct dentry *, char *, size_t);
	int (*removexattr) (struct dentry *, const char *);
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
	u32 i_rdev;
	ulong i_size;		//æ–‡ä»¶å¤§å°
	ulong i_atime;		//ä¸Šä¸€æ¬¡è®¿é—®æ—¶é—´
	ulong i_mtime;		//ä¸Šä¸€æ¬¡ä¿®æ”¹æ—¶é—´
	ulong i_ctime;		//åˆ›å»ºæ—¶é—´
	ulong i_blkbits;	//å—å¤§å°
	ulong i_blocks;		//æ–‡ä»¶çš„å—ä¸ªæ•°
	u32	  i_mode;		//è®¿é—®æƒé™
	u32	  i_ino;
	u32	  i_count;		//å¼•ç”¨æ•°
	u32	  i_dirty;
	uid_t i_uid;
	gid_t i_gid;
	sb_t * i_sb;
	struct inode_operations *i_op;
	struct file_operations * i_fop;
	struct address_space *	 i_mapping;
	void * i_private;
}inode_t;

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

struct dentry_operations
{
	int (*d_revalidate)(struct dentry *);				//Ê¹dentryÉúĞ§
	int (*d_hash) (struct dentry *, char *);			//Ïòhash±íÖĞ¼ÓÒ»¸ödentry
	int (*d_compare) (struct dentry *, char *, char *);	//µ÷ÓÃºó±íÊ¾ÎŞinodeÔÚÊ¹ÓÃ¸Ãdentry
	void (*d_delete)(struct dentry *);					//
	void (*d_release)(struct dentry *);					//ÓÃÓÚÇå³ıÒ»¸ödentry
	void (*d_iput)(struct dentry *, inode_t *);			//ÓÃÓÚdentryÊÍ·ÅËüµÄinode
};

/*
root
|
boot-bin-lib
|
xxx-yyy-zzz
ÈçÉÏÍ¼ËùÊ¾½á¹¹£¬d_childÖ¸Ïò±¾Ä¿Â¼µÄĞÖµÜÄ¿Â¼Á´±í
			d_subdirsÖ¸Ïò±¾Ä¿Â¼µÄ×ÓÄ¿Â¼¡£
			ÒÔ´ËĞÎ³ÉÒ»¸öÊ÷×´½á¹¹¡£
			Ôö¼Ó×Ó½ÚµãÊ±ĞèÒªÏÈ¸ù¾İd_subdirsÖ¸ÕëÕÒµ½µÚÒ»¸ö×Ó½Úµã£¬
			È»ºóµÃµ½¸Ã½ÚµãµÄdentryÖ¸Õë£¬ÔÙ¸ù¾İd_childÖ¸Õë×÷ÎªÁ´±íÍ·£¬ÕÒµ½±íÎ²²¢²åÈë¡£
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

/*´æ´¢Ò»¸öÒÑ¾­°²×°µÄÎÄ¼şÏµÍ³ĞÅÏ¢£¬µ±ÏµÍ³³õÊ¼»¯Ê±£¬
»áÍ¨¹ı¶ÁÈ¡´ÅÅÌµÄÌØ¶¨Î»ÖÃÀ´¼ÓÔØÎÄ¼şÏµÍ³µÄ¿ØÖÆĞÅÏ¢*/
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
	ulong				s_flags;//°²×°±êÊ¶
	ulong				s_magic;
	dentry_t			*s_root;
	ulong    			s_umount;
	ulong				s_lock;
	int 		   		s_count;
	ulong				s_active;//ÒıÓÃ¼ÆÊı
	struct list_head	s_inodes;
	struct list_head	s_files;
	struct list_head	s_mounts;
	void				*s_fs_info;
	unsigned int		s_max_links;
	u32					s_time_gran;
}sb_t;

/*vfs_mount½á¹¹£¬ÓÃÓÚÖ¸Ê¾Ò»¸ö¹ÒÔØµã*/
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

#endif

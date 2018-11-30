/*filename: file.h
author:wei-coder
time:  2018-10-20
purpose: file�����ͷ�ļ�*/


#ifndef _FILE_H__
#define _FILE_H__

#include "type.h"
#include "vfs.h"
#include "list.h"

#define F_DUPFD		0	/* dup */
#define F_GETFD		1	/* get close_on_exec */
#define F_SETFD		2	/* set/clear close_on_exec */
#define F_GETFL		3	/* get file->f_flags */
#define F_SETFL		4	/* set file->f_flags */

#define F_MODE_DIR	0
#define F_MODE_FILE	1
#define	F_MODE_RO	2
#define F_MODE_WO	4

struct stat {
	u32	st_dev;
	u32	st_ino;
	u32	st_mode;
	u32	st_nlink;
	u32	st_uid;
	u32	st_gid;
	u32	st_rdev;
	long	st_size;
	ulong	st_atime;
	ulong	st_mtime;
	ulong	st_ctime;
	u32	st_blksize;
	u32	st_blocks;
	u32	st_flags;
	u32	st_gen;
};

struct file_operations
{
	u32 (*lseek) (struct file *, u32, int);//?�来修?�文件�??�?�读写位�?	
	u32 (*read) (struct file *, char *, u32 *, int);//从设�?�??�步�?�取?��?   
	u32 (*write) (struct file *, const char *, u32 *, int);//?�设�??�?�?��?	
	//u32 (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, u32);//?�始?�一�?异步的读取?�作   
	//u32 (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, u32);//?�始?�一�?异步的?�?�操作   
	//int (*readdir) (struct file *, void *, filldir_t);//仅?��?读取??录，对�?设�??�件�?该�?�段为NULL   
	//u32 (*poll) (struct file *, struct poll_table_struct *); //�?询函数�??��????�???��?以�?行?�?��??�读写?�?�??  
	int (*ioctl) (inode_t *, struct file *, u32, unsigned long); //?��?设�?I/O?�制命令	 
	int (*mmap) (struct file *, struct vm_area_struct *); //?��?请�?将设�??�存?�射?��?程?�址空�? 
	int (*open) (inode_t *, struct file *); //?�开   
	int (*flush) (struct file *);	 
	int (*release) (inode_t *, struct file *); //?��?	 
	int (*fsync) (struct file *, struct dentry *, int); //?�新�?处?�?�?��?	
	//int (*aio_fsync) (struct kiocb *, int); //异步刷新�?处?�?�?��?   
	int (*fasync) (int, struct file *, int); //?�?��?��?FASYNC?�志?�?�?�?�	
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


u32 sys_read(int fd, char * buf, int len);
u32 sys_write(int fd, const char * buf, int len);
int sys_open(const char * filename, int flags);
int sys_close(int fd);
int sys_create(const char * filename, int mode);
int sys_stat();
int sys_lseek();
int sys_fstat();
int sys_fcntl();

#endif

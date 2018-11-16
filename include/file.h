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
	u32 (*llseek) (struct file *, u32, int);//用来修改文件当前的读写位置	
	u32 (*read) (struct file *, char *, u32 *);//从设备中同步读取数据   
	u32 (*write) (struct file *, const char *, u32 *);//向设备发送数据	
	//u32 (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, u32);//初始化一个异步的读取操作   
	//u32 (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, u32);//初始化一个异步的写入操作   
	//int (*readdir) (struct file *, void *, filldir_t);//仅用于读取目录，对于设备文件，该字段为NULL   
	//u32 (*poll) (struct file *, struct poll_table_struct *); //轮询函数，判断目前是否可以进行非阻塞的读写或写入   
	int (*ioctl) (inode_t *, struct file *, u32, unsigned long); //执行设备I/O控制命令	 
	int (*mmap) (struct file *, struct vm_area_struct *); //用于请求将设备内存映射到进程地址空间  
	int (*open) (inode_t *, struct file *); //打开   
	int (*flush) (struct file *);	 
	int (*release) (inode_t *, struct file *); //关闭	 
	int (*fsync) (struct file *, struct dentry *, int); //刷新待处理的数据	
	//int (*aio_fsync) (struct kiocb *, int); //异步刷新待处理的数据   
	int (*fasync) (int, struct file *, int); //通知设备FASYNC标志发生变化	
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
int sys_create(const char * filename, mode_t mode);
int sys_stat();
int sys_lseek();
int sys_fstat();
int sys_fcntl();

#endif

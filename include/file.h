/*filename: file.h
author:wei-coder
time:  2018-10-20
purpose: file对象的头文件*/


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
	u32 (*lseek) (struct file *, u32, int);//?ㄦ潵淇�?规枃浠跺??�?勮鍐欎綅缃?	
	u32 (*read) (struct file *, char *, u32 *, int);//浠庤�?涓??屾�?诲彇?版�?   
	u32 (*write) (struct file *, const char *, u32 *, int);//?戣�??�?�?版�?	
	//u32 (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, u32);//?濆?栦竴涓?寮傛鐨勮鍙�?嶄綔   
	//u32 (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, u32);//?濆?栦竴涓?寮傛鐨�?�?ユ搷浣�   
	//int (*readdir) (struct file *, void *, filldir_t);//浠�?ㄤ?璇诲彇??褰曪紝瀵逛?璁惧??囦欢�?璇ュ?楁涓篘ULL   
	//u32 (*poll) (struct file *, struct poll_table_struct *); //杞?璇㈠嚱鏁帮??ゆ�????�???﹀�?浠ヨ?琛�?�?诲??勮鍐�?�?�??  
	int (*ioctl) (inode_t *, struct file *, u32, unsigned long); //?ц?璁惧?I/O?у埗鍛戒护	 
	int (*mmap) (struct file *, struct vm_area_struct *); //?ㄤ?璇锋?灏嗚�??呭瓨?犲皠?拌?绋�?板潃绌洪�? 
	int (*open) (inode_t *, struct file *); //?撳紑   
	int (*flush) (struct file *);	 
	int (*release) (inode_t *, struct file *); //?抽�?	 
	int (*fsync) (struct file *, struct dentry *, int); //?锋柊�?澶�?�?�?版�?	
	//int (*aio_fsync) (struct kiocb *, int); //寮傛鍒锋柊�?澶�?�?�?版�?   
	int (*fasync) (int, struct file *, int); //?�?ヨ?惧?FASYNC?囧織?�?�?�?�	
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

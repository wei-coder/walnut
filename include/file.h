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
	u32 (*lseek) (struct file *, u32, int);//鐢ㄦ潵淇敼鏂囦欢褰撳墠鐨勮鍐欎綅缃�	
	u32 (*read) (struct file *, char *, u32 *);//浠庤澶囦腑鍚屾璇诲彇鏁版嵁   
	u32 (*write) (struct file *, const char *, u32 *);//鍚戣澶囧彂閫佹暟鎹�	
	//u32 (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, u32);//鍒濆鍖栦竴涓紓姝ョ殑璇诲彇鎿嶄綔   
	//u32 (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, u32);//鍒濆鍖栦竴涓紓姝ョ殑鍐欏叆鎿嶄綔   
	//int (*readdir) (struct file *, void *, filldir_t);//浠呯敤浜庤鍙栫洰褰曪紝瀵逛簬璁惧鏂囦欢锛岃瀛楁涓篘ULL   
	//u32 (*poll) (struct file *, struct poll_table_struct *); //杞鍑芥暟锛屽垽鏂洰鍓嶆槸鍚﹀彲浠ヨ繘琛岄潪闃诲鐨勮鍐欐垨鍐欏叆   
	int (*ioctl) (inode_t *, struct file *, u32, unsigned long); //鎵ц璁惧I/O鎺у埗鍛戒护	 
	int (*mmap) (struct file *, struct vm_area_struct *); //鐢ㄤ簬璇锋眰灏嗚澶囧唴瀛樻槧灏勫埌杩涚▼鍦板潃绌洪棿  
	int (*open) (inode_t *, struct file *); //鎵撳紑   
	int (*flush) (struct file *);	 
	int (*release) (inode_t *, struct file *); //鍏抽棴	 
	int (*fsync) (struct file *, struct dentry *, int); //鍒锋柊寰呭鐞嗙殑鏁版嵁	
	//int (*aio_fsync) (struct kiocb *, int); //寮傛鍒锋柊寰呭鐞嗙殑鏁版嵁   
	int (*fasync) (int, struct file *, int); //閫氱煡璁惧FASYNC鏍囧織鍙戠敓鍙樺寲	
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

/*filename: file.h
author:wei-coder
time:  2018-10-20
purpose: file对象的头文件*/


#ifndef _FILE_H__
#define _FILE_H__

#include <types.h>
#include <klib.h>
#include "vfs.h"

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

int sys_read(int fd, char * buf, int len);
int sys_write(int fd, char * buf, int len);
int sys_open(char * filename, int flags);
int sys_close(int fd);
int sys_create(const char * filename, int mode);
int sys_stat();
int sys_lseek();
int sys_fstat();
int fcntl(int fd, int cmd, ...);

#endif

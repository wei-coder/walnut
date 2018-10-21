/*filename: file.h
author:wei-coder
time:  2018-10-20
purpose: file对象的头文件*/

#ifndef _FILE_H__
#define _FILE_H__

#include "type.h"
#include "vfs.h"
#include "list.h"

struct file_operations
{
	int(*read)(int,const void*, u32);
	int(*write)(int,const void*, u32);
	int (*open)(const char *, int);
	int (*close)(int);
	ulong (*seek)(file_t *, ulong, int);
	int (*mmap)(file_t *,ulong);
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


int sys_read(int fd, char * buf, int len);
int sys_write(int fd, const char * buf, int len);
int sys_open(const char * filename, int flags);
int sys_close(int fd);
int sys_creat(const char * filename, mode_t mode);
int sys_stat();
int sys_lseek();
int sys_fstat();
int sys_fcntl();

#endif

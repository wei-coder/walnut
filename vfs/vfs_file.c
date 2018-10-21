/*filename: file.c
author:wei-coder
time:  2018-10-20
purpose: file对象的实现文件*/

#include "file.h"

int sys_read(int fd, char * buf, int len)
{
	file_t * this_file =  getfile(fd);
	return do_read(this_file, buf, len);
}

int sys_write(int fd, const char * buf, int len)
{
	return 0;
}

int sys_open(const char * filename, int flags)
{
	return 0;
}

int sys_close(int fd)
{
	return 0;
}

int sys_creat(const char * filename, mode_t mode)
{
	return 0;
}

int sys_stat()
{
	return 0;
}

int sys_lseek()
{
	return 0;
}

int sys_fstat()
{
	return 0;
}

int sys_fcntl()
{
	return 0;
}


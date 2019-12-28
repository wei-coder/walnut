/*filename: file.c
author:wei-coder
time:  2018-10-20
purpose: file对象的实现文件*/

#include <unistd.h>
#include <karg.h>
#include <string.h>
#include "../mm/heap.h"
#include "../task/sched.h"
#include "vfs.h"
#include "file.h"

fd_t * getfile(int fd)
{
	fd_t * tmpfd = NULL;
	struct list_head * pNode = &(current->flist.phead->fdnode);
	while(NULL != pNode)
	{
		tmpfd = (fd_t*)pNode;
		if(fd == tmpfd->fno)
		{
			return tmpfd;
		}
	}
	return NULL;
}
int get_fd(int min)
{
	for(int i = min/(8*sizeof(ulong)); i<4; i++)
	{
		for(int j = min%(8*sizeof(ulong)); j<8*sizeof(ulong); j++)
		{
			if(current->flist.fmap[i]&(1<<j))
			{
				return i*(8*sizeof(ulong))+j;
			}
		}
	}
	return -1;
}

static inline void set_fd(int fd)
{
	current->flist.fmap[fd/(8*sizeof(ulong))] |= 1<<(fd%(8*sizeof(ulong)));
}

static inline void reset_fd(int fd)
{
	current->flist.fmap[fd/(8*sizeof(ulong))] &= ~(1<<(fd%(8*sizeof(ulong))));
}

int sys_read(int fd, char * buf, int len)
{
	fd_t * this_fd =  getfile(fd);
	if(NULL == this_fd)
	{
		return -1;
	}
	return this_fd->pfile->f_op->read(this_fd->pfile,buf,len);
}

int sys_write(int fd, char * buf, int len)
{
	fd_t * this_fd =  getfile(fd);
	if(NULL == this_fd)
	{
		return -1;
	}
	return this_fd->pfile->f_op->write(this_fd->pfile,buf,len);
}

int sys_open(char * filename, int flags)
{
	path_t path = {0};
	if(VFS_OK == vfs_get_path(filename, &path, GET_PATH_FLAG_ALL))
	{
		file_t * newfile = (file_t*)kmalloc(sizeof(file_t));
		newfile->f_count = 1;
		newfile->f_dentry = path.p_dentry;
		newfile->f_pos = 0;
		newfile->f_mode = 0;
		newfile->f_dirty = 0;
		fd_t * newfd = (fd_t*)kmalloc(sizeof(fd_t));
		current->flist.num++;
		newfd->fno = get_fd(0);
		newfd->pfile = newfile;
		if(NULL == current->flist.phead)
		{
			newfd->fdnode.next = (struct list_head *)newfd;
			newfd->fdnode.prev = (struct list_head *)newfd;
			current->flist.phead = newfd;
		}
		else
		{
			newfd->fdnode.next = &(current->flist.phead->fdnode);
			newfd->fdnode.prev = current->flist.phead->fdnode.prev;
			current->flist.phead->fdnode.prev->next = &(newfd->fdnode);
			current->flist.phead->fdnode.prev = &(newfd->fdnode);
		}
		set_fd(newfd->fno);
		return newfd->fno;
	}
	else if(O_CREATE&flags)
	{
		return sys_create(filename, 0);
	}
	return -1;
}

int sys_close(int fd)
{
	fd_t * pfd = getfile(fd);
	if(NULL == pfd)
	{
		return -1;
	}
	pfd->fdnode.next->prev = pfd->fdnode.prev;
	pfd->fdnode.prev->next = pfd->fdnode.next;
	file_t * pfile = pfd->pfile;
	free(pfile);
	free(pfd);
	reset_fd(fd);
	return 0;
}

int sys_create(const char * filename, int mode)
{
	path_t path = {0};
	size_t namelen = strlen(filename);
	char *pointer = (char *)(filename+namelen);
	while(*(--pointer)!= '/')
	{
		/*do nothing*/;
	}
	namelen = namelen-strlen(pointer);
	char * pathname = (char *)kmalloc(namelen+1);
	strncpy(pathname,filename,namelen);
	if(VFS_OK == vfs_get_path(pathname,&path, GET_PATH_FLAG_ALL))
	{
		return -1;
	}
	dentry_t * newdentry = (dentry_t *)kmalloc(sizeof(dentry_t));
	memset((void *)newdentry, 0, sizeof(dentry_t));
	pointer ++;
	namelen = strlen((const char *)pointer);
	strncpy((char *)(newdentry->d_iname), pointer, (namelen<DNAME_LEN_MAX?namelen:DNAME_LEN_MAX));
	newdentry->d_parent = path.p_dentry;
	newdentry->d_subdirs.next = NULL;
	newdentry->d_subdirs.prev = NULL;
	newdentry->d_child.next = &(path.p_dentry->d_subdirs);
	newdentry->d_child.prev = path.p_dentry->d_subdirs.prev;
	path.p_dentry->d_subdirs.prev->next = &(newdentry->d_child);
	path.p_dentry->d_subdirs.prev = &(newdentry->d_child);
	newdentry->d_op = path.p_dentry->d_op;
	newdentry->d_sb = path.p_dentry->d_sb;
	newdentry->d_time = time(0);
	return path.p_dentry->d_inode->i_op->create(path.p_dentry->d_inode,newdentry,mode);;
}

int sys_lseek(int fd, u32 offset, int where)
{
	fd_t * this_fd = getfile(fd);
	if(NULL == this_fd)
	{
		return -1;
	}
	file_t * pfile = this_fd->pfile;
	switch(where)
	{
		case SEEK_SET:
			pfile->f_pos = offset;
			break;
		case SEEK_END:
			pfile->f_pos = pfile->f_dentry->d_inode->i_size;
			break;
		case SEEK_CUR:
			if(pfile->f_pos + offset > pfile->f_dentry->d_inode->i_size)
			{
				pfile->f_pos = pfile->f_dentry->d_inode->i_size;
			}
			else
			{
				pfile->f_pos += offset;
			}
			break;
		default:
			break;
	}

	return 0;
}

int sys_fstat(int fd, struct stat *buf)
{
	if(-1 == fd)
	{
		return -1;
	}
	fd_t * pfd = getfile(fd);
	if(NULL == pfd)
	{
		return -1;
	}
	file_t * pfile = pfd->pfile;
	buf->st_dev = pfile->f_dentry->d_inode->i_rdev;
	buf->st_ino = pfile->f_dentry->d_inode->i_ino;
	buf->st_mode = pfile->f_dentry->d_inode->i_mode;
	buf->st_nlink = pfile->f_dentry->d_inode->i_count;
	buf->st_uid = pfile->f_dentry->d_inode->i_uid;
	buf->st_gid = pfile->f_dentry->d_inode->i_gid;
	buf->st_rdev = pfile->f_dentry->d_inode->i_rdev;
	buf->st_size = pfile->f_dentry->d_inode->i_size;
	buf->st_atime = pfile->f_dentry->d_inode->i_atime;
	buf->st_ctime = pfile->f_dentry->d_inode->i_ctime;
	buf->st_blksize = pfile->f_dentry->d_inode->i_blkbits;
	buf->st_blocks = pfile->f_dentry->d_inode->i_blocks;
	buf->st_flags = pfile->f_dentry->d_flags;
	buf->st_gen = 0;
	return 0;
}

int sys_fcntl(int fd, int cmd, ...)
{
	va_list args;
	va_start(args,cmd);
	int arg = va_arg(args,int);
	fd_t * pfd = getfile(fd);
	if(NULL == pfd)
	{
		return -1;
	}
	file_t * pfile = pfd->pfile;
	switch(cmd)
	{
		case F_DUPFD:
			return get_fd(arg);
		case F_GETFD:
			return pfile->f_mode;
		case F_SETFD:
			pfile->f_mode &= arg;
			break;
		case F_GETFL:
			return pfile->f_mode;
		case F_SETFL:
			pfile->f_mode &= arg;
			break;
		default:
			break;
	}
	return 0;
}

int sys_stat(char * filename, struct stat * stat_buf)
{
	int fd = sys_open(filename, O_R);
	return sys_fstat(fd, stat_buf);
}

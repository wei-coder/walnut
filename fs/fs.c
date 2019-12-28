/*filename: fs.c
author:wei-coder
time:  2018-10-20
purpose: 文件系统的用户侧接口函数*/

#include <unistd.h>

_syscall2(int,open,const char *,filename,int,flag);
_syscall3(int,read,int,fd,char *,buf,off_t,count);
_syscall3(int,write,int,fd,const char *,buf,off_t,count);
_syscall1(int,close,int,fd);
_syscall2(int,creat,const char *,filename, mode_t,mode);
_syscall3(int,stat,const char *,filename,struct stat *,stat_buf);
_syscall2(int,fstat,int,fd,struct stat *, stat_buf);
_syscall3(int,lseek,int,fd,off_t,offset,int,origin);
_syscall1(int,chdir,const char *,filename);
_syscall2(int,chmod,const char *,filename,mode_t,mode);
_syscall3(int,chown,const char *, filename, uid_t,owner, gid_t,group);
_syscall1(int,chroot,const char *, filename);
_syscall3(int,mount,const char *,specialfile, const char *,dir, int,rwflag);
_syscall1(int,umount,const char *,specialfile);
_syscall3(int,mknod,const char *,filename, mode_t,mode,dev_t,dev);
_syscall2(int,link,const char *,filename1, const char *,filename2);
_syscall1(int,unlink,const char *,filename);
_syscall2(int,mkdir,const char *,pathname, mode_t,mode);
_syscall1(int,rmdir,const char *,pathname);
_syscall2(int,rename,const char *,oldpath,const char *, newpath);


/*
filename: string.h
author:	wei-coder
date:	2017-12
purpose:	字符串相关函数的声明
*/

<<<<<<< HEAD
#ifndef __STRING_H
#define __STRING_H
=======
#ifdef __STRING_H
#define __STRING_H 
>>>>>>> 2747b789ab05c6901e2e08b9db7fbb72fd8f6f24

int memcmp(const void *s1, const void *s2, size_t len);
void memset(const void * dest, u8 value, size_t len);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *cs, const char *ct, size_t count);
size_t strnlen(const char *s, size_t maxlen);
unsigned int atou(const char *s);
unsigned long long simple_strtoull(const char *cp, char **endp, unsigned int base);
char *strstr(const char *s1, const char *s2);

#endif

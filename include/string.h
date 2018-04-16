/*
filename: string.h
author:	wei-coder
date:	2017-12
purpose:	�ַ�����غ���������
*/

#ifdef __STRING_H
#define __STRING_H 

int memcmp(const void *s1, const void *s2, size_t len);
void memset(const void * dest, u8 value, size_t len);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *cs, const char *ct, size_t count);
size_t strnlen(const char *s, size_t maxlen);
unsigned int atou(const char *s);
unsigned long long simple_strtoull(const char *cp, char **endp, unsigned int base);
char *strstr(const char *s1, const char *s2);

#endif

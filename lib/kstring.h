/*
filename: string.h
author:	wei-coder
date:	2017-12
purpose:	字符串相关函数的声明
*/

#pragma once

#include <types.h>

int memcmp(const void *s1, const void *s2, size_t len);

void memset(void * dest, u8 value, size_t len);

void memcpy(void * dest, void * source, size_t len);

void strcpy(char * dst, char * src);

void strncpy(char * dst, const char * src, size_t len);

int strcmp(const char *str1, const char *str2);

int strncmp(const char *cs, const char *ct, size_t count);

int strnlen(const char *s, size_t maxlen);

size_t strlen(const char *s);

char *strstr(const char *s1, const char *s2);

char * strchar(char * s, char c);

char * strtok(char *s, const char *delim);

unsigned int atou(const char *s);

unsigned long long simple_strtoull(const char *cp, char **endp, unsigned int base);


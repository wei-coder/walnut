/*
filename: string.c
author:	wei-coder
date:	2017-12
purpose:	字符串及内存操作相关的基础函数
*/

#include <types.h>
#include "kstring.h"

int memcmp(const void *s1, const void *s2, size_t len)
{
	bool diff;
	asm("repe; cmpsb; setnz %0"
	    : "=qm" (diff), "+D" (s1), "+S" (s2), "+c" (len));
	return diff;
}

void memset(void * dest, u8 value, size_t len)
{
	u8 *dst = (u8*)dest;
	while(len)
	{
		*dst++ = value;
		len--;
	}
}

void memcpy(void * dest, void * source, size_t len)
{
	u8 * dst = (u8 *)dest;
	u8 * src = (u8 *)source;
	while(len)
	{
		*dst++ = *src++;
		len --;
	}
}

void strcpy(char * dst, char * src)
{
	while(*src)
	{
		*dst++ = *src ++;
	}
	*dst = 0;
}

void strncpy(char * dst, const char * src, size_t len)
{
	while(len-- && *src)
	{
		*dst++ = *src++;
	}
	*dst = 0;
}


int strcmp(const char *str1, const char *str2)
{
	const unsigned char *s1 = (const unsigned char *)str1;
	const unsigned char *s2 = (const unsigned char *)str2;
	int delta = 0;

	while (*s1 || *s2) {
		delta = *s1 - *s2;
		if (delta)
			return delta;
		s1++;
		s2++;
	}
	return 0;
}

int strncmp(const char *cs, const char *ct, size_t count)
{
	unsigned char c1, c2;

	while (count) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
		count--;
	}
	return 0;
}

int strnlen(const char *s, size_t maxlen)
{
	const char *es = s;
	while (*es && maxlen) {
		es++;
		maxlen--;
	}

	return (es - s);
}

unsigned int atou(const char *s)
{
	unsigned int i = 0;
	while (isdigit(*s))
		i = i * 10 + (*s++ - '0');
	return i;
}

#define TOLOWER(x) ((x) | 0x20)

static unsigned int simple_guess_base(const char *cp)
{
	if (cp[0] == '0') {
		if (TOLOWER(cp[1]) == 'x' && isxdigit(cp[2]))
			return 16;
		else
			return 8;
	} else {
		return 10;
	}
}

unsigned long long simple_strtoull(const char *cp, char **endp, unsigned int base)
{
	unsigned long long result = 0;

	if (!base)
		base = simple_guess_base(cp);

	if (base == 16 && cp[0] == '0' && TOLOWER(cp[1]) == 'x')
		cp += 2;

	while (isxdigit(*cp)) {
		unsigned int value;

		value = isdigit(*cp) ? *cp - '0' : TOLOWER(*cp) - 'a' + 10;
		if (value >= base)
			break;
		result = result * base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;

	return result;
}


size_t strlen(const char *s)
{
	const char *sc;
	if(NULL == s)
		return 0;

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

char *strstr(const char *s1, const char *s2)
{
	size_t l1, l2;

	l2 = strlen(s2);
	if (!l2)
		return (char *)s1;
	l1 = strlen(s1);
	while (l1 >= l2) {
		l1--;
		if (!memcmp(s1, s2, l2))
			return (char *)s1;
		s1++;
	}
	return NULL;
}

char * strchar(char * s, char c)
{
	if(NULL == s)
	{
		return NULL;
	}
	char * tmp = s;
	while(*tmp != 0)
	{
		if(*tmp == c)
		{
			return tmp;
		}
		tmp++;
	}
	return NULL;
}

char * strtok(char *s, const char *delim)
{
	static char * this;
	char * tmp = NULL;
	if(NULL != s)
	{
		if(*delim == *s)
		{
			s += 1;
		}
		tmp = s;
	}
	else if(NULL != this)
	{
		s = this;
		tmp = this;
	}
	else
	{
		return NULL;
	}
	while(0 != *tmp)
	{
		if(*delim == *tmp)
		{
			*tmp = '\0';
			this = tmp++;
			return s;
		}
		tmp ++;
	}
	this = NULL;
	return s;
}

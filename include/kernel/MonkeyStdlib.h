#ifndef MONKEYSTDLIB_KERNEL_H
#define MONKEYSTDLIB_KERNEL_H

#define _INTSIZEOF(arg) ((sizeof(arg) + sizeof(int) - 1)&(~(sizeof(int) - 1)))
#define va_start(arg,s)	arg = (va_list)(((unsigned char *)&(s)) + _INTSIZEOF(s))
#define va_end			__builtin_va_end
#define va_arg			__builtin_va_arg
#define va_copy(d,s)	__builtin_va_copy((d),(s))
#define	va_list			__builtin_va_list
#define CHAR_BIT		8
#define	CHAR_MAX		(+127)
#define CHAR_MIN		0
#define INT_MAX			(+0x7fffffff)
#define INT_MIN			(-0x7fffffff)
#define SCHAR_MAX		(+127)
#define	SCHAR_MIN		(-127)
#define	SHRT_MAX		(+0x7fff)
#define SHRT_MIN		(-0x7fff)
#define UCHAR_MAX		(+0xff)
#define UINT_MAX		(+0xffffffff)
#define USHRT_MAX		(+0xffff)

typedef unsigned int size_t;

void itoa(unsigned int n, char *buf);
void xtoa(unsigned int n, char *buf);
int atoi(char*pstr);
int isDigit(unsigned char c);
int isLetter(unsigned char c);
void *memset(void *s, int c, unsigned int count);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
unsigned int strlen(const char *s);
int sprintf(char *str, const char *fmt, ...);
void _memcpy(char *s1, char *s2, int len);

#endif

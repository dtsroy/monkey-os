#include "kernel/MonkeyStdlib.h"

int sprintf(char *str, const char *fmt, ...) {
	int count=0, n, index=0, ret=2, num=0, len=0;
	char c, *s, buf[65], digit[16];
	char *backup = str;
	const char *backupfmt = fmt;
	unsigned char isClean=0;
begin: // 这个可以清空多余内容(但是效率减半)
	memset(buf, 0, sizeof(buf));
	memset(digit, 0, sizeof(digit));

	va_list ap;
	va_start(ap, fmt);

	while(*fmt) {
		if(*fmt == '%') {
			fmt++;
			switch(*fmt) {
			case 'd':
				n = va_arg(ap, int);
				if (n < 0) {
					*str = '-';
					str++;
					n = -n;
				}
				itoa(n, buf);
				_memcpy(str, buf, strlen(buf));
				str += strlen(buf);
				break;
			case 'c':
				c = va_arg(ap, int);
				*str = c;
				str++;
				break;
			case 'x':
				n = va_arg(ap, int);
				xtoa(n, buf);
				_memcpy(str, buf, strlen(buf));
				str += strlen(buf);
				break;
			case 's':
				s = va_arg(ap, char *);
				_memcpy(str, s, strlen(s));
				str += strlen(s);
				break;
			case '%':
				*str = '%';
				str++;
				break;
			case '0':
				index = 0;
				num = 0;
				memset(digit, 0, sizeof(digit));
				while(1) {
					fmt++;
					ret = isDigit(*fmt);
					if (ret) {
						digit[index] = *fmt;
						index++;
					} else {
						num = atoi(digit);
						break;
					}
				}
				switch(*fmt) {
				case 'd':
					n = va_arg(ap, int);
					if (n < 0) {
						*str = '-';
						str++;
						n = -n;
					}    
					itoa(n, buf);
					len = strlen(buf);
					if(len >= num) {
						_memcpy(str, buf, strlen(buf));
						str += strlen(buf);
					} else {
						memset(str, '0', num-len);
						str += num-len;
						_memcpy(str, buf, strlen(buf));
						str += strlen(buf);
					}
					break;
				case 'x':
					n = va_arg(ap, int);
					xtoa(n, buf);
					len = strlen(buf);
					if (len >= num) {
						_memcpy(str, buf, len);
						str += len;
					} else {
						memset(str, '0', num-len);
						str += num-len;
						_memcpy(str, buf, len);
						str += len;
					}
					break;
				case 's':
					s = va_arg(ap, char *);
					len = strlen(s);
					if(len >= num) {
						_memcpy(str, s, strlen(s));
						str += strlen(s);
					} else {
						memset(str, '0', num-len);
						str += num-len;
						_memcpy(str, s, strlen(s));
						str += strlen(s);
					}
					break;
				}
			}
		} else {
			*str = *fmt;
			str++;
		}
		fmt++;
	}
	va_end(ap);
	if (!isClean) {
		memset(backup, 0, strlen(backup));
		isClean = 1;
		str = backup;
		fmt = backupfmt;
		goto begin;
	}
	return count;
}

void itoa(unsigned int n, char *buf)
{
	int i;
	if (n < 10) {
		buf[0] = n + '0';
		buf[1] = '\0';
		return;
	}
	itoa(n / 10, buf);
	for (i=0; buf[i]!='\0'; i++);
	buf[i] = (n % 10) + '0';
	buf[i+1] = '\0';
}

int atoi(char *pstr)
{
	int int_ret = 0;
	signed char int_sign = 1;
	if (pstr == '\0') {return -1;}
	while (((*pstr) == ' ') || ((*pstr) == '\n') || ((*pstr) == '\t') || ((*pstr) == '\b')) {pstr++;}
	if (*pstr == '-') {int_sign = -1;}
	if (*pstr == '-' || *pstr == '+') {pstr++;}
	while (*pstr >= '0' && *pstr <= '9') {
		int_ret = int_ret * 10 + *pstr - '0';
		pstr++;
	}
	int_ret = int_sign * int_ret;
	return int_ret;
}

void xtoa(unsigned int n, char *buf) {
	int i;
	if (n < 16) {
		buf[0] = n<10 ? n+'0' : n-10+'a';
		buf[1] = '\0';
		return;
	}
	xtoa(n / 16, buf);
	for (i = 0; buf[i] != '\0'; i++);
	buf[i] = (n % 16) < 10 ? (n % 16) + '0' : (n % 16) - 10 + 'a';
	buf[i + 1] = '\0';
}

int isDigit(unsigned char c) {
	return c >= '0' && c <= '9';
}

int isLetter(unsigned char c) {
	return (c >= 'a' && c <= 'z')||(c >= 'A' && c <= 'Z');
}

void *memset(void *s, int c, unsigned int count) {
	char *xs = (char *) s;
	while (count--) {*xs++ = c;}
	return s;
}

char *strcpy(char *dest, const char *src) {
	char *tmp=dest;
	while (*dest++ = *src++) ;
	return tmp;
}

unsigned int strlen(const char *s) {
	int ret=0;
	while(*s++) {ret++;}
	return ret;
}

void _memcpy(char *s1, char *s2, int len) {
	while (len--) {*s1++ = *s2++;}
}

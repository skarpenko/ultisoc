/*
 * Copyright (c) 2018-2019 The UltiSoC Project. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * String handling routines
 */

#include <str.h>


size_t strlen(const char *s)
{
	size_t i;

	i = 0;
	while(s[i])
		++i;

	return i;
}


int strcmp(const char *cs, const char *ct)
{
	char res;

	while(1) {
		if((res = *cs - *ct++) != 0 || !*cs++)
			break;
	}

	return res;
}


void *memset(void *s, int c, size_t n)
{
	char *xs = (char *)s;

	for( ; n > 0; --n)
		*xs++ = (char)c;

	return s;
}


void *memmove(void *dst, const void *src, size_t n)
{
	const char *s = (const char*)src;
	char *d = (char*)dst;
	size_t i;

	if(dst == src || !n)
		return dst;

	if(dst > src) {
		do {
			--n;
			d[n] = s[n];
		} while(n);
	} else {
		for(i = 0; i < n; ++i)
			d[i] = s[i];
	}

	return dst;
}


/* Returns 1 if a string represents binary number */
static inline
int is_bin(const char *str, size_t l)
{
	if(l > 2 && str[0] == '0' && (str[1] == 'b' || str[1] == 'B'))
		return 1;
	else
		return 0;
}


/* Returns 1 if a string represents hexadecimal number */
static inline
int is_hex(const char *str, size_t l)
{
	if(l > 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
		return 1;
	else
		return 0;
}


/* Returns 1 if a string represents negative decimal */
static inline
int is_neg(const char *str)
{
	return (str[0] == '-');
}


/* Returns 1 if a string represents decimal number with sign */
static inline
int has_sign(const char *str)
{
	return (str[0] == '-') || (str[0] == '+');
}


/* Convert binary string to unsigned */
static int bin2u(const char *str, size_t l, unsigned *v)
{
	size_t i;
	for(i = 0; i < l && i < 32; ++i) {
		char c = str[l - i - 1];
		if(c == '1')
			c = 1;
		else if(c == '0')
			c = 0;
		else
			return -1;
		*v |= c << i;
	}
	return 0;
}


/* Convert hexadecimal string to unsigned */
static int hex2u(const char *str, size_t l, unsigned *v)
{
	size_t i;
	for(i = 0; i < l && i < 8; ++i) {
		char c = str[l - i - 1];
		if(c >= '0' && c <= '9')
			c -= '0';
		else if(c >= 'a' && c <= 'f')
			c -= 'a' - 10;
		else if(c >= 'A' && c <= 'F')
			c -= 'A' - 10;
		else
			return -1;
		*v |= c << (i<<2);
	}
	return 0;
}


/* Convert decimal string to unsigned */
static int dec2u(const char *str, size_t l, unsigned *v)
{
	size_t i;
	unsigned d = 1;
	for(i = 0; i < l && i < 10; ++i) {
		char c = str[l - i - 1];
		if(c >= '0' && c <= '9')
			c -= '0';
		else
			return -1;
		*v += c * d;
		d *= 10;
	}
	return 0;
}


/* Convert string number to unsigned */
int str2u(const char *str, unsigned *v)
{
	int ret;
	size_t l = strlen(str);

	if (!l)
		return -1;

	*v = 0;

	if (is_bin(str, l)) {
		return bin2u(&str[2], l - 2, v);
	} else if(is_hex(str, l)) {
		return hex2u(&str[2], l - 2, v);
	} else {
		ret = dec2u(&str[has_sign(str)], l - has_sign(str), v);
		if(!ret && is_neg(str))
			*v = (unsigned)-*v;
	}
	return ret;
}

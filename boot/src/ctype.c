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
 * Character handling functions
 */

#include <ctype.h>


/* Character attribute flags */
#define _U	0x01	/* upper case */
#define _L	0x02	/* lower case */
#define _D	0x04	/* digit */
#define _C	0x08	/* control */
#define _P	0x10	/* punctuation */
#define _S	0x20	/* white space (space/lf/tab) */
#define _X	0x40	/* hex digit */
#define _SP	0x80	/* hard space (0x20) */


/* Characters attribute map */
static unsigned char _ctype[] = {
	_C,_C,_C,_C,_C,_C,_C,_C,				/* 0-7 */
	_C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,			/* 8-15 */
	_C,_C,_C,_C,_C,_C,_C,_C,				/* 16-23 */
	_C,_C,_C,_C,_C,_C,_C,_C,				/* 24-31 */
	_S|_SP,_P,_P,_P,_P,_P,_P,_P,				/* 32-39 */
	_P,_P,_P,_P,_P,_P,_P,_P,				/* 40-47 */
	_D,_D,_D,_D,_D,_D,_D,_D,				/* 48-55 */
	_D,_D,_P,_P,_P,_P,_P,_P,				/* 56-63 */
	_P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,		/* 64-71 */
	_U,_U,_U,_U,_U,_U,_U,_U,				/* 72-79 */
	_U,_U,_U,_U,_U,_U,_U,_U,				/* 80-87 */
	_U,_U,_U,_P,_P,_P,_P,_P,				/* 88-95 */
	_P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,		/* 96-103 */
	_L,_L,_L,_L,_L,_L,_L,_L,				/* 104-111 */
	_L,_L,_L,_L,_L,_L,_L,_L,				/* 112-119 */
	_L,_L,_L,_P,_P,_P,_P,_C,				/* 120-127 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,			/* 128-143 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,			/* 144-159 */
	_S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,	/* 160-175 */
	_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,	/* 176-191 */
	_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,	/* 192-207 */
	_U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,	/* 208-223 */
	_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,	/* 224-239 */
	_L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L		/* 240-255 */
};


static inline
unsigned char ismask(int ch)
{
	return _ctype[(unsigned char)ch];
}


unsigned char tolower(unsigned char c)
{
	if(isupper(c))
		c -= 'A'-'a';
	return c;
}


unsigned char toupper(unsigned char c)
{
	if(islower(c))
		c -= 'a'-'A';
	return c;
}


int isalnum(int c)
{
	return ((ismask(c)&(_U|_L|_D)) != 0);
}


int isalpha(int c)
{
	return ((ismask(c)&(_U|_L)) != 0);
}


int iscntrl(int c)
{
	return ((ismask(c)&(_C)) != 0);
}


int isdigit(int c)
{
	return ((ismask(c)&(_D)) != 0);
}


int isgraph(int c)
{
	return ((ismask(c)&(_P|_U|_L|_D)) != 0);
}


int islower(int c)
{
	return ((ismask(c)&(_L)) != 0);
}


int isprint(int c)
{
	return ((ismask(c)&(_P|_U|_L|_D|_SP)) != 0);
}


int ispunct(int c)
{
	return ((ismask(c)&(_P)) != 0);
}


int isspace(int c)
{
	return ((ismask(c)&(_S)) != 0);
}


int isupper(int c)
{
	return ((ismask(c)&(_U)) != 0);
}


int isxdigit(int c)
{
	return ((ismask(c)&(_D|_X)) != 0);
}


int isbdigit(int c)
{
	return (c == '0' || c == '1');
}


int isascii(int c)
{
	return ((unsigned char)c <= 0x7f);
}


int toascii(int c)
{
	return ((unsigned char)c & 0x7f);
}

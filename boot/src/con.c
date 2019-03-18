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
 * Console
 */

#include <global.h>
#include <uart.h>
#include <con.h>


void con_init()
{
	G()->con.flags = CON_FLAGS_NONE;	/* Clear flags */
}


void con_set_flags(unsigned flags)
{
	G()->con.flags = flags;
}


unsigned con_get_flags()
{
	return G()->con.flags;
}


int con_getc_ex(unsigned flags)
{
	int ch;
	do {
		ch = uart_get_char();
	} while((flags & CON_GETC_EX_F_BLOCK) && ch < 0);

	if((ch >= 0) && (con_get_flags() & CON_FLAGS_ECHO))
		con_putc(ch);

	return ch;
}


void con_putc(char ch)
{
	uart_put_char(ch);
	if(ch == 0xA)
		uart_put_char(0xD);
}


void con_puts(const char* str)
{
	while(*str) {
		con_putc(*str);
		++str;
	}
}


void cprint_str(const char *str)
{
	if(str)
		con_puts(str);
	else
		con_puts("<NULL>");
}


void cprint_hex(unsigned hex)
{
	const static char h[] = "0123456789ABCDEF";
	con_putc(h[(hex & 0xF0000000)>>28]);
	con_putc(h[(hex & 0x0F000000)>>24]);
	con_putc(h[(hex & 0x00F00000)>>20]);
	con_putc(h[(hex & 0x000F0000)>>16]);
	con_putc(h[(hex & 0x0000F000)>>12]);
	con_putc(h[(hex & 0x00000F00)>>8]);
	con_putc(h[(hex & 0x000000F0)>>4]);
	con_putc(h[(hex & 0x0000000F)>>0]);
}


void cprint_int(int v)
{
	int d;
	int o = 1000000000;
	int f = 0;

	if(v<0)
		con_putc('-');

	while(o != 1) {
		d = v / o;
		v = v % o;
		o = o / 10;
		d = (d<0 ? -d : d);
		v = (v<0 ? -v : v);
		f = f | d;
		if(f) con_putc('0' + d);
	}
	con_putc('0' + v);
}


void cprint_uint(unsigned v)
{
	unsigned d;
	unsigned o = 1000000000;
	unsigned f = 0;

	while(o != 1) {
		d = v / o;
		v = v % o;
		o = o / 10;
		f = f | d;
		if(f) con_putc('0' + d);
	}
	con_putc('0' + v);
}


void cprint_int64(long long v)
{
	long long d;
	long long o = 1000000000000000000LL;
	long long f = 0;

	if(v<0)
		con_putc('-');

	while(o != 1) {
		d = v / o;
		v = v % o;
		o = o / 10;
		d = (d<0 ? -d : d);
		v = (v<0 ? -v : v);
		f = f | d;
		if(f) con_putc('0' + d);
	}
	con_putc('0' + v);
}


void cprint_uint64(unsigned long long v)
{
	unsigned long long d;
	unsigned long long o = 10000000000000000000ULL;
	unsigned long long f = 0;

	while(o != 1) {
		d = v / o;
		v = v % o;
		o = o / 10;
		f = f | d;
		if(f) con_putc('0' + d);
	}
	con_putc('0' + v);
}

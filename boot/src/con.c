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
#include <str.h>
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


static int __con_getc_ex(unsigned flags)
{
	int ch;

	do {
		ch = uart_get_char();
	} while((flags & CON_GETC_EX_F_BLOCK) && ch < 0);

	return ch;
}


int con_getc_ex(unsigned flags)
{
	int ch = __con_getc_ex(flags);

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


static inline void reset_esc_seq()
{
	struct console_data *cd = &G()->con;
	cd->nesc = 0;
}

static void flush_esc_seq()
{
	struct console_data *cd = &G()->con;

	if(con_get_flags() & CON_FLAGS_ECHO) {
		size_t i;
		for(i = 0; i < cd->nesc; ++i)
			uart_put_char(cd->esc[i]);
	}
	reset_esc_seq();
}

static int buf_del_char()
{
	struct console_data *cd = &G()->con;

	if(!cd->nc || cd->nc == cd->cur)
		return -1;

	memmove(&cd->iobuf[cd->cur], &cd->iobuf[cd->cur + 1], cd->nc - cd->cur);

	--cd->nc;

	return 0;
}

static int buf_cur_l()
{
	struct console_data *cd = &G()->con;

	if(!cd->nc || !cd->cur)
		return -1;

	--cd->cur;

	return 0;
}

static int buf_cur_r()
{
	struct console_data *cd = &G()->con;

	if(!cd->nc || cd->cur == cd->nc)
		return -1;

	++cd->cur;

	return 0;
}

static int buf_bcksp()
{
	struct console_data *cd = &G()->con;

	if(!cd->nc || !cd->cur)
		return -1;

	if(cd->nc != cd->cur)
		memmove(&cd->iobuf[cd->cur - 1], &cd->iobuf[cd->cur], cd->nc - cd->cur);

	--cd->nc;
	--cd->cur;

	return 0;
}

static int buf_insert(char ch)
{
	struct console_data *cd = &G()->con;

	if(cd->nc == CONFIG_CONIOBUF_SZ - 1)
		return -1;

	if(cd->nc != cd->cur) {
		memmove(&cd->iobuf[cd->cur + 1], &cd->iobuf[cd->cur], cd->nc - cd->cur);
		/* Send insert sequence */
		cd->nesc = 0;
		cd->esc[cd->nesc++] = 27;
		cd->esc[cd->nesc++] = '[';
		cd->esc[cd->nesc++] = '1';
		cd->esc[cd->nesc++] = '@';
	}

	cd->iobuf[cd->cur] = ch;
	++cd->nc;
	++cd->cur;

	return 0;
}

char *con_get_iobuf()
{
	struct console_data *cd = &G()->con;
	int echo = (con_get_flags() & CON_FLAGS_ECHO);
	int ch;

	while((ch = __con_getc_ex(CON_GETC_EX_F_BLOCK)) != 0xD) {
		int ret = -1;

		/* ESC sequence */
		if(ch == 27) {									/* ESC */
			cd->nesc = 0;
			cd->esc[cd->nesc++] = (char)ch;
		} else if(cd->nesc) {
			if(ch == '[' && cd->nesc == 1) {
				cd->esc[cd->nesc++] = (char)ch;
			} else if(ch >= '1' && ch <= '9' && cd->nesc == 2) {
				cd->esc[cd->nesc++] = (char)ch;
			} else if(ch == '~' && cd->nesc == 3 && cd->esc[cd->nesc-1] == '3') {	/* DEL */
				/* Send delete sequence */
				cd->nesc = 0;
				cd->esc[cd->nesc++] = 27;
				cd->esc[cd->nesc++] = '[';
				cd->esc[cd->nesc++] = '1';
				cd->esc[cd->nesc++] = 'P';

				if(buf_del_char() < 0)
					reset_esc_seq();
				else
					flush_esc_seq();
			} else if(ch == '~' && cd->nesc == 3 && cd->esc[cd->nesc-1] == '1') {	/* HOME */
				reset_esc_seq();
			} else if(ch == '~' && cd->nesc == 3 && cd->esc[cd->nesc-1] == '4') {	/* END */
				reset_esc_seq();
			} else if(ch == 'D') {							/* <--- */
				cd->esc[cd->nesc++] = (char)ch;
				if(buf_cur_l() < 0)
					reset_esc_seq();
				else
					flush_esc_seq();
			} else if(ch == 'C') {							/* ---> */
				cd->esc[cd->nesc++] = (char)ch;
				if(buf_cur_r() < 0)
					reset_esc_seq();
				else
					flush_esc_seq();
			} else {
				reset_esc_seq();
			}
		} else if(ch == 127) {								/* BACKSPACE */
			ret = buf_bcksp();
		} else {
			ret = buf_insert((char)ch);
			flush_esc_seq();
		}

		if(echo && ret == 0)
			uart_put_char((char)ch);
	}

	cd->iobuf[cd->nc] = 0;
	cd->nc = 0;
	cd->nesc = 0;
	cd->cur = 0;

	return cd->iobuf;
}


void cprint_str(const char *str)
{
	if(str)
		con_puts(str);
	else
		con_puts("<NULL>");
}


void cprint_hex(unsigned hex, size_t nn)
{
	const static char h[] = "0123456789ABCDEF";
	switch(nn) {
		default:
		case 8: con_putc(h[(hex & 0xF0000000)>>28]);
		case 7: con_putc(h[(hex & 0x0F000000)>>24]);
		case 6: con_putc(h[(hex & 0x00F00000)>>20]);
		case 5: con_putc(h[(hex & 0x000F0000)>>16]);
		case 4: con_putc(h[(hex & 0x0000F000)>>12]);
		case 3: con_putc(h[(hex & 0x00000F00)>>8]);
		case 2: con_putc(h[(hex & 0x000000F0)>>4]);
		case 1: con_putc(h[(hex & 0x0000000F)>>0]);
		case 0: ;
	}
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


void cprint_strf(const char *str, size_t w)
{
	if(w && str) {
		unsigned i, c;
		for(c = 0, i = 0; i < w; ++i)
			if(str[c])
				con_putc(str[c++]);
			else
				con_putc(' ');
	}
}

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

#ifndef _BOOTROM_CON_H_
#define _BOOTROM_CON_H_


/* Init console */
void con_init();

/* Set console flags */
void con_set_flags(unsigned flags);

/* Get console flags */
unsigned con_get_flags();

/* Console flags */
#define CON_FLAGS_NONE		(0x0)	/* No flags */
#define CON_FLAGS_ECHO		(0X1)	/* Echo */


/*
 * Get char
 * Return value depends on flags
 */
int con_getc_ex(unsigned flags);

/* con_getc_ex() flags */
#define CON_GETC_EX_F_NONE	(0x0)	/* No flags */
#define CON_GETC_EX_F_BLOCK	(0x1)	/* Block until character received */


/* Get char
 * Returns either 8-bit char or -1.
 */
static inline
int con_getc()
{
	return con_getc_ex(CON_GETC_EX_F_NONE);
}


/* Get char
 * Blocks if no characters available.
 */
static inline
char con_getc_b()
{
	return (char)con_getc_ex(CON_GETC_EX_F_BLOCK);
}


/* Put char */
void con_putc(char ch);


/* Put string */
void con_puts(const char* str);


/* Print string */
void cprint_str(const char *str);


/* Print HEX value */
void cprint_hex(unsigned hex);


/* Print integer value */
void cprint_int(int v);


/* Print unsigned value */
void cprint_uint(unsigned v);


/* Print 64-bit integer value */
void cprint_int64(long long v);


/* Print 64-bit unsigned value */
void cprint_uint64(unsigned long long v);


#endif /* _BOOTROM_CON_H_ */

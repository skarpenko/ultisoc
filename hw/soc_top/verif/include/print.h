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
 * Print functions
 */

#ifndef _VERIF_PRINT_H_
#define _VERIF_PRINT_H_

#include <arch.h>
#include <soc_regs.h>


static inline
void print_init()
{
	writel(27, USOC_UART_DIVD);	/* 115200bps */
}


static inline
void print_char(char ch)
{
	while((readl(USOC_UART_CTRL) & 0x2) != 0)
		;
	writel(ch, USOC_UART_DATA);
	if(ch == 0x0A) {
		while((readl(USOC_UART_CTRL) & 0x2) != 0)
			;
		writel(0x0D, USOC_UART_DATA);
	}
}


static inline
void print_str(const char *str)
{
	if(str) {
		while(*str) {
			print_char(*str);
			++str;
		}
	} else
		print_str("<NULL>");
}


#endif /* _VERIF_PRINT_H_ */

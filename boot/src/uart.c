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
 * UltiSoC UART support
 */

#include <arch.h>
#include <soc_regs.h>
#include <uart.h>


void uart_init()
{
	u32 sys_freq = readl(USOC_CTRL_SYSFREQ);	/* UART frequency */
	u32 divq, divr;

	/* Set UART control reg */
	writel((USOC_UART_CTRL_TX_IM | USOC_UART_CTRL_RX_IM), USOC_UART_CTRL);

	/* Calculate divider for 115200bps */
	divq = sys_freq / (16 * 115200);
	divr = sys_freq % (16 * 115200);
	if(divr > (16 * 115200 / 2)) ++divq;

	/* Set divider */
	writel(divq, USOC_UART_DIVD);
}


int uart_get_char()
{
	int ret = -1;
	if(!(readl(USOC_UART_CTRL) & USOC_UART_CTRL_RX_FE))
		ret = readl(USOC_UART_DATA);
	return ret;
}


void uart_put_char(char ch)
{
	while(readl(USOC_UART_CTRL) & USOC_UART_CTRL_TX_FF)
		;
	writel(ch, USOC_UART_DATA);
}

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
 * XModem related commands
 */

#include <stddef.h>
#include <arch.h>
#include <soc_info.h>
#include <uart.h>
#include <str.h>
#include <con.h>
#include <cmd_types.h>
#include <xmodem.h>



/* Send byte */
static void outbyte(struct xm_recvr *xr, char b)
{
	uart_put_char(b);
}


/* Receive byte */
static int inbyte(struct xm_recvr *xr, unsigned timeout)
{
	int ch;

	timeout = timeout * soc_sys_freq() + rdtsc_lo();

	do {
		ch = uart_get_char();
	} while(ch < 0 && (long)rdtsc_lo() - (long)timeout < 0);

	return ch;
}


/* Load arbitrary data using XModem protocol */
static int cmd_xmodem(struct cmd_args *args)
{
	unsigned addr;
	int res;
	struct xm_recvr xmr;

	if(args->n < 2) {
		cprint_str("Insufficient arguments.\n");
		return -1;
	}

	/* Parse address */
	if(str2u(args->args[1], &addr) < 0) {
		cprint_str("Invalid argument: ");
		cprint_str(args->args[1]);
		cprint_str("\n");
		return -1;
	}

	/* Prepare XModem */
	xm_recvr_init(&xmr, outbyte, inbyte);

	cprint_str("XModem: [0x"); cprint_hex32(addr); cprint_str("] -> ");

	/* Start receiver */
	res = xm_recvr_start_rx(&xmr, (void*)addr);
	switch(res) {
		case XM_ERR_EOT:
			cprint_str("\nDone.\n");
			break;
		case XM_ERR_CAN:
			cprint_str("\nTransmission canceled.\n");
			break;
		case XM_ERR_OOSEQ:
			cprint_str("\nSequence error.\n");
			break;
		case XM_ERR_RETR:
			cprint_str("\nMaximum retries reached.\n");
			break;
		default: ;
	}

	return 0;
}
COMMAND(x0xmod, "xmodem", "xmodem <addr>", "load data over XModem protocol", cmd_xmodem);

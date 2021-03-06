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
#include <elf_stream.h>
#include <soc_info.h>
#include <global.h>



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


/* Called from ELF stream to set new destination buffer address */
static void set_buf_cb(void *udata, unsigned long addr)
{
	struct xm_recvr *xmr = (struct xm_recvr*)udata;
	xm_recvr_setbuf(xmr, (void*)addr);
}


/* Called from XModem for each received block */
int xmodem_cb(struct xm_recvr *xmr, void *buf, size_t size)
{
	struct elf_stream *es = (struct elf_stream*)xm_recvr_getudata(xmr);
	return elf_stream_parse(es, buf, size);
}


/* Load MIPS-I ELF using XModem protocol */
static int cmd_xelf(struct cmd_args *args)
{
	struct xm_recvr xmr;
	struct elf_stream es;
	int res;

	/* Prepare XModem */
	xm_recvr_init(&xmr, outbyte, inbyte);
	xm_recvr_setucb(&xmr, xmodem_cb);
	xm_recvr_setudata(&xmr, &es);

	/* Prepare ELF loader */
	elf_stream_init(&es, set_buf_cb, &xmr);

	cprint_str("XModem: [ELF] -> ");

	/* Start receiver */
	res = xm_recvr_start_rx(&xmr, (void*)soc_ram_base());
	switch(res) {
		case XM_ERR_CAN:
			cprint_str("\nTransmission canceled.\n");
			break;
		case XM_ERR_OOSEQ:
			cprint_str("\nSequence error.\n");
			break;
		case XM_ERR_RETR:
			cprint_str("\nMaximum retries reached.\n");
			break;
		case ELFS_LOADED:
			G()->elf_entry = es.entry;	/* Set ELF entry point */
			cprint_str("\nELF loaded.\n");
			break;
		case ELFS_INV_FILE_FMT:
			cprint_str("\nInvalid ELF format.\n");
			break;
		case ELFS_INV_CLASS:
			cprint_str("\nInvalid ELF class.\n");
			break;
		case ELFS_INV_DATA_ENC:
			cprint_str("\nInvalid data encoding.\n");
			break;
		case ELFS_INV_VERSION:
			cprint_str("\nInvalid ELF version.\n");
			break;
		case ELFS_INV_ARCH_TYPE:
			cprint_str("\nInvalid target architecture.\n");
			break;
		case ELFS_INV_OBJ_TYPE:
			cprint_str("\nInvalid ELF object type.\n");
			break;
		case ELFS_INV_LAYOUT:
			cprint_str("\nInvalid ELF object layout.\n");
			break;
		default:
			cprint_str("\nELF not loaded.\n");
			break;
	}

	return 0;
}
COMMAND(x0xelf, "xelf", "xelf", "load ELF binary over XModem protocol", cmd_xelf);

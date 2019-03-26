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
 * Memory related commands
 */

#include <stddef.h>
#include <con.h>
#include <str.h>
#include <ctype.h>
#include <cmd_types.h>


/* Dump memory contents */
static int cmd_memdump(struct cmd_args *args)
{
	unsigned addr;
	unsigned len;
	unsigned l;
	unsigned pp = (unsigned)-1;
	unsigned pl = 0;

	if(args->n < 3) {
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

	/* Parse length */
	if(str2u(args->args[2], &len) < 0) {
		cprint_str("Invalid argument: ");
		cprint_str(args->args[2]);
		cprint_str("\n");
		return -1;
	}

	/* Parse lines per page if specified */
	if(args->n > 3) {
		if(str2u(args->args[3], &pp) < 0) {
			cprint_str("Invalid argument: ");
			cprint_str(args->args[3]);
			cprint_str("\n");
			return -1;
		}
		if(!pp)
			pp = (unsigned)-1;
	}

	/* Dump contents */
	for(l = 0; l < len; l += 16) {
		unsigned char *p = (unsigned char*)addr;
		unsigned n = (len - l < 16 ? len - l : 16);
		unsigned i;

		++pl;

		/* Address */
		cprint_hex32(addr); cprint_str(":");

		/* Print hex */
		for(i = 0; i < 16; ++i) {
			cprint_str(" ");
			if(i < n)
				cprint_hex8(p[i]);
			else
				cprint_str("  ");
			if(i != 15 && i % 4 == 3)
				cprint_str(" |");
		}

		cprint_str(" ");

		/* Print printable characters */
		for(i = 0; i < 16; ++i) {
			if(i < n)
				cprint_char(isprint(p[i]) ? p[i] : '.');
			else
				cprint_str(" ");
		}

		cprint_str("\n");

		addr += 16;

		if(pl == pp) {
			char ch;
			pl = 0;
			cprint_str("Press any key to continue or 'q' to quit...\n");
			ch = con_getc_b();
			if(ch == 'q' || ch == 'Q') {
				cprint_str("\n");
				break;
			}
		}
	}

	return 0;
}
COMMAND(m0memdu, "memdump", "memdump <addr> <len> [per_page]", "dump memory contents", cmd_memdump);

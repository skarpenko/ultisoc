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
 * Disassembler related commands
 */

#include <stddef.h>
#include <con.h>
#include <str.h>
#include <disasm.h>
#include <cmd_types.h>



/* Disassembly command */
static int cmd_dis(struct cmd_args *args)
{
	unsigned addr;
	unsigned inum;
	unsigned pp = (unsigned)-1;

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

	/* Parse instructions number */
	if(str2u(args->args[2], &inum) < 0) {
		cprint_str("Invalid argument: ");
		cprint_str(args->args[2]);
		cprint_str("\n");
		return -1;
	}

	if(!inum)
		return 0;

	/* Parse instructions per page if specified */
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

	if(pp > inum)
		pp = inum;

	/* Disassemble */
	do {
		disasm((void*)addr, pp);
		addr += pp << 2;
		pp = (pp > inum ? inum : pp);
		inum -= pp;
		if(inum) {
			char ch;
			cprint_str("Press any key to continue or 'q' to quit...\n");
			ch = con_getc_b();
			if(ch == 'q' || ch == 'Q') {
				cprint_str("\n");
				break;
			}
		}
	} while(inum);

	return 0;
}
COMMAND(d0dis0, "disasm", "disasm <addr> <num> [per_page]", "disassemble instructions (aliases: 'dis', 'd')", cmd_dis);
COMMAND(d0dis1, "dis", NULL, NULL, cmd_dis);
COMMAND(d0dis2, "d", NULL, NULL, cmd_dis);


/* Print register names */
static int cmd_regnames(struct cmd_args *args)
{
	int i;

	cprint_str("General purpose registers:\n");
	for(i = 0; i < DISASM_GPRS_NR; ++i) {
		if(i) {
			cprint_str(", ");
			if(!(i%12))
				cprint_str("\n");
		}
		cprint_str(disasm_gprs[i]);
	}

	cprint_str("\n");
	cprint_str("\n");

	cprint_str("Coprocessor 0 registers:\n");
	for(i = 8; i < 16; ++i) {
		if(i > 8)
			cprint_str(", ");
		cprint_str(disasm_c0rs[i]);
	}
	cprint_str("\n");

	return 0;
}
COMMAND(d1regn, "regnames", "regnames", "print CPU register names", cmd_regnames);

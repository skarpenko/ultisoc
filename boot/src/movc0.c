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
 * Move to/from Coprocessor 0 commands
 */

#include <stddef.h>
#include <con.h>
#include <str.h>
#include <disasm.h>
#include <compiler.h>
#include <cmd_types.h>


static inline
unsigned mfc0(unsigned reg)
{
	unsigned v;
	union instruction snippet[2];

	/* jr ra */
	snippet[0].word = 0;
	snippet[0].r.op = 0;
	snippet[0].r.func = 8;
	snippet[0].r.rs = 31;
	/* mfc0 t0, <reg>  (delay slot) */
	snippet[1].word = 0;
	snippet[1].r.op = 16;
	snippet[1].r.rs = 0;	/* mfc0 */
	snippet[1].r.rt = 8;	/* t0 */
	snippet[1].r.rd = reg;	/* <reg> */

	barrier();	/* Prevent compiler optimizations on snippet */

	/* Execute snippet */
	__asm__ __volatile__ (
		".set push       ;"
		".set noreorder  ;"
		"jalr %1         ;"
		"nop             ;"
		"move %0, $t0    ;"
		".set pop        ;"
		: "=r" (v)
		: "r" ((unsigned)&snippet[0])
		: "$t0"
	);

	return v;
}


static inline
void mtc0(unsigned v, unsigned reg)
{
	union instruction snippet[2];

	/* jr ra */
	snippet[0].word = 0;
	snippet[0].r.op = 0;
	snippet[0].r.func = 8;
	snippet[0].r.rs = 31;
	/* mtc0 t0, <reg>  (delay slot) */
	snippet[1].word = 0;
	snippet[1].r.op = 16;
	snippet[1].r.rs = 4;	/* mtc0 */
	snippet[1].r.rt = 8;	/* t0 */
	snippet[1].r.rd = reg;	/* <reg> */

	barrier();	/* Prevent compiler optimizations on snippet */

	/* Execute snippet */
	__asm__ __volatile__ (
		".set push       ;"
		".set noreorder  ;"
		"move $t0, %0    ;"
		"jalr %1         ;"
		"nop             ;"
		".set pop        ;"
		:
		: "r" (v), "r" ((unsigned)&snippet[0])
		: "$t0"
	);
}


/* Move from CP0 */
static int cmd_mfc0(struct cmd_args *args)
{
	unsigned reg_no = 32;
	unsigned v;

	if(args->n < 2) {
		cprint_str("Insufficient arguments.\n");
		return -1;
	}

	/* Try to parse register as number */
	if(str2u(args->args[1], &reg_no) < 0) {
		unsigned i;
		for(i = 0; i < DISASM_C0RS_NR; ++i) {
			if(strcmp(args->args[1], disasm_c0rs[i]) == 0) {
				reg_no = i;
				break;
			}
		}

		if(reg_no > 31) {
			cprint_str("Invalid argument: ");
			cprint_str(args->args[1]);
			cprint_str("\n");
			return -1;
		}
	}

	v = mfc0(reg_no);

	cprint_str("CP0: ");
	cprint_str(disasm_c0rs[reg_no]); cprint_str(" = 0x"); cprint_hex32(v); cprint_str("\n");

	return 0;
}
COMMAND(c0mfc, "mfc0", "mfc0 <reg_name|num>", "read value from CP0 register", cmd_mfc0);


/* Move to CP0 */
static int cmd_mtc0(struct cmd_args *args)
{
	unsigned reg_no = 32;
	unsigned v;

	if(args->n < 3) {
		cprint_str("Insufficient arguments.\n");
		return -1;
	}

	/* Parse value */
	if(str2u(args->args[1], &v) < 0) {
		cprint_str("Invalid argument: ");
		cprint_str(args->args[1]);
		cprint_str("\n");
		return -1;
	}

	/* Try to parse register as number */
	if(str2u(args->args[2], &reg_no) < 0) {
		unsigned i;
		for(i = 0; i < DISASM_C0RS_NR; ++i) {
			if(strcmp(args->args[2], disasm_c0rs[i]) == 0) {
				reg_no = i;
				break;
			}
		}

		if(reg_no > 31) {
			cprint_str("Invalid argument: ");
			cprint_str(args->args[2]);
			cprint_str("\n");
			return -1;
		}
	}

	mtc0(v, reg_no);

	cprint_str("CP0: ");
	cprint_str("0x"); cprint_hex32(v); cprint_str(" --> "); cprint_str(disasm_c0rs[reg_no]); cprint_str("\n");

	return 0;
}
COMMAND(c0mtc, "mtc0", "mtc0 <value> <reg_name|num>", "write value to CP0 register", cmd_mtc0);

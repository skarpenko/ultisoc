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
 * Jump to address commands
 */

#include <stddef.h>
#include <con.h>
#include <str.h>
#include <cmd_types.h>
#include <global.h>


/* Jump */
static int cmd_jmp(struct cmd_args *args)
{
	unsigned addr;

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

	/* Do jump */
	__asm__ __volatile__ (
		".set push       ;"
		".set noreorder  ;"
		"jalr %0         ;"
		"nop             ;"
		".set pop        ;"
		:
		: "r" (addr)
		:
	);

	return 0;
}
COMMAND(j0jmp, "jmp", "jmp <addr>", "jump to address", cmd_jmp);


/* Run */
static int cmd_run(struct cmd_args *args)
{
	unsigned long addr = G()->elf_entry;

	if(!addr) {
		cprint_str("ELF binary is not loaded.\n");
		return -1;
	}

	/* Do jump */
	__asm__ __volatile__ (
		".set push       ;"
		".set noreorder  ;"
		"jalr %0         ;"
		"nop             ;"
		".set pop        ;"
		:
		: "r" (addr)
		:
	);

	return 0;
}
COMMAND(j0run, "run", "run", "run loaded ELF binary", cmd_run);

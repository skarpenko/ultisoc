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
 * ROM linker script
 */


OUTPUT_FORMAT("elf32-littlemips", "elf32-bigmips",
		"elf32-littlemips")
OUTPUT_ARCH(mips)
ENTRY(__reset)


MEMORY
{
	rom(rwx): ORIGIN = 0x00000000, LENGTH = 32K
}


SECTIONS
{
	.startup	0x00000000 : { *(.startup .startup.*) } > rom
	.text		ALIGN(0x4) : { *(.text .text.*) } > rom
	.rodata		ALIGN(0x4) : { *(.rodata .rodata.*) } > rom
	.rodata1	ALIGN(0x4) : { *(.rodata1) } > rom
	.cmd_table	ALIGN(0x4) : {
		__CMD_TABLE_BEGIN = ABSOLUTE(.);
		KEEP(*(SORT(.cmd_table*)))
		__CMD_TABLE_END = ABSOLUTE(.);
	} > rom

	/DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.reginfo) }
}

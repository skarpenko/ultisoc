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
 * Ultiparc disassembler
 */

#ifndef _BOOTROM_DISASM_H_
#define _BOOTROM_DISASM_H_

#include <arch.h>


/*
 * R-Type instruction (Register)
 *
 * 31          26 25       21 20       16 15       11 10        6 5            0
 * +-------------+-----------+-----------+-----------+-----------+-------------+
 * |     op      |    rs     |    rt     |    rd     |   shamt   |    funct    |
 * +-------------+-----------+-----------+-----------+-----------+-------------+
 */
struct rtype_instr {
	u32 func  : 6;
	u32 shamt : 5;
	u32 rd    : 5;
	u32 rt    : 5;
	u32 rs    : 5;
	u32 op    : 6;
};


/*
 * I-Type instruction (Immediate)
 *
 * 31          26 25       21 20       16 15                                   0
 * +-------------+-----------+-----------+-------------------------------------+
 * |     op      |    rs     |    rt     |              immediate              |
 * +-------------+-----------+-----------+-------------------------------------+
 */
struct itype_instr {
	u32 imm : 16;
	u32 rt  : 5;
	u32 rs  : 5;
	u32 op  : 6;
};


/*
 * J-Type instruction (Jump)
 *
 * 31          26 25                                                           0
 * +-------------+-------------------------------------------------------------+
 * |     op      |                         target                              |
 * +-------------+-------------------------------------------------------------+
 */
struct jtype_instr {
	u32 target : 26;
	u32 op     : 6;
};


union instruction {
	u32 word;
	struct rtype_instr r;
	struct itype_instr i;
	struct jtype_instr j;
};


#define DISASM_GPRS_NR		(32)	/* Number of general purpose registers */
#define DISASM_C0RS_NR		(32)	/* Number of Coprocessor 0 registers */


extern const char* disasm_gprs[];	/* General purpose register names */
extern const char* disasm_c0rs[];	/* Coprocessor 0 register names */


/* Disassemble instruction */
int disasm_instr(u32 instr, u32 addr);


/* Disassemble number of instructions at address */
void disasm(void *ptr, unsigned ni);


#endif /* _BOOTROM_DISASM_H_ */

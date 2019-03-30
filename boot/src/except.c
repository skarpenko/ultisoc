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
 * Exceptions handler
 */

#include <stddef.h>
#include <arch.h>
#include <con.h>
#include <disasm.h>


#define DIS_INSTR_BEFORE	8	/* Number of instructions before faulting instruction to disassemble */
#define DIS_INSTR_AFTER		8	/* Number of instructions after faulting instruction to disassemble */

#define CAUSE_BD_MASK		0x80000000	/* Branch Delay mask */


static const char *cause_str(u32 vec)
{
	switch (vec) {
		case 0:
			return "Reset";
		case 1:
			return "Bus Error";
		case 2:
			return "Integer Overflow";
		case 3:
			return "Address Error";
		case 4:
			return "Unimplemented Instruction";
		case 5:
			return "Breakpoint Trap";
		case 6:
			return "System Call Trap";
		case 7:
			return "Hardware Interrupt";
		default:
			return "<Unknown>";
	}
}


void interrupt_entry(struct interrupt_frame *p)
{
	char ch;

	cprint_str("\n*** EXCEPTION *\n");
	cprint_str("Type: "); cprint_str(cause_str(p->vec)); cprint_str("\n");
	cprint_str("Faulting instruction: 0x"); cprint_hex32(p->epc + (p->cause & CAUSE_BD_MASK ? 4 : 0)); cprint_str("\n");

	cprint_str("   at: 0x"); cprint_hex32(p->at); cprint_str("     v0: 0x"); cprint_hex32(p->v0);
	cprint_str("     v1: 0x"); cprint_hex32(p->v1); cprint_str("     a0: 0x"); cprint_hex32(p->a0);
	cprint_str("\n");

	cprint_str("   a1: 0x"); cprint_hex32(p->a1); cprint_str("     a2: 0x"); cprint_hex32(p->a2);
	cprint_str("     a3: 0x"); cprint_hex32(p->a3); cprint_str("     t0: 0x"); cprint_hex32(p->t0);
	cprint_str("\n");

	cprint_str("   t1: 0x"); cprint_hex32(p->t1); cprint_str("     t2: 0x"); cprint_hex32(p->t2);
	cprint_str("     t3: 0x"); cprint_hex32(p->t3); cprint_str("     t4: 0x"); cprint_hex32(p->t4);
	cprint_str("\n");

	cprint_str("   t5: 0x"); cprint_hex32(p->t5); cprint_str("     t6: 0x"); cprint_hex32(p->t6);
	cprint_str("     t7: 0x"); cprint_hex32(p->t7); cprint_str("     t8: 0x"); cprint_hex32(p->t8);
	cprint_str("\n");

	cprint_str("   t9: 0x"); cprint_hex32(p->t9); cprint_str("     ra: 0x"); cprint_hex32(p->ra);
	cprint_str("     hi: 0x"); cprint_hex32(p->hi); cprint_str("     lo: 0x"); cprint_hex32(p->lo);
	cprint_str("\n");

	cprint_str("  epc: 0x"); cprint_hex32(p->epc); cprint_str("  cause: 0x"); cprint_hex32(p->cause);
	cprint_str("     sr: 0x"); cprint_hex32(p->sr); cprint_str("    psr: 0x"); cprint_hex32(p->psr);
	cprint_str("\n");

	cprint_str("***\n");

	cprint_str("Press any key to restart or 'd' to disassemble...\n");
	ch = con_getc_b();
	if(ch == 'd' || ch == 'D') {
		addr_t fault_addr = p->epc + (p->cause & CAUSE_BD_MASK ? 4 : 0);
		addr_t addr = fault_addr;
		addr -= DIS_INSTR_BEFORE * 4;
		cprint_str("\nDisassembly:\n");
		for( ; addr <= fault_addr + DIS_INSTR_AFTER * 4; addr += 4) {
			u32 *instr = (u32*)addr;
			cprint_str(addr == fault_addr ? "=>" : "  ");
			cprint_hex32(addr); cprint_strf(":", 6);
			cprint_hex32(*instr); cprint_strf("", 6);
			disasm_instr(*instr, addr);
			cprint_str("\n");
		}
		cprint_str("Press any key to restart...\n");
		con_getc_b();
	}
}

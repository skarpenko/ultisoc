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


#define CAUSE_BD_MASK	0x80000000


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

}

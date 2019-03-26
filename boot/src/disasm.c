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

#include <con.h>
#include <disasm.h>


#define IFLDW	(8)	/* Instruction field width */


const char* disasm_gprs[DISASM_GPRS_NR] = {
	"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
};


const char* disasm_c0rs[DISASM_C0RS_NR] = {
	"$0", "$1", "$2", "$3", "$4", "$5", "$6", "$7",
	"tsclo", "tschi", "ivtb", "psr", "sr", "cause",
	"epc", "prid", "$16", "$17", "$18", "$19", "$20",
	"$21", "$22", "$23", "$24", "$25", "$26", "$27",
	"$28", "$29", "$30", "$31"
};


static inline
u32 sign_extend16(u32 imm)
{
	return (imm & (1<<15)) ? imm | 0xFFFF0000 : imm;
}


static inline
u32 zero_extend16(u32 imm)
{
	return imm;
}


static int disasm_mfc0(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.func || iw.r.shamt)
		return -1;

	cprint_strf("mfc0", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_c0rs[iw.r.rd]);

	return 0;
}


static int disasm_mtc0(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.func || iw.r.shamt)
		return -1;

	cprint_strf("mtc0", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_c0rs[iw.r.rd]);

	return 0;
}


static int disasm_rfe(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.func == 16 && !iw.r.rd && !iw.r.rt && !iw.r.shamt) {
		cprint_str("rfe");
		return 0;
	} else
		return -1;
}


static int disasm_wait(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.func == 32 && !iw.r.rd && !iw.r.rt && !iw.r.shamt) {
		cprint_str("wait");
		return 0;
	} else
		return -1;
}


static int disasm_sll(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.rs)
		return -1;

	if(iw.r.rd || iw.r.rt) {
		cprint_strf("sll", IFLDW);
		cprint_str(disasm_gprs[iw.r.rd]);
		cprint_str(", ");
		cprint_str(disasm_gprs[iw.r.rt]);
		cprint_str(", ");
		cprint_int(iw.r.shamt);
	} else
		cprint_str("nop");

	return 0;
}


static int disasm_srl(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.rs)
		return -1;

	cprint_strf("srl", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_int(iw.r.shamt);

	return 0;
}


static int disasm_sra(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.rs)
		return -1;

	cprint_strf("sra", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_int(iw.r.shamt);

	return 0;
}


static int disasm_sllv(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	cprint_strf("sllv", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_srlv(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	cprint_strf("srlv", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_srav(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	cprint_strf("srav", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_jr(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.rt || iw.r.rd)
		return -1;

	cprint_strf("jr", IFLDW);
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_jalr(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.rd)
		return -1;

	cprint_strf("jalr", IFLDW);
	if(iw.r.rd != 31) {
		cprint_str(disasm_gprs[iw.r.rd]);
		cprint_str(", ");
	}
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;

}


static int disasm_syscall(u32 instr)
{
	cprint_str("syscall");
	return 0;
}


static int disasm_break(u32 instr)
{
	cprint_str("break");
	return 0;
}


static int disasm_mfhi(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.rs || iw.r.rt)
		return -1;

	cprint_strf("mfhi", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);

	return 0;
}


static int disasm_mthi(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.rd || iw.r.rt)
		return -1;

	cprint_strf("mthi", IFLDW);
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_mflo(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.rs || iw.r.rt)
		return -1;

	cprint_strf("mflo", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);

	return 0;
}


static int disasm_mtlo(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.rd || iw.r.rt)
		return -1;

	cprint_strf("mtlo", IFLDW);
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_mult(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.rd)
		return -1;

	cprint_str("mult");

	return 0;
}


static int disasm_multu(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.rd)
		return -1;

	cprint_str("multu");

	return 0;
}


static int disasm_div(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.rd)
		return -1;

	cprint_str("div");

	return 0;
}


static int disasm_divu(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.r.rd)
		return -1;

	cprint_str("divu");

	return 0;
}


static int disasm_add(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	cprint_strf("add", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_addu(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	cprint_strf("addu", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_sub(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	cprint_strf("sub", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_subu(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	cprint_strf("subu", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_and(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	cprint_strf("and", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_or(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	cprint_strf("or", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_xor(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	cprint_strf("xor", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_nor(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	cprint_strf("nor", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_slt(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	cprint_strf("slt", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_sltu(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	cprint_strf("sltu", IFLDW);
	cprint_str(disasm_gprs[iw.r.rd]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);

	return 0;
}


static int disasm_bltz(u32 instr, u32 addr)
{
	u32 target;
	union instruction iw;
	iw.word = instr;

	target = sign_extend16(iw.i.imm) << 2;

	cprint_strf("bltz", IFLDW);
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", ");
	cprint_hex32(addr + 4 + target);

	return 0;
}


static int disasm_bgez(u32 instr, u32 addr)
{
	u32 target;
	union instruction iw;
	iw.word = instr;

	target = sign_extend16(iw.i.imm) << 2;

	cprint_strf("bgez", IFLDW);
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", ");
	cprint_hex32(addr + 4 + target);

	return 0;
}


static int disasm_bltzal(u32 instr, u32 addr)
{
	u32 target;
	union instruction iw;
	iw.word = instr;

	target = sign_extend16(iw.i.imm) << 2;

	cprint_strf("bltzal", IFLDW);
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", ");
	cprint_hex32(addr + 4 + target);

	return 0;
}


static int disasm_bgezal(u32 instr, u32 addr)
{
	u32 target;
	union instruction iw;
	iw.word = instr;

	target = sign_extend16(iw.i.imm) << 2;

	cprint_strf("bgezal", IFLDW);
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", ");
	cprint_hex32(addr + 4 + target);

	return 0;
}


static int disasm_j(u32 instr, u32 addr)
{
	u32 target;
	union instruction iw;
	iw.word = instr;

	target = iw.j.target;
	target = ((addr + 4) & 0xF0000000) | target << 2;

	cprint_strf("j", IFLDW);
	cprint_hex32(target);

	return 0;
}


static int disasm_jal(u32 instr, u32 addr)
{
	u32 target;
	union instruction iw;
	iw.word = instr;

	target = iw.j.target;
	target = ((addr + 4) & 0xF0000000) | target << 2;

	cprint_strf("jal", IFLDW);
	cprint_hex32(target);

	return 0;
}


static int disasm_beq(u32 instr, u32 addr)
{
	u32 target;
	union instruction iw;
	iw.word = instr;

	target = sign_extend16(iw.i.imm) << 2;

	cprint_strf("beq", IFLDW);
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_hex32(addr + 4 + target);

	return 0;
}


static int disasm_bne(u32 instr, u32 addr)
{
	u32 target;
	union instruction iw;
	iw.word = instr;

	target = sign_extend16(iw.i.imm) << 2;

	cprint_strf("bne", IFLDW);
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_hex32(addr + 4 + target);

	return 0;
}


static int disasm_blez(u32 instr, u32 addr)
{
	u32 target;
	union instruction iw;
	iw.word = instr;

	if(iw.i.rt)
		return -1;

	target = sign_extend16(iw.i.imm) << 2;

	cprint_strf("blez", IFLDW);
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", ");
	cprint_hex32(addr + 4 + target);

	return 0;
}


static int disasm_bgtz(u32 instr, u32 addr)
{
	u32 target;
	union instruction iw;
	iw.word = instr;

	if(iw.i.rt)
		return -1;

	target = sign_extend16(iw.i.imm) << 2;

	cprint_strf("bgtz", IFLDW);
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", ");
	cprint_hex32(addr + 4 + target);

	return 0;
}


static int disasm_addi(u32 instr)
{
	s32 imm;
	union instruction iw;
	iw.word = instr;

	imm = sign_extend16(iw.i.imm);

	cprint_strf("addi", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", ");
	cprint_int(imm);

	return 0;
}


static int disasm_addiu(u32 instr)
{
	u32 imm;
	union instruction iw;
	iw.word = instr;

	imm = sign_extend16(iw.i.imm);

	cprint_strf("addiu", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", ");
	cprint_int(imm);

	return 0;
}


static int disasm_slti(u32 instr)
{
	u32 imm;
	union instruction iw;
	iw.word = instr;

	imm = sign_extend16(iw.i.imm);

	cprint_strf("slti", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", ");
	cprint_int(imm);

	return 0;
}


static int disasm_sltiu(u32 instr)
{
	u32 imm;
	union instruction iw;
	iw.word = instr;

	imm = sign_extend16(iw.i.imm);

	cprint_strf("sltiu", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", ");
	cprint_uint(imm);

	return 0;
}


static int disasm_andi(u32 instr)
{
	u32 imm;
	union instruction iw;
	iw.word = instr;

	imm = zero_extend16(iw.i.imm);

	cprint_strf("andi", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", 0x");
	cprint_hex32(imm);

	return 0;
}


static int disasm_ori(u32 instr)
{
	u32 imm;
	union instruction iw;
	iw.word = instr;

	imm = zero_extend16(iw.i.imm);

	cprint_strf("ori", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", 0x");
	cprint_hex32(imm);

	return 0;
}


static int disasm_xori(u32 instr)
{
	u32 imm;
	union instruction iw;
	iw.word = instr;

	imm = zero_extend16(iw.i.imm);

	cprint_strf("xori", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_str(", 0x");
	cprint_hex32(imm);

	return 0;
}


static int disasm_lui(u32 instr)
{
	union instruction iw;
	iw.word = instr;

	if(iw.i.rs)
		return -1;

	cprint_strf("lui", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", 0x");
	cprint_hex32(iw.i.imm);

	return 0;
}


static int disasm_lb(u32 instr)
{
	s32 imm;
	union instruction iw;
	iw.word = instr;

	imm = sign_extend16(iw.i.imm);

	cprint_strf("lb", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_int(imm);
	cprint_char('(');
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_char(')');

	return 0;
}


static int disasm_lh(u32 instr)
{
	s32 imm;
	union instruction iw;
	iw.word = instr;

	imm = sign_extend16(iw.i.imm);

	cprint_strf("lh", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_int(imm);
	cprint_char('(');
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_char(')');

	return 0;
}


static int disasm_lw(u32 instr)
{
	s32 imm;
	union instruction iw;
	iw.word = instr;

	imm = sign_extend16(iw.i.imm);

	cprint_strf("lw", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_int(imm);
	cprint_char('(');
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_char(')');

	return 0;
}


static int disasm_lbu(u32 instr)
{
	s32 imm;
	union instruction iw;
	iw.word = instr;

	imm = sign_extend16(iw.i.imm);

	cprint_strf("lbu", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_int(imm);
	cprint_char('(');
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_char(')');

	return 0;
}


static int disasm_lhu(u32 instr)
{
	s32 imm;
	union instruction iw;
	iw.word = instr;

	imm = sign_extend16(iw.i.imm);

	cprint_strf("lhu", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_int(imm);
	cprint_char('(');
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_char(')');

	return 0;
}


static int disasm_sb(u32 instr)
{
	s32 imm;
	union instruction iw;
	iw.word = instr;

	imm = sign_extend16(iw.i.imm);

	cprint_strf("sb", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_int(imm);
	cprint_char('(');
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_char(')');

	return 0;
}


static int disasm_sh(u32 instr)
{
	s32 imm;
	union instruction iw;
	iw.word = instr;

	imm = sign_extend16(iw.i.imm);

	cprint_strf("sh", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_int(imm);
	cprint_char('(');
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_char(')');

	return 0;
}


static int disasm_sw(u32 instr)
{
	s32 imm;
	union instruction iw;
	iw.word = instr;

	imm = sign_extend16(iw.i.imm);

	cprint_strf("sw", IFLDW);
	cprint_str(disasm_gprs[iw.r.rt]);
	cprint_str(", ");
	cprint_int(imm);
	cprint_char('(');
	cprint_str(disasm_gprs[iw.r.rs]);
	cprint_char(')');

	return 0;
}


static int disasm_special(u32 instr)
{
	int ret = -1;
	union instruction iw;
	iw.word = instr;

	if(iw.r.shamt &&
		(iw.r.func != 0 && iw.r.func != 2 && iw.r.func != 3 &&
		iw.r.func != 12 && iw.r.func != 13)) {
			return ret;
	}

	switch(iw.r.func) {
		case 0:
			ret = disasm_sll(instr);
			break;
		case 2:
			ret = disasm_srl(instr);
			break;
		case 3:
			ret = disasm_sra(instr);
			break;
		case 4:
			ret = disasm_sllv(instr);
			break;
		case 6:
			ret = disasm_srlv(instr);
			break;
		case 7:
			ret = disasm_srav(instr);
			break;
		case 8:
			ret = disasm_jr(instr);
			break;
		case 9:
			ret = disasm_jalr(instr);
			break;
		case 12:
			ret = disasm_syscall(instr);
			break;
		case 13:
			ret = disasm_break(instr);
			break;
		case 16:
			ret = disasm_mfhi(instr);
			break;
		case 17:
			ret = disasm_mthi(instr);
			break;
		case 18:
			ret = disasm_mflo(instr);
			break;
		case 19:
			ret = disasm_mtlo(instr);
			break;
		case 24:
			ret = disasm_mult(instr);
			break;
		case 25:
			ret = disasm_multu(instr);
			break;
		case 26:
			ret = disasm_div(instr);
			break;
		case 27:
			ret = disasm_divu(instr);
			break;
		case 32:
			ret = disasm_add(instr);
			break;
		case 33:
			ret = disasm_addu(instr);
			break;
		case 34:
			ret = disasm_sub(instr);
			break;
		case 35:
			ret = disasm_subu(instr);
			break;
		case 36:
			ret = disasm_and(instr);
			break;
		case 37:
			ret = disasm_or(instr);
			break;
		case 38:
			ret = disasm_xor(instr);
			break;
		case 39:
			ret = disasm_nor(instr);
			break;
		case 42:
			ret = disasm_slt(instr);
			break;
		case 43:
			ret = disasm_sltu(instr);
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}


static int disasm_cop0(u32 instr)
{
	int ret;
	union instruction iw;
	iw.word = instr;

	switch(iw.r.rs) {
		case 0:
			ret = disasm_mfc0(instr);
			break;
		case 4:
			ret = disasm_mtc0(instr);
			break;
		case 16:
			if(iw.r.func == 16)
				ret = disasm_rfe(instr);
			else if (iw.r.func == 32)
				ret = disasm_wait(instr);
			else
				ret = -1;
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}


static int disasm_load_store(u32 instr)
{
	int ret;
	union instruction iw;
	iw.word = instr;

	switch(iw.r.op) {
		case 32:
			ret = disasm_lb(instr);
			break;
		case 33:
			ret = disasm_lh(instr);
			break;
		case 35:
			ret = disasm_lw(instr);
			break;
		case 36:
			ret = disasm_lbu(instr);
			break;
		case 37:
			ret = disasm_lhu(instr);
			break;
		case 40:
			ret = disasm_sb(instr);
			break;
		case 41:
			ret = disasm_sh(instr);
			break;
		case 43:
			ret = disasm_sw(instr);
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}


static int disasm_regimm(u32 instr, u32 addr)
{
	int ret;
	union instruction iw;
	iw.word = instr;

	switch(iw.r.rt) {
		case 0:
			ret = disasm_bltz(instr, addr);
			break;
		case 1:
			ret = disasm_bgez(instr, addr);
			break;
		case 16:
			ret = disasm_bltzal(instr, addr);
			break;
		case 17:
			ret = disasm_bgezal(instr, addr);
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}


static int disasm_other(u32 instr, u32 addr)
{
	int ret;
	union instruction iw;
	iw.word = instr;

	switch(iw.r.op) {
		case 2:
			ret = disasm_j(instr, addr);
			break;
		case 3:
			ret = disasm_jal(instr, addr);
			break;
		case 4:
			ret = disasm_beq(instr, addr);
			break;
		case 5:
			ret = disasm_bne(instr, addr);
			break;
		case 6:
			ret = disasm_blez(instr, addr);
			break;
		case 7:
			ret = disasm_bgtz(instr, addr);
			break;
		case 8:
			ret = disasm_addi(instr);
			break;
		case 9:
			ret = disasm_addiu(instr);
			break;
		case 10:
			ret = disasm_slti(instr);
			break;
		case 11:
			ret = disasm_sltiu(instr);
			break;
		case 12:
			ret = disasm_andi(instr);
			break;
		case 13:
			ret = disasm_ori(instr);
			break;
		case 14:
			ret = disasm_xori(instr);
			break;
		case 15:
			ret = disasm_lui(instr);
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}


int disasm_instr(u32 instr, u32 addr)
{
	int ret;
	union instruction iw;
	iw.word = instr;

	switch(iw.r.op) {
		case 0:
			ret = disasm_special(instr);
			break;
		case 1:
			ret = disasm_regimm(instr, addr);
			break;
		case 16:
			ret = disasm_cop0(instr);
			break;
		case 32:
		case 33:
		case 35:
		case 36:
		case 37:
		case 40:
		case 41:
		case 43:
			ret = disasm_load_store(instr);
			break;
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			ret = disasm_other(instr, addr);
			break;
		default:
			ret = -1;
			break;
	}

	if(ret < 0)
		cprint_str("???");

	return ret;
}


void disasm(void *ptr, unsigned n)
{
	u32 *prog = (u32 *) ptr;
	u32 addr = (u32) ptr;
	unsigned i;

	if (!n)
		return;

	for (i = 0; i < n; ++i, addr += 4) {
		cprint_hex32(addr); cprint_strf(":", 6);
		cprint_hex32(prog[i]); cprint_strf("", 6);
		disasm_instr(prog[i], addr);
		cprint_str("\n");
	}
}

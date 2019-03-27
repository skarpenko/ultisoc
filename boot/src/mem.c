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
#include <arch.h>
#include <con.h>
#include <str.h>
#include <ctype.h>
#include <cmd_types.h>


/* Read template */
#define readT(T, __addr) ({		\
	T __v;				\
	__v = *((volatile T*)__addr);	\
	__v;				\
})


/* Write template */
#define writeT(T, __v, __addr)		\
	*((volatile T*)__addr) = __v;	\


/* Read byte/halfword/word */
static int cmd_read(struct cmd_args *args)
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

	if(strcmp(args->args[0], "rdb") == 0) {
		u8 v = readT(u8, addr);
		cprint_hex32(addr); cprint_str(": 0x"); cprint_hex8(v); cprint_str("\n");
	} else if(strcmp(args->args[0], "rdh") == 0) {
		u16 v = readT(u16, addr);
		cprint_hex32(addr); cprint_str(": 0x"); cprint_hex16(v); cprint_str("\n");
	} else {
		u32 v = readT(u32, addr);
		cprint_hex32(addr); cprint_str(": 0x"); cprint_hex32(v); cprint_str("\n");
	}

	return 0;
}
COMMAND(r0rdb, "rdb", "rdb <addr>", "read byte", cmd_read);
COMMAND(r0rdh, "rdh", "rdh <addr>", "read halfword", cmd_read);
COMMAND(r0rdw, "rdw", "rdw <addr>", "read word", cmd_read);


/* Write byte/halfword/word */
static int cmd_write(struct cmd_args *args)
{
	unsigned addr;
	unsigned value;

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

	/* Parse value */
	if(str2u(args->args[2], &value) < 0) {
		cprint_str("Invalid argument: ");
		cprint_str(args->args[2]);
		cprint_str("\n");
		return -1;
	}

	if(strcmp(args->args[0], "wdb") == 0) {
		cprint_str("0x"); cprint_hex8(value); cprint_str(" --> ");
		cprint_hex32(addr); cprint_str("\n");
		writeT(u8, value, addr);
	} else if(strcmp(args->args[0], "wdh") == 0) {
		cprint_str("0x"); cprint_hex16(value); cprint_str(" --> ");
		cprint_hex32(addr); cprint_str("\n");
		writeT(u16, value, addr);
	} else {
		cprint_str("0x"); cprint_hex32(value); cprint_str(" --> ");
		cprint_hex32(addr); cprint_str("\n");
		writeT(u32, value, addr);
	}

	return 0;
}
COMMAND(w0wrb, "wrb", "wrb <addr> <value>", "write byte", cmd_write);
COMMAND(w0wrh, "wrh", "wrh <addr> <value>", "write halfword", cmd_write);
COMMAND(w0wrw, "wrw", "wrw <addr> <value>", "write word", cmd_write);


/* Set memory block to value */
static int cmd_memset(struct cmd_args *args)
{
	unsigned addr;
	unsigned value;
	unsigned len;

	if(args->n < 4) {
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

	/* Parse value */
	if(str2u(args->args[2], &value) < 0) {
		cprint_str("Invalid argument: ");
		cprint_str(args->args[2]);
		cprint_str("\n");
		return -1;
	}

	/* Parse length */
	if(str2u(args->args[3], &len) < 0) {
		cprint_str("Invalid argument: ");
		cprint_str(args->args[3]);
		cprint_str("\n");
		return -1;
	}

	if(len == 0) {
		cprint_str("length is zero\n");
		return 0;
	}

	cprint_str("memset: [0x"); cprint_hex32(addr); cprint_str("-0x"); cprint_hex32(addr + len - 1);
	cprint_str("] to 0x"); cprint_hex8(value); cprint_str("\n");
	memset((void*)addr, value, len);

	return 0;
}
COMMAND(m0memst, "memset", "memset <addr> <value> <len>", "fill block of memory", cmd_memset);


/* Move memory block */
static int cmd_memmove(struct cmd_args *args)
{
	unsigned dst_addr;
	unsigned src_addr;
	unsigned len;

	if(args->n < 4) {
		cprint_str("Insufficient arguments.\n");
		return -1;
	}

	/* Parse destination address */
	if(str2u(args->args[1], &dst_addr) < 0) {
		cprint_str("Invalid argument: ");
		cprint_str(args->args[1]);
		cprint_str("\n");
		return -1;
	}

	/* Parse source address */
	if(str2u(args->args[2], &src_addr) < 0) {
		cprint_str("Invalid argument: ");
		cprint_str(args->args[2]);
		cprint_str("\n");
		return -1;
	}

	/* Parse length */
	if(str2u(args->args[3], &len) < 0) {
		cprint_str("Invalid argument: ");
		cprint_str(args->args[3]);
		cprint_str("\n");
		return -1;
	}

	if(len == 0) {
		cprint_str("length is zero\n");
		return 0;
	}

	cprint_str("memmove: [0x"); cprint_hex32(src_addr); cprint_str("-0x"); cprint_hex32(src_addr + len - 1);
	cprint_str("] to [0x"); cprint_hex32(dst_addr); cprint_str("-0x"); cprint_hex32(dst_addr + len - 1);
	cprint_str("]"); cprint_str("\n");

	memmove((void*)dst_addr, (void*)src_addr, len);

	return 0;
}
COMMAND(m0memmv, "memmove", "memmove <dst> <src> <len>", "move block of memory", cmd_memmove);


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
		cprint_hex32(addr);

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

		cprint_str("  ");

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
COMMAND(m0memdu0, "memdump", "memdump <addr> <len> [per_page]", "dump memory contents (alias: 'md')", cmd_memdump);
COMMAND(m0memdu1, "md", NULL, NULL, cmd_memdump);

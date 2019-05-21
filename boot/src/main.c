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
 * BootROM entry
 */

#include <stddef.h>
#include <arch.h>
#include <global.h>
#include <uart.h>
#include <con.h>
#include <str.h>
#include <soc_info.h>
#include <soc_regs.h>
#include <cmd.h>
#include <cmd_types.h>


static void print_welcome();
static void print_soc_info();


void boot_entry()
{
	DEFINE_GLOBAL_DATA(global_data);	/* BootROM data */

	memset(&global_data, 0, sizeof(global_data));

	/* Init peripherals */
	writel(0, USOC_INTCTL_MASK);	/* Mask all interrupt lines */
	writel(0, USOC_ITIMER_CTRL);	/* Disable timer */
	writel(0, USOC_CTRL_LED);	/* Turn off LEDs */

	/* Init serial console */
	uart_init();
	con_init();

	con_set_flags(con_get_flags() | CON_FLAGS_ECHO);	/* Enable echo by default */
	con_set_flags(con_get_flags() | CON_FLAGS_LFCR);	/* Enable CR after LF by default */

	/* Welcome messages */
	print_welcome();
	print_soc_info();

	cprint_str("Type 'h' for help.\n\n");

	cmd_run("echo");

	while(1) {
		char *buf;

		cprint_str("$ ");

		buf = con_get_iobuf();
		cprint_str("\n");

		if(cmd_run(buf) == CMD_ENENT)
			cprint_str("Unknown command.\n");
	}
}


static void print_welcome()
{
	const char *welcome =
		"\n         __    __\n"
		"/  \\||_.(_  _ /\n"
		"\\__/||_|__)(_)\\____\n"
		"|__) _  _ |_|__)/  \\|\\/|\n"
		"|__)(_)(_)|_| \\ \\__/|  |\n";

	con_puts(welcome);
	con_puts("(Build: " __DATE__ " " __TIME__ ")\n");
}


static void print_soc_info()
{
	unsigned cpuid = soc_cpuid();
	addr_t rom_start = soc_rom_base();
	addr_t rom_end = rom_start + soc_rom_size() - 1;
	addr_t ram_start = soc_ram_base();
	addr_t ram_end = ram_start + soc_ram_size() - 1;
	unsigned sys_freq = soc_sys_freq();
	unsigned soc_ver = soc_version();

	cprint_str("\n");
	cprint_str("CPU Id   : "); cprint_hex32(cpuid); cprint_str("\n");
	cprint_str("SoC ver. : "); cprint_int(soc_ver); cprint_str("\n");
	cprint_str("Sys.freq.: "); cprint_int(sys_freq); cprint_str("Hz\n");
	cprint_str("ROM      : [0x");
		cprint_hex32(rom_start); cprint_str("-0x"); cprint_hex32(rom_end);
		cprint_str("]\n");
	cprint_str("RAM      : [0x");
		cprint_hex32(ram_start); cprint_str("-0x"); cprint_hex32(ram_end);
		cprint_str("]\n");
	cprint_str("\n");
}


/* Help command */
static int cmd_help(struct cmd_args *args)
{
	extern struct cmd __CMD_TABLE_BEGIN;
	extern struct cmd __CMD_TABLE_END;
	struct cmd *it;
	size_t fw = 0;	/* Field width */

	for(it = &__CMD_TABLE_BEGIN; it < &__CMD_TABLE_END; ++it) {
		size_t f = strlen(it->hint);
		fw = (f > fw ? f + 1 : fw);
	}

	for(it = &__CMD_TABLE_BEGIN; it < &__CMD_TABLE_END; ++it) {
		if(it->help && it->hint) {
			cprint_strf(it->hint, fw);
			cprint_str(" - ");
			cprint_str(it->help);
			cprint_str("\n");
		}
	}

	return 0;
}
COMMAND(a0help, "help", "help", "this help screen (aliases: 'h')", cmd_help);
COMMAND(a0help0, "h", NULL, NULL, cmd_help);	/* Shortcut */


/* Echo command */
static int cmd_echo(struct cmd_args *args)
{
	int e;

	if(args->n != 1) {
		if(!strcmp(args->args[1], "on")) {
			con_set_flags(con_get_flags() | CON_FLAGS_ECHO);
		} else if(!strcmp(args->args[1], "off")) {
			con_set_flags(con_get_flags() & ~CON_FLAGS_ECHO);
		} else {
			cprint_str("Unknown argument: ");
			cprint_str(args->args[1]);
			cprint_str("\n");
			return -1;
		}
	}

	e = (con_get_flags() & CON_FLAGS_ECHO);
	cprint_str("Echo is ");
	cprint_str(e ? "on" : "off");
	cprint_str("\n");

	return 0;
}
COMMAND(a1echo, "echo", "echo [on|off]", "console echo control", cmd_echo);


/* Lfcr command */
static int cmd_lfcr(struct cmd_args *args)
{
	int e;

	if(args->n != 1) {
		if(!strcmp(args->args[1], "on")) {
			con_set_flags(con_get_flags() | CON_FLAGS_LFCR);
		} else if(!strcmp(args->args[1], "off")) {
			con_set_flags(con_get_flags() & ~CON_FLAGS_LFCR);
		} else {
			cprint_str("Unknown argument: ");
			cprint_str(args->args[1]);
			cprint_str("\n");
			return -1;
		}
	}

	e = (con_get_flags() & CON_FLAGS_LFCR);
	cprint_str("LFCR mode is ");
	cprint_str(e ? "on" : "off");
	cprint_str("\n");

	return 0;
}
COMMAND(a1lfcr, "lfcr", "lfcr [on|off]", "line ending mode", cmd_lfcr);


/* Print SoC info  */
static int cmd_soc_info(struct cmd_args *args)
{
	print_soc_info();
	return 0;
}
COMMAND(a2socinf, "socinf", "socinf", "print SoC details", cmd_soc_info);

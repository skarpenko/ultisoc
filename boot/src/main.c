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

#include <arch.h>
#include <global.h>
#include <uart.h>
#include <con.h>
#include <soc_info.h>


void print_welcome();
void print_soc_info();


void boot_entry()
{
	DEFINE_GLOBAL_DATA(global_data);	/* BootROM data */

	/* Init serial console */
	uart_init();
	con_init();

	/* Welcome messages */
	print_welcome();
	print_soc_info();

	/*TBD*/

	con_set_flags(CON_FLAGS_ECHO);

	while(1) {
		char ch = con_getc_b();
		if(ch == 'r') break;
	}
}


void print_welcome()
{
	const char *welcome =
		"\n         __    __\n"
		"/  \\||_.(_  _ /\n"
		"\\__/||_|__)(_)\\____\n"
		"|__) _  _ |_|__)/  \\|\\/|\n"
		"|__)(_)(_)|_| \\ \\__/|  |\n";

	con_puts(welcome);
	con_puts("(Build: " __TIMESTAMP__ ")\n");
}


void print_soc_info()
{
	unsigned cpuid = soc_cpuid();
	addr_t rom_start = soc_rom_base();
	addr_t rom_end = rom_start + soc_rom_size() - 1;
	addr_t ram_start = soc_ram_base();
	addr_t ram_end = ram_start + soc_ram_size() - 1;
	unsigned sys_freq = soc_sys_freq();
	unsigned soc_ver = soc_version();

	cprint_str("\n");
	cprint_str("CPU Id   : "); cprint_hex(cpuid); cprint_str("\n");
	cprint_str("SoC ver. : "); cprint_int(soc_ver); cprint_str("\n");
	cprint_str("Sys.freq.: "); cprint_int(sys_freq); cprint_str("Hz\n");
	cprint_str("ROM      : [0x");
		cprint_hex(rom_start); cprint_str("-0x"); cprint_hex(rom_end);
		cprint_str("]\n");
	cprint_str("RAM      : [0x");
		cprint_hex(ram_start); cprint_str("-0x"); cprint_hex(ram_end);
		cprint_str("]\n");
	cprint_str("\n");
}

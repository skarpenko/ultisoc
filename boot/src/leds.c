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
 * LEDs control command
 */

#include <stddef.h>
#include <arch.h>
#include <soc_regs.h>
#include <con.h>
#include <str.h>
#include <cmd_types.h>


/* LEDs control */
static int cmd_led(struct cmd_args *args)
{
	unsigned mask;

	if(args->n == 1) {
		mask = readl(USOC_CTRL_LED);
		cprint_str("LEDs status: 0x");
		cprint_hex32(mask);
		cprint_str("\n");
		return 0;
	}

	/* Parse mask */
	if(str2u(args->args[1], &mask) < 0) {
		cprint_str("Invalid argument: ");
		cprint_str(args->args[1]);
		cprint_str("\n");
		return -1;
	}


	writel(mask, USOC_CTRL_LED);

	cprint_str("LEDs set to: 0x");
	cprint_hex32(mask);
	cprint_str("\n");

	return 0;
}
COMMAND(l0led, "led", "led [mask]", "set/get LEDs status", cmd_led);

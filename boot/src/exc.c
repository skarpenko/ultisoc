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
 * Trigger exception command
 */

#include <stddef.h>
#include <con.h>
#include <str.h>
#include <cmd_types.h>


/* Trigger exception */
static int cmd_exc(struct cmd_args *args)
{
	unsigned exc_no;

	if(args->n < 2) {
		cprint_str("Insufficient arguments.\n");
		return -1;
	}

	/* Parse exception number */
	if(str2u(args->args[1], &exc_no) < 0) {
		cprint_str("Invalid argument: ");
		cprint_str(args->args[1]);
		cprint_str("\n");
		return -1;
	}

	switch(exc_no) {
		case 0:		/* Reset */
			__asm__ __volatile__ (
				".set push       ;"
				".set noreorder  ;"
				".extern __reset ;"
				"j __reset       ;"
				"nop             ;"
				".set pop        ;"
			);
			break;
		case 1:		/* Bus Error */
			__asm__ __volatile__ (
				".set push                     ;"
				".set noreorder                ;"
				".extern __bus_error_exception ;"
				"j __bus_error_exception       ;"
				"nop                           ;"
				".set pop                      ;"
			);
			break;
		case 2:		/* Integer Overflow */
			__asm__ __volatile__ (
				".set push                    ;"
				".set noreorder               ;"
				".extern __overflow_exception ;"
				"j __overflow_exception       ;"
				"nop                          ;"
				".set pop                     ;"
			);
			break;
		case 3:		/* Address Error */
			__asm__ __volatile__ (
				".set push                         ;"
				".set noreorder                    ;"
				".extern __address_error_exception ;"
				"j __address_error_exception       ;"
				"nop                               ;"
				".set pop                          ;"
			);
			break;
		case 4:		/* Reserved Instruction */
			__asm__ __volatile__ (
				".set push                       ;"
				".set noreorder                  ;"
				".extern __resvd_instr_exception ;"
				"j __resvd_instr_exception       ;"
				"nop                             ;"
				".set pop                        ;"
			);
			break;
		case 5:		/* Breakpoint */
			__asm__ __volatile__ (
				".set push                 ;"
				".set noreorder            ;"
				".extern __break_exception ;"
				"j __break_exception       ;"
				"nop                       ;"
				".set pop                  ;"
			);
			break;
		case 6:		/* System Call */
			__asm__ __volatile__ (
				".set push                   ;"
				".set noreorder              ;"
				".extern __syscall_exception ;"
				"j __syscall_exception       ;"
				"nop                         ;"
				".set pop                    ;"
			);
			break;
		case 7:		/* Hardware Interrupt */
			__asm__ __volatile__ (
				".set push                    ;"
				".set noreorder               ;"
				".extern __hardware_interrupt ;"
				"j __hardware_interrupt       ;"
				"nop                          ;"
				".set pop                     ;"
			);
			break;
		default:
			cprint_str("Invalid exception number.\n");
			return -1;

	}

	return 0;	/* return is not expected */
}
COMMAND(e0exc, "exc", "exc <0-7>", "trigger exception", cmd_exc);

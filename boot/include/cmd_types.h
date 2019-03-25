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
 * User command handling data structures
 */

#ifndef _BOOTROM_CMD_TYPES_H_
#define _BOOTROM_CMD_TYPES_H_


/* Command arguments */
struct cmd_args {
	unsigned n;	/* Number of arguments including command name at args[0] */
	char *args[5];	/* Arguments list */
};


/* Command structure */
struct cmd {
	const char *name;			/* Command name */
	const char *hint;			/* Usage hint */
	const char *help;			/* Short help string */
	int (*func)(struct cmd_args *args);	/* Command function */
};


/* Command declaration macro */
#define COMMAND(_l, _n, _i, _h, _f)					\
	struct cmd __attribute__((section(".cmd_table." #_l))) _l = {	\
		.name = _n,						\
		.hint = _i,						\
		.help = _h,						\
		.func = _f						\
	}


#endif /* _BOOTROM_CMD_TYPES_H_ */

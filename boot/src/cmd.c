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
 * User command handling
 */

#include <stddef.h>
#include <ctype.h>
#include <str.h>
#include <cmd.h>
#include <cmd_types.h>


/* Parse command string */
static unsigned parse(char *str, struct cmd_args *args)
{
	int pws, ws;	/* Whitespace state (previous/current) */
	int i = 0;

	args->n = 0;
	if(!str || !str[0])
		return 0;

	do {
		pws = 1;
		while(str[i]) {
			ws = isspace(str[i]);
			if(pws && !ws) {
				args->args[args->n++] = &str[i];
			} else if(!pws && ws) {
				str[i++] = 0;
				break;
			}
			pws = ws;
			++i;
		}
	} while(str[i] && args->n != sizeof(args->args) / sizeof(args->args[0]) );

	return args->n;
}


static struct cmd* search_cmd(const char *name)
{
	extern struct cmd __CMD_TABLE_BEGIN;
	extern struct cmd __CMD_TABLE_END;
	struct cmd *it;

	for(it = &__CMD_TABLE_BEGIN; it < &__CMD_TABLE_END; ++it) {
		if(!strcmp(it->name, name))
			return it;
	}

	return NULL;
}


int cmd_run(char *cmd_str)
{
	struct cmd_args args;
	unsigned nargs;
	struct cmd *c;
	int ret = -1;

	nargs = parse(cmd_str, &args);
	if(!nargs)
		return ret;

	c = search_cmd(args.args[0]);
	if(!c || !c->func)
		return CMD_ENENT;

	return c->func(&args);
}

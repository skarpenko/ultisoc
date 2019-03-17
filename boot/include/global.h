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
 * Global BootROM data structure
 */

#ifndef _BOOTROM_GLOBAL_H_
#define _BOOTROM_GLOBAL_H_

#include <arch.h>


/* BootROM data */
struct global {
	int __dummy;
};


/* Set pointer to global data */
static inline
void global_set(struct global* ptr)
{
	__asm__ __volatile__ (
		".set push             ;"
		".set noreorder        ;"
		"move $gp, %0          ;"	/* use GP */
		".set pop              ;"
		:
		: "r" (ptr)
		:
	);
}


/* Get global data pointer */
static inline
struct global* global_get()
{
	unsigned long g;
	__asm__ __volatile__ (
		".set push       ;"
		".set noreorder  ;"
		"move %0, $gp    ;"
		".set pop        ;"
		: "=r" (g)
		:
		:
	);
	return (struct global*)g;
}


/* Define global data structure */
#define DEFINE_GLOBAL_DATA(__name)	\
	struct global __name;		\
	global_set(&__name)


/* Alias for global_get() */
#define G() global_get()


#endif /* _BOOTROM_GLOBAL_H_ */

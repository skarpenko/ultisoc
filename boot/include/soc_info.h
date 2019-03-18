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
 * SoC info
 */

#ifndef _BOOTROM_SOC_INFO_H_
#define _BOOTROM_SOC_INFO_H_

#include <arch.h>
#include <soc_regs.h>


/* Returns CPU Id value */
static inline
unsigned soc_cpuid()
{
	u32 v;
	__asm__ __volatile__ (
		".set push         ;"
		".set noreorder    ;"
		"mfc0 %0, $15      ;"
		".set pop          ;"
		: "=r" (v)
		:
		:
	);

	return v;

}


/* Returns ROM base address */
static inline
addr_t soc_rom_base()
{
	return 0x00000000;	/* Always zero */
}


/* Returns ROM size */
static inline
addr_t soc_rom_size()
{
	return readl(USOC_CTRL_ROMSIZE);
}


/* Returns RAM base address */
static inline
addr_t soc_ram_base()
{
	return readl(USOC_CTRL_RAMBASE);
}


/* Returns RAM size */
static inline
addr_t soc_ram_size()
{
	return readl(USOC_CTRL_RAMSIZE);
}


/* Returns system frequency */
static inline
unsigned soc_sys_freq()
{
	return readl(USOC_CTRL_SYSFREQ);
}


/* Returns SoC version */
static inline
unsigned soc_version()
{
	return readl(USOC_CTRL_SOCVER);
}


#endif /* _BOOTROM_SOC_INFO_H_ */

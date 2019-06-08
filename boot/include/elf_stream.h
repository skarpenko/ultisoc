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
 * ELF loader
 */

#ifndef _BOOTROM_ELF_STREAM_H_
#define _BOOTROM_ELF_STREAM_H_

#include <stddef.h>


/* ELF loader status */
#define ELFS_NONE		0	/* No state (internally used) */
#define ELFS_LOADED		1	/* ELF loaded successfully */
#define ELFS_INV_FILE_FMT	2	/* Invalid file format */
#define ELFS_INV_CLASS		3	/* Invalid ELF class */
#define ELFS_INV_DATA_ENC	4	/* Invalid data encoding */
#define ELFS_INV_VERSION	5	/* Invalid version */
#define ELFS_INV_ARCH_TYPE	6	/* Invalid target machine type */
#define ELFS_INV_OBJ_TYPE	7	/* Invalid object type */
#define ELFS_INV_LAYOUT		8	/* Invalid file layout */


/* Program segment */
struct segment {
	size_t fbegin;		/* Stream offset of segment start */
	size_t fend;		/* Stream offset of segment end */
	unsigned long addr;	/* Base address */
};


/* ELF data stream */
struct elf_stream {
	int status;			/* Stream status */

	size_t s_pos;			/* Stream position */

	/* ELF header data */
	unsigned long entry;		/* Entry point */
	/* Program headers data */
	size_t phoff;			/* Program headers offset */
	size_t phentsize;		/* Entry size */
	size_t phnum;			/* Headers number */

	/* Segments loading */
	size_t phcur;			/* Current program header */
	size_t phnld;			/* Number of loaded program headers */
	struct segment segs[4];		/* Loaded segments */

	/* Internally used stream parser */
	int (*parser)(struct elf_stream*);

	/* Current input buffer */
	size_t buf_sz;
	char *buf;

	/* Temporary buffer */
	size_t tmp_sz;
	char tmp[64];

	/* Callback to change load destination */
	void (*offset)(void*, unsigned long);
	void *udata;
};


/* Init ELF stream object */
void elf_stream_init(struct elf_stream* es, void (*offset)(void*, unsigned long), void *udata);


/* Parse ELF data stream */
int elf_stream_parse(struct elf_stream* es, void *buf, size_t sz);


#endif /* _BOOTROM_ELF_STREAM_H_ */

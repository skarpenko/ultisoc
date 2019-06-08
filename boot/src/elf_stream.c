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

#include <str.h>
#include <elf.h>
#include <elf_stream.h>


/* Consume stream buffer */
static inline void consume_buffer(struct elf_stream *es, size_t size)
{
	es->buf_sz -= size;
	es->buf += size;
	es->s_pos += size;
}


/* Place program segment in memory */
static void place_seg(struct elf_stream *es, size_t s)
{
	struct segment *seg = &es->segs[s];
	size_t sbegin = es->s_pos;

	if(seg->addr != (unsigned long)es->buf) {	/* Loaded segment data is not on its place */
		void *p = (void*)seg->addr;
		size_t i = seg->fbegin - sbegin;
		size_t sz = es->buf_sz - i;
		sz = ((sz + seg->fbegin) > seg->fend ? seg->fend - seg->fbegin : sz);
		memmove(p, es->buf + i, sz);	/* Move data */

		consume_buffer(es, sz + i);

		seg->addr += sz;
		seg->fbegin += sz;
	} else {					/* Segment data placed correctly */
		size_t rem = seg->fend - seg->fbegin;
		size_t sz = (es->buf_sz > rem ? rem : es->buf_sz);

		consume_buffer(es, sz);

		seg->addr += sz;
		seg->fbegin += sz;
	}

	/* Update load destination */
	es->offset(es->udata, seg->addr);
}


/* Load program segments */
static int load_seg(struct elf_stream *es)
{
	size_t sbegin = es->s_pos;
	size_t send = sbegin + es->buf_sz;
	size_t s;
	unsigned ld = 0;	/* Number of loaded segments */

	for(s = 0; s < es->phnld; ++s) {
		if(es->segs[s].fbegin == es->segs[s].fend) {
			++ld;	/* Segment already loaded, count it */
		} else if(send > es->segs[s].fbegin && sbegin < es->segs[s].fend) {
			place_seg(es, s);
			return ELFS_NONE;
		}
	}

	if(ld == es->phnld) {
		return ELFS_LOADED;	/* All segments loaded. */
	} else if(s == es->phnld) {
		/* Data is not related to any loadable segment, keep load address unchanged */
		es->offset(es->udata, (unsigned long)es->buf);
		consume_buffer(es, es->buf_sz);
	}

	return ELFS_NONE;
}


/* Program headers parser */
static int parse_phdr(struct elf_stream *es)
{
	Elf32_Phdr_t *phdr;

	/* Program header position in stream */
	size_t sbegin = es->phoff + es->phcur * es->phentsize;
	size_t send = sbegin + es->phentsize;

	/* Bad, we missed the header */
	if(es->s_pos >= send)
		return ELFS_INV_LAYOUT;

	/* Header should appear later in stream */
	if(es->s_pos + es->buf_sz < sbegin)
		return ELFS_NONE;

	if(sbegin >= es->s_pos && send <= (es->s_pos + es->buf_sz)) {	/* Full header received */
		size_t i = sbegin - es->s_pos;
		phdr = (Elf32_Phdr_t*)&es->buf[i];
		consume_buffer(es, i + es->phentsize);
	} else {							/* Partially received header */
		size_t i = (sbegin > es->s_pos) ? sbegin - es->s_pos : 0;
		size_t sz = es->buf_sz - i;
		size_t rem = sizeof(Elf32_Phdr_t) - es->tmp_sz;
		size_t n = (sz < rem ? sz : rem);
		memmove(&es->tmp[es->tmp_sz], &es->buf[i], n);	/* Store part at tmp buffer */

		consume_buffer(es, n + i);
		es->tmp_sz += n;

		if(es->tmp_sz == sizeof(Elf32_Phdr_t)) {	/* All parts arrived */
			phdr = (Elf32_Phdr_t*)es->tmp;
			es->tmp_sz = 0;
		} else
			return ELFS_NONE;
	}


	if(phdr->p_type == PT_LOAD) {	/* Loadable segment */
		/* Check if there is space to save segment data */
		if(es->phnld == (sizeof(es->segs) / sizeof(es->segs[0])))
			return ELFS_INV_LAYOUT;

		/* Save segment data */
		es->segs[es->phnld].addr = phdr->p_paddr;
		es->segs[es->phnld].fbegin = phdr->p_offset;
		es->segs[es->phnld].fend = es->segs[es->phnld].fbegin + phdr->p_filesz;

		++es->phnld;
	}

	/* Set next parser if all program headers loaded */
	if(++es->phcur == es->phnum)
		es->parser = load_seg;

	return ELFS_NONE;
}


/* ELF header parser */
static int parse_ehdr(struct elf_stream *es)
{
	Elf32_Ehdr_t *ehdr;

	if(es->tmp_sz || (es->buf_sz < sizeof(Elf32_Ehdr_t))) {		/* Header received partially */
		size_t rem = sizeof(Elf32_Ehdr_t) - es->tmp_sz;
		size_t n = (es->buf_sz < rem ? es->buf_sz : rem);
		memmove(&es->tmp[es->tmp_sz], es->buf, n);	/* Store header part at tmp buffer */

		consume_buffer(es, n);
		es->tmp_sz += n;

		if(es->tmp_sz == sizeof(Elf32_Ehdr_t)) {	/* All header parts arrived */
			ehdr = (Elf32_Ehdr_t*)es->tmp;
			es->tmp_sz = 0;
		} else
			return ELFS_NONE;
	} else {							/* Full header received */
		ehdr = (Elf32_Ehdr_t*)es->buf;
		consume_buffer(es, sizeof(Elf32_Ehdr_t));
	}

	/* Check magic value */
	if((ehdr->e_ident[EI_MAG0] != ELF_MAGIC[EI_MAG0]) || (ehdr->e_ident[EI_MAG1] != ELF_MAGIC[EI_MAG1]) ||
	  (ehdr->e_ident[EI_MAG2] != ELF_MAGIC[EI_MAG2]) || (ehdr->e_ident[EI_MAG3] != ELF_MAGIC[EI_MAG3]))
		return ELFS_INV_FILE_FMT;


	/* Check ELF class */
	if(ehdr->e_ident[EI_CLASS] != ELFCLASS32)
		return ELFS_INV_CLASS;

	/* Check ELF data encoding */
	if(ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
		return ELFS_INV_DATA_ENC;

	/* Check ELF version */
	if(ehdr->e_ident[EI_VERSION] != EV_CURRENT)
		return ELFS_INV_VERSION;

	/* Check machine type */
	if(ehdr->e_machine != EM_MIPS)
		return ELFS_INV_ARCH_TYPE;

	/* Check file type */
	if(ehdr->e_type != ET_EXEC)
		return ELFS_INV_OBJ_TYPE;

	if(!ehdr->e_phnum || ehdr->e_phentsize > sizeof(es->tmp))
		return ELFS_INV_FILE_FMT;

	/* Save received header data */
	es->entry = ehdr->e_entry;
	es->phoff = ehdr->e_phoff;
	es->phentsize = ehdr->e_phentsize;
	es->phnum = ehdr->e_phnum;

	/* Set next parser */
	es->parser = parse_phdr;

	return ELFS_NONE;
}


void elf_stream_init(struct elf_stream* es, void (*offset)(void*, unsigned long), void *udata)
{
	memset(es, 0, sizeof(struct elf_stream));
	es->offset = offset;
	es->udata = udata;
	es->parser = parse_ehdr;
}


int elf_stream_parse(struct elf_stream* es, void *buf, size_t sz)
{
	es->buf = (char*)buf;
	es->buf_sz = sz;

	while(es->buf_sz) {
		es->status = es->parser(es);
		if(es->status)
			break;
	}

	return es->status;
}

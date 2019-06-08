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
 * MIPS ELF32 defines and data structures
 */

#ifndef _BOOTROM_ELF_H_
#define _BOOTROM_ELF_H_


/*** NOTE: For more details refer to Executable and Linking Format Specification ***/


#define ELF_MAGIC	"\x7f""ELF"
#define EI_MAG0		0	/* File identification */
#define EI_MAG1		1	/* File identification */
#define EI_MAG2		2	/* File identification */
#define EI_MAG3		3	/* File identification */
#define EI_CLASS	4	/* File class */
#define EI_DATA		5	/* Data encoding */
#define EI_VERSION	6	/* File version */
#define EI_OSABI	7	/* Operating system / ABI identification */
#define EI_ABIVERSION	8	/* ABI version */
#define EI_PAD		9	/* Start of padding bytes */
#define EI_NIDENT	16


/* Required architectures */
#define EM_NONE		0	/* No machine */
#define EM_MIPS		8	/* MIPS RS3000 */


#define ELFCLASSNONE	0	/* Invalid class */
#define ELFCLASS32	1	/* 32-bit objects */
#define ELFDATANONE	0	/* Invalid data encoding */
#define ELFDATA2LSB	1	/* LSB first */
#define EV_NONE		0	/* Invalid version */
#define EV_CURRENT	1	/* Current version */


/* Object file type */
#define ET_NONE		0	/* No file type */
#define ET_EXEC		2	/* Executable file */


/* Segment types */
#define PT_NULL		0	/* The array element is unused */
#define PT_LOAD		1	/* The array element specifies loadable segment */


typedef unsigned int	Elf32_Addr;	/* Unsigned program address */
typedef unsigned short	Elf32_Half;	/* Unsigned medium integer */
typedef unsigned int	Elf32_Off;	/* Unsigned file offset */
typedef int		Elf32_Sword;	/* Signed large integer */
typedef unsigned int	Elf32_Word;	/* Unsigned large integer */


/* ELF header */
struct Elf32_Ehdr {
	/* The initial bytes mark the file as an object file and provide
	 * machine-independent data with which to decode and interpret the
	 * file's contents.
	 */
	unsigned char	e_ident[EI_NIDENT];

	/* This member identifies the object file type.
	 * Although the core file contents are unspecified, type ET_CORE is
	 * reserved to mark the file. Values from ET_LOPROC through ET_HIPROC
	 * (inclusive) are reserved for processor-specific semantics. Other
	 * values are reserved and will be assigned to new object file types as
	 * necessary.
	 */
	Elf32_Half	e_type;

	/* This member's value specifies the required architecture for an
	 * individual file.
	 * Other values are reserved and will be assigned to new machines as
	 * necessary. Processor-specific ELF names use the machine name to
	 * distinguish them. For example, the flags mentioned below use the
	 * prefix EF_; a flag named WIDGET for the EM_XYZ machine would be
	 * called EF_XYZ_WIDGET.
	 */
	Elf32_Half	e_machine;

	/* This member identifies the object file version.
	 * The value 1 signifies the original file format; extensions will
	 * create new versions with higher numbers. The value of EV_CURRENT,
	 * though given as 1 above, will change as necessary to reflect the
	 * current version number.
	 */
	Elf32_Word	e_version;

	/*
	 * This member gives the virtual address to which the system first
	 * transfers control, thus starting the process. If the file has no
	 * associated entry point, this member holds zero.
	 */
	Elf32_Addr	e_entry;

	/*
	 * This member holds the program header table's file offset in bytes.
	 * If the file has no program header table, this member holds zero.
	 */
	Elf32_Off	e_phoff;

	/*
	 * This member holds the section header table's file offset in bytes.
	 * If the file has no section header table, this member holds zero.
	 */
	Elf32_Off	e_shoff;

	/*
	 * This member holds processor-specific flags associated with the file.
	 * Flag names take the form EF_<machine>_<flag>.
	 */
	Elf32_Word	e_flags;

	/*
	 * This member holds the ELF header's size in bytes.
	 */
	Elf32_Half	e_ehsize;

	/*
	 * This member holds the size in bytes of one entry in the file's
	 * program header table; all entries are the same size.
	 */
	Elf32_Half	e_phentsize;

	/*
	 * This member holds the number of entries in the program header
	 * table. Thus the product of e_phentsize and e_phnum gives the table's
	 * size in bytes. If a file has no program header table, e_phnum holds
	 * the value zero.
	 */
	Elf32_Half	e_phnum;

	/*
	 * This member holds a section header's size in bytes. A section header
	 * is one entry in the section header table; all entries are the same
	 * size.
	 */
	Elf32_Half	e_shentsize;

	/*
	 * This member holds the number of entries in the section header table.
	 * Thus the product of e_shentsize and e_shnum gives the section header
	 * table's size in bytes. If a file has no section header table,
	 * e_shnum holds the value zero.
	 */
	Elf32_Half	e_shnum;

	/*
	 * This member holds the section header table index of the entry
	 * associated with the section name string table. If the file has no
	 * section name string table, this member holds the value SHN_UNDEF.
	 */
	Elf32_Half	e_shstrndx;
};


/* Program header */
struct Elf32_Phdr {
	/*
	 * This member tells what kind of segment this array element describes
	 * or how to interpret the array element's information.
	 */
	Elf32_Word	p_type;

	/*
	 * This member gives the offset from the beginning of the file at which
	 * the first byte of the segment resides.
	 */
	Elf32_Off	p_offset;

	/*
	 * This member gives the virtual address at which the first byte of the
	 * segment resides in memory.
	 */
	Elf32_Addr	p_vaddr;

	/*
	 * On systems for which physical addressing is relevant, this member is
	 * reserved for the segment's physical address. Because System V
	 * ignores physical addressing for application programs, this member
	 * has unspecified contents for executable files and shared objects.
	 */
	Elf32_Addr	p_paddr;

	/*
	 * This member gives the number of bytes in the file image of the
	 * segment; it may be zero.
	 */
	Elf32_Word	p_filesz;

	/*
	 * This member gives the number of bytes in the memory image of the
	 * segment; it may be zero.
	 */
	Elf32_Word	p_memsz;

	/*
	 * This member gives flags relevant to the segment.
	 */
	Elf32_Word	p_flags;

	/*
	 * As ``Program Loading'' later in this part describes, loadable
	 * process segments must have congruent values for p_vaddr and
	 * p_offset, modulo the page size. This member gives the value to which
	 * the segments are aligned in memory and in the file. Values 0 and 1
	 * mean no alignment is required. Otherwise, p_align should be a
	 * positive, integral power of 2, and p_vaddr should equal p_offset,
	 * modulo p_align.
	 */
	Elf32_Word	p_align;
};


/* ELF specific types definitions */
typedef struct Elf32_Ehdr	Elf32_Ehdr_t;
typedef struct Elf32_Phdr	Elf32_Phdr_t;


#endif /* _BOOTROM_ELF_H_ */

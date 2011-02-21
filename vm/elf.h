/*	$NetBSD: exec_elf.h,v 1.94 2007/12/26 22:11:52 christos Exp $	*/

/*-
 * Copyright (c) 1994 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Christos Zoulas.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ELF_H_
#define	ELF_H_

/*
 * The current ELF ABI specification is available at:
 *	http://www.sco.com/developers/gabi/
 *
 * Current header definitions are in:
 *	http://www.sco.com/developers/gabi/latest/ch4.eheader.html
 */

typedef	mips_ubyte Elf_Byte;
typedef mips_uhalf Elf32_Half;
typedef	mips_uword Elf32_Addr, Elf32_Off, Elf32_Word;
typedef mips_sword Elf32_Sword;

/*
 * ELF Header
 */
#define	ELF_NIDENT	16

typedef struct {
	unsigned char	e_ident[ELF_NIDENT];	/* Id bytes */
	Elf32_Half		e_type;			/* file type */
	Elf32_Half		e_machine;		/* machine type */
	Elf32_Word		e_version;		/* version number */
	Elf32_Addr		e_entry;		/* entry point */
	Elf32_Off		e_phoff;		/* Program hdr offset */
	Elf32_Off		e_shoff;		/* Section hdr offset */
	Elf32_Word		e_flags;		/* Processor flags */
	Elf32_Half      e_ehsize;		/* sizeof ehdr */
	Elf32_Half      e_phentsize;	/* Program header entry size */
	Elf32_Half      e_phnum;		/* Number of program headers */
	Elf32_Half      e_shentsize;	/* Section header entry size */
	Elf32_Half      e_shnum;		/* Number of section headers */
	Elf32_Half      e_shstrndx;		/* String table index */
} Elf32_Ehdr;

/* e_ident offsets */
#define	EI_MAG0		0	/* '\177' */
#define	EI_MAG1		1	/* 'E'    */
#define	EI_MAG2		2	/* 'L'    */
#define	EI_MAG3		3	/* 'F'    */
#define	EI_CLASS	4	/* File class */
#define	EI_DATA		5	/* Data encoding */
#define	EI_VERSION	6	/* File version */
#define	EI_OSABI	7	/* Operating system/ABI identification */
#define	EI_ABIVERSION	8	/* ABI version */
#define	EI_PAD		9	/* Start of padding bytes up to EI_NIDENT*/
#define	EI_NIDENT	16	/* First non-ident header byte */

/* e_ident[EI_MAG0,EI_MAG3] */
#define	ELFMAG0		0x7f
#define	ELFMAG1		'E'
#define	ELFMAG2		'L'
#define	ELFMAG3		'F'
#define	ELFMAG		"\177ELF"
#define	SELFMAG		4

/* e_ident[EI_CLASS] */
#define	ELFCLASSNONE	0	/* Invalid class */
#define	ELFCLASS32	1	/* 32-bit objects */
#define	ELFCLASS64	2	/* 64-bit objects */
#define	ELFCLASSNUM	3

/* e_ident[EI_DATA] */
#define	ELFDATANONE	0	/* Invalid data encoding */
#define	ELFDATA2LSB	1	/* 2's complement values, LSB first */
#define	ELFDATA2MSB	2	/* 2's complement values, MSB first */

/* e_ident[EI_VERSION] */
#define	EV_NONE		0	/* Invalid version */
#define	EV_CURRENT	1	/* Current version */
#define	EV_NUM		2

/* e_ident[EI_OSABI] */
#define	ELFOSABI_SYSV		0	/* UNIX System V ABI */
#define	ELFOSABI_HPUX		1	/* HP-UX operating system */
#define ELFOSABI_NETBSD		2	/* NetBSD */
#define ELFOSABI_LINUX		3	/* GNU/Linux */
#define ELFOSABI_HURD		4	/* GNU/Hurd */
#define ELFOSABI_86OPEN		5	/* 86Open */
#define ELFOSABI_SOLARIS	6	/* Solaris */
#define ELFOSABI_MONTEREY	7	/* Monterey */
#define ELFOSABI_IRIX		8	/* IRIX */
#define ELFOSABI_FREEBSD	9	/* FreeBSD */
#define ELFOSABI_TRU64		10	/* TRU64 UNIX */
#define ELFOSABI_MODESTO	11	/* Novell Modesto */
#define ELFOSABI_OPENBSD	12	/* OpenBSD */
/* Unofficial OSABIs follow */
#define ELFOSABI_ARM		97	/* ARM */
#define	ELFOSABI_STANDALONE	255	/* Standalone (embedded) application */

/* e_type */
#define	ET_NONE		0	/* No file type */
#define	ET_REL		1	/* Relocatable file */
#define	ET_EXEC		2	/* Executable file */
#define	ET_DYN		3	/* Shared object file */
#define	ET_CORE		4	/* Core file */
#define	ET_NUM		5

#define	ET_LOOS		0xfe00	/* Operating system specific range */
#define	ET_HIOS		0xfeff
#define	ET_LOPROC	0xff00	/* Processor-specific range */
#define	ET_HIPROC	0xffff

/* e_machine */
#define	EM_NONE		0	/* No machine */
#define	EM_M32		1	/* AT&T WE 32100 */
#define	EM_SPARC	2	/* SPARC */
#define	EM_386		3	/* Intel 80386 */
#define	EM_68K		4	/* Motorola 68000 */
#define	EM_88K		5	/* Motorola 88000 */
#define	EM_486		6	/* Intel 80486 */
#define	EM_860		7	/* Intel 80860 */
#define	EM_MIPS		8	/* MIPS I Architecture */
#define	EM_S370		9	/* Amdahl UTS on System/370 */
#define	EM_MIPS_RS3_LE	10	/* MIPS RS3000 Little-endian */
			/* 11-14 - Reserved */
#define	EM_RS6000	11	/* IBM RS/6000 XXX reserved */
#define	EM_PARISC	15	/* Hewlett-Packard PA-RISC */
#define	EM_NCUBE	16	/* NCube XXX reserved */
#define	EM_VPP500	17	/* Fujitsu VPP500 */
#define	EM_SPARC32PLUS	18	/* Enhanced instruction set SPARC */
#define	EM_960		19	/* Intel 80960 */
#define	EM_PPC		20	/* PowerPC */
#define	EM_PPC64	21	/* 64-bit PowerPC */
			/* 22-35 - Reserved */
#define	EM_V800		36	/* NEC V800 */
#define	EM_FR20		37	/* Fujitsu FR20 */
#define	EM_RH32		38	/* TRW RH-32 */
#define	EM_RCE		39	/* Motorola RCE */
#define	EM_ARM		40	/* Advanced RISC Machines ARM */
#define	EM_ALPHA	41	/* DIGITAL Alpha */
#define	EM_SH		42	/* Hitachi Super-H */
#define	EM_SPARCV9	43	/* SPARC Version 9 */
#define	EM_TRICORE	44	/* Siemens Tricore */
#define	EM_ARC		45	/* Argonaut RISC Core */
#define	EM_H8_300	46	/* Hitachi H8/300 */
#define	EM_H8_300H	47	/* Hitachi H8/300H */
#define	EM_H8S		48	/* Hitachi H8S */
#define	EM_H8_500	49	/* Hitachi H8/500 */
#define	EM_IA_64	50	/* Intel Merced Processor */
#define	EM_MIPS_X	51	/* Stanford MIPS-X */
#define	EM_COLDFIRE	52	/* Motorola Coldfire */
#define	EM_68HC12	53	/* Motorola MC68HC12 */
#define	EM_MMA		54	/* Fujitsu MMA Multimedia Accelerator */
#define	EM_PCP		55	/* Siemens PCP */
#define	EM_NCPU		56	/* Sony nCPU embedded RISC processor */
#define	EM_NDR1		57	/* Denso NDR1 microprocessor */
#define	EM_STARCORE	58	/* Motorola Star*Core processor */
#define	EM_ME16		59	/* Toyota ME16 processor */
#define	EM_ST100	60	/* STMicroelectronics ST100 processor */
#define	EM_TINYJ	61	/* Advanced Logic Corp. TinyJ embedded family processor */
#define	EM_X86_64	62	/* AMD x86-64 architecture */
#define	EM_PDSP		63	/* Sony DSP Processor */
#define	EM_PDP10	64	/* Digital Equipment Corp. PDP-10 */
#define	EM_PDP11	65	/* Digital Equipment Corp. PDP-11 */
#define	EM_FX66		66	/* Siemens FX66 microcontroller */
#define	EM_ST9PLUS	67	/* STMicroelectronics ST9+ 8/16 bit microcontroller */
#define	EM_ST7		68	/* STMicroelectronics ST7 8-bit microcontroller */
#define	EM_68HC16	69	/* Motorola MC68HC16 Microcontroller */
#define	EM_68HC11	70	/* Motorola MC68HC11 Microcontroller */
#define	EM_68HC08	71	/* Motorola MC68HC08 Microcontroller */
#define	EM_68HC05	72	/* Motorola MC68HC05 Microcontroller */
#define	EM_SVX		73	/* Silicon Graphics SVx */
#define	EM_ST19		74	/* STMicroelectronics ST19 8-bit CPU */
#define	EM_VAX		75	/* Digital VAX */
#define	EM_CRIS		76	/* Axis Communications 32-bit embedded processor */
#define	EM_JAVELIN	77	/* Infineon Technologies 32-bit embedded CPU */
#define	EM_FIREPATH	78	/* Element 14 64-bit DSP processor */
#define	EM_ZSP		79	/* LSI Logic's 16-bit DSP processor */
#define	EM_MMIX		80	/* Donald Knuth's educational 64-bit processor */
#define	EM_HUANY	81	/* Harvard's machine-independent format */
#define	EM_PRISM	82	/* SiTera Prism */
#define	EM_AVR		83	/* Atmel AVR 8-bit microcontroller */
#define	EM_FR30		84	/* Fujitsu FR30 */
#define	EM_D10V		85	/* Mitsubishi D10V */
#define	EM_D30V		86	/* Mitsubishi D30V */
#define	EM_V850		87	/* NEC v850 */
#define	EM_M32R		88	/* Mitsubishi M32R */
#define	EM_MN10300	89	/* Matsushita MN10300 */
#define	EM_MN10200	90	/* Matsushita MN10200 */
#define	EM_PJ		91	/* picoJava */
#define	EM_OPENRISC	92	/* OpenRISC 32-bit embedded processor */
#define	EM_ARC_A5	93	/* ARC Cores Tangent-A5 */
#define	EM_XTENSA	94	/* Tensilica Xtensa Architecture */
#define	EM_NS32K	97	/* National Semiconductor 32000 series */

/* Unofficial machine types follow */
#define	EM_AVR32	6317	/* used by NetBSD/avr32 */
#define	EM_ALPHA_EXP	36902	/* used by NetBSD/alpha; obsolete */
#define	EM_NUM		36903

/*
 * Program Header
 */
typedef struct {
	Elf32_Word	p_type;		/* entry type */
	Elf32_Off	p_offset;	/* offset */
	Elf32_Addr	p_vaddr;	/* virtual address */
	Elf32_Addr	p_paddr;	/* physical address */
	Elf32_Word	p_filesz;	/* file size */
	Elf32_Word	p_memsz;	/* memory size */
	Elf32_Word	p_flags;	/* flags */
	Elf32_Word	p_align;	/* memory & file alignment */
} Elf32_Phdr;

/* p_type */
#define	PT_NULL		0		/* Program header table entry unused */
#define	PT_LOAD		1		/* Loadable program segment */
#define	PT_DYNAMIC	2		/* Dynamic linking information */
#define	PT_INTERP	3		/* Program interpreter */
#define	PT_NOTE		4		/* Auxiliary information */
#define	PT_SHLIB	5		/* Reserved, unspecified semantics */
#define	PT_PHDR		6		/* Entry for header table itself */
#define	PT_NUM		7

#define	PT_LOOS		0x60000000	/* OS-specific range */
#define	PT_HIOS		0x6fffffff
#define	PT_LOPROC	0x70000000	/* Processor-specific range */
#define	PT_HIPROC	0x7fffffff

#define	PT_MIPS_REGINFO	0x70000000

/* p_flags */
#define	PF_R		0x4	/* Segment is readable */
#define	PF_W		0x2	/* Segment is writable */
#define	PF_X		0x1	/* Segment is executable */

#define	PF_MASKOS	0x0ff00000	/* Operating system specific values */
#define	PF_MASKPROC	0xf0000000	/* Processor-specific values */

/*
 * Section Headers
 */
typedef struct {
	Elf32_Word	sh_name;	/* section name (.shstrtab index) */
	Elf32_Word	sh_type;	/* section type */
	Elf32_Word	sh_flags;	/* section flags */
	Elf32_Addr	sh_addr;	/* virtual address */
	Elf32_Off	sh_offset;	/* file offset */
	Elf32_Word	sh_size;	/* section size */
	Elf32_Word	sh_link;	/* link to another */
	Elf32_Word	sh_info;	/* misc info */
	Elf32_Word	sh_addralign;	/* memory alignment */
	Elf32_Word	sh_entsize;	/* table entry size */
} Elf32_Shdr;

/* sh_type */
#define	SHT_NULL		0		/* Section header table entry unused */
#define	SHT_PROGBITS	1		/* Program information */
#define	SHT_SYMTAB		2		/* Symbol table */
#define	SHT_STRTAB		3		/* String table */
#define	SHT_RELA		4		/* Relocation information w/ addend */
#define	SHT_HASH		5		/* Symbol hash table */
#define	SHT_DYNAMIC		6		/* Dynamic linking information */
#define	SHT_NOTE		7		/* Auxiliary information */
#define	SHT_NOBITS		8		/* No space allocated in file image */
#define	SHT_REL			9		/* Relocation information w/o addend */
#define	SHT_SHLIB		10		/* Reserved, unspecified semantics */
#define	SHT_DYNSYM		11		/* Symbol table for dynamic linker */
#define	SHT_NUM			12

#define	SHT_LOOS		0x60000000	/* Operating system specific range */
#define SHT_SUNW_VERDEF	0x6ffffffd	/* Versions defined by file */
#define SHT_SUNW_VERNEED 0x6ffffffe	/* Versions needed by file */
#define SHT_SUNW_VERSYM	0x6fffffff	/* Symbol versions */
#define	SHT_HIOS		0x6fffffff
#define	SHT_LOPROC		0x70000000	/* Processor-specific range */
#define	SHT_HIPROC		0x7fffffff
#define	SHT_LOUSER		0x80000000	/* Application-specific range */
#define	SHT_HIUSER		0xffffffff

/* sh_flags */
#define	SHF_WRITE		0x1		/* Section contains writable data */
#define	SHF_ALLOC		0x2		/* Section occupies memory */
#define	SHF_EXECINSTR	0x4		/* Section contains executable insns */

#define	SHF_MASKOS		0x0f000000	/* Operating system specific values */
#define	SHF_MASKPROC	0xf0000000	/* Processor-specific values */

/*
 * Symbol Table
 */
typedef struct {
	Elf32_Word	st_name;	/* Symbol name (.symtab index) */
	Elf32_Word	st_value;	/* value of symbol */
	Elf32_Word	st_size;	/* size of symbol */
	Elf_Byte	st_info;	/* type / binding attrs */
	Elf_Byte	st_other;	/* unused */
	Elf32_Half	st_shndx;	/* section index of symbol */
} Elf32_Sym;

/* Symbol Table index of the undefined symbol */
#define	ELF_SYM_UNDEFINED	0

#define STN_UNDEF		0	/* undefined index */

/* st_info: Symbol Bindings */
#define	STB_LOCAL		0	/* local symbol */
#define	STB_GLOBAL		1	/* global symbol */
#define	STB_WEAK		2	/* weakly defined global symbol */
#define	STB_NUM			3

#define	STB_LOOS		10	/* Operating system specific range */
#define	STB_HIOS		12
#define	STB_LOPROC		13	/* Processor-specific range */
#define	STB_HIPROC		15

/* st_info: Symbol Types */
#define	STT_NOTYPE		0	/* Type not specified */
#define	STT_OBJECT		1	/* Associated with a data object */
#define	STT_FUNC		2	/* Associated with a function */
#define	STT_SECTION		3	/* Associated with a section */
#define	STT_FILE		4	/* Associated with a file name */
#define	STT_NUM			5

#define	STT_LOOS		10	/* Operating system specific range */
#define	STT_HIOS		12
#define	STT_LOPROC		13	/* Processor-specific range */
#define	STT_HIPROC		15

/* st_other: Visibility Types */
#define	STV_DEFAULT		0	/* use binding type */
#define	STV_INTERNAL	1	/* not referenced from outside */
#define	STV_HIDDEN		2	/* not visible, may be used via ptr */
#define	STV_PROTECTED	3	/* visible, not preemptible */

/* st_info/st_other utility macros */
#define	ELF_ST_BIND(info)		((unsigned)(info) >> 4)
#define	ELF_ST_TYPE(info)		((unsigned)(info) & 0xf)
#define	ELF_ST_INFO(bind,type)	((Elf_Byte)(((bind) << 4) | ((type) & 0xf)))
#define	ELF_ST_VISIBILITY(other) ((unsigned)(other) & 3)

/*
 * Special section indexes
 */
#define	SHN_UNDEF	0		/* Undefined section */

#define	SHN_LORESERVE	0xff00		/* Reserved range */
#define	SHN_ABS			0xfff1		/*  Absolute symbols */
#define	SHN_COMMON		0xfff2		/*  Common symbols */
#define	SHN_HIRESERVE	0xffff

#define	SHN_LOPROC	0xff00		/* Processor-specific range */
#define	SHN_HIPROC	0xff1f
#define	SHN_LOOS	0xff20		/* Operating system specific range */
#define	SHN_HIOS	0xff3f

#define	SHN_MIPS_ACOMMON	0xff00
#define	SHN_MIPS_TEXT		0xff01
#define	SHN_MIPS_DATA		0xff02
#define	SHN_MIPS_SCOMMON	0xff03

/*
 * Relocation Entries
 */
typedef struct {
	Elf32_Word	r_offset;	/* where to do it */
	Elf32_Word	r_info;		/* index & type of relocation */
} Elf32_Rel;

typedef struct {
	Elf32_Word	r_offset;	/* where to do it */
	Elf32_Word	r_info;		/* index & type of relocation */
	Elf32_Sword	r_addend;	/* adjustment value */
} Elf32_Rela;

/* mips relocs.  */

#define R_MIPS_NONE		0
#define R_MIPS_16		1
#define R_MIPS_32		2
#define R_MIPS_REL32	3
#define R_MIPS_REL		R_MIPS_REL32
#define R_MIPS_26		4
#define R_MIPS_HI16		5	/* high 16 bits of symbol value */
#define R_MIPS_LO16		6	/* low 16 bits of symbol value */
#define R_MIPS_GPREL16	7  	/* GP-relative reference  */
#define R_MIPS_LITERAL	8 	/* Reference to literal section  */
#define R_MIPS_GOT16	9	/* Reference to global offset table */
#define R_MIPS_GOT		R_MIPS_GOT16
#define R_MIPS_PC16		10  	/* 16 bit PC relative reference */
#define R_MIPS_CALL16 	11  	/* 16 bit call thru glbl offset tbl */
#define R_MIPS_CALL		R_MIPS_CALL16
#define R_MIPS_GPREL32	12

/* r_info utility macros */
#define	ELF32_R_SYM(info)	((info) >> 8)
#define	ELF32_R_TYPE(info)	((info) & 0xff)
#define	ELF32_R_INFO(sym, type)	(((sym) << 8) + (unsigned char)(type))

/*
 * Dynamic Section structure array
 */
typedef struct {
	Elf32_Word	d_tag;		/* entry tag value */
	union {
	    Elf32_Addr	d_ptr;
	    Elf32_Word	d_val;
	} d_un;
} Elf32_Dyn;

/* d_tag */
#define	DT_NULL		0	/* Marks end of dynamic array */
#define	DT_NEEDED	1	/* Name of needed library (DT_STRTAB offset) */
#define	DT_PLTRELSZ	2	/* Size, in bytes, of relocations in PLT */
#define	DT_PLTGOT	3	/* Address of PLT and/or GOT */
#define	DT_HASH		4	/* Address of symbol hash table */
#define	DT_STRTAB	5	/* Address of string table */
#define	DT_SYMTAB	6	/* Address of symbol table */
#define	DT_RELA		7	/* Address of Rela relocation table */
#define	DT_RELASZ	8	/* Size, in bytes, of DT_RELA table */
#define	DT_RELAENT	9	/* Size, in bytes, of one DT_RELA entry */
#define	DT_STRSZ	10	/* Size, in bytes, of DT_STRTAB table */
#define	DT_SYMENT	11	/* Size, in bytes, of one DT_SYMTAB entry */
#define	DT_INIT		12	/* Address of initialization function */
#define	DT_FINI		13	/* Address of termination function */
#define	DT_SONAME	14	/* Shared object name (DT_STRTAB offset) */
#define	DT_RPATH	15	/* Library search path (DT_STRTAB offset) */
#define	DT_SYMBOLIC	16	/* Start symbol search within local object */
#define	DT_REL		17	/* Address of Rel relocation table */
#define	DT_RELSZ	18	/* Size, in bytes, of DT_REL table */
#define	DT_RELENT	19	/* Size, in bytes, of one DT_REL entry */
#define	DT_PLTREL	20 	/* Type of PLT relocation entries */
#define	DT_DEBUG	21	/* Used for debugging; unspecified */
#define	DT_TEXTREL	22	/* Relocations might modify non-writable seg */
#define	DT_JMPREL	23	/* Address of relocations associated with PLT */
#define	DT_BIND_NOW	24	/* Process all relocations at load-time */
#define	DT_INIT_ARRAY	25	/* Address of initialization function array */
#define	DT_FINI_ARRAY	26	/* Size, in bytes, of DT_INIT_ARRAY array */
#define	DT_INIT_ARRAYSZ	27	/* Address of termination function array */
#define	DT_FINI_ARRAYSZ	28	/* Size, in bytes, of DT_FINI_ARRAY array*/
#define	DT_NUM		29

#define	DT_LOOS		0x60000000	/* Operating system specific range */
#define DT_VERSYM	0x6ffffff0	/* Symbol versions */
#define	DT_FLAGS_1	0x6ffffffb	/* ELF dynamic flags */
#define DT_VERDEF	0x6ffffffc	/* Versions defined by file */
#define DT_VERDEFNUM	0x6ffffffd	/* Number of versions defined by file */
#define DT_VERNEED	0x6ffffffe	/* Versions needed by file */
#define DT_VERNEEDNUM	0x6fffffff	/* Number of versions needed by file */
#define	DT_HIOS		0x6fffffff
#define	DT_LOPROC	0x70000000	/* Processor-specific range */
#define	DT_HIPROC	0x7fffffff

/* mips dynamic tags */

#define DT_MIPS_RLD_VERSION	0x70000001
#define DT_MIPS_TIME_STAMP	0x70000002
#define DT_MIPS_ICHECKSUM	0x70000003
#define DT_MIPS_IVERSION	0x70000004
#define DT_MIPS_FLAGS		0x70000005
#define DT_MIPS_BASE_ADDRESS	0x70000006
#define DT_MIPS_CONFLICT	0x70000008
#define DT_MIPS_LIBLIST		0x70000009
#define DT_MIPS_CONFLICTNO	0x7000000b
#define	DT_MIPS_LOCAL_GOTNO	0x7000000a	/* number of local got ents */
#define DT_MIPS_LIBLISTNO	0x70000010
#define	DT_MIPS_SYMTABNO	0x70000011	/* number of .dynsym entries */
#define DT_MIPS_UNREFEXTNO	0x70000012
#define	DT_MIPS_GOTSYM		0x70000013	/* first dynamic sym in got */
#define DT_MIPS_HIPAGENO	0x70000014
#define	DT_MIPS_RLD_MAP		0x70000016	/* address of loader map */

/* Flag values for DT_FLAGS_1 (incomplete) */
#define	DF_1_INITFIRST	0x00000020	/* Object's init/fini take priority */

#endif /* !ELF_H_ */

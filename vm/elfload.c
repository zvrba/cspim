/* 
 * File:    elfload.c
 * Author:  zvrba
 * Created: 2008-02-19
 *
 * ===========================================================================
 * COPYRIGHT (c) 2008 Zeljko Vrba <zvrba.external@zvrba.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * ===========================================================================
 */
/**
 * @file
 * Prepare specially linked MIPS executable for execution in memory.  The
 * emulator's memory layout is as follows:
 *
 * +---------------+ base
 * | control state | (cpu registers, break value, etc)
 * +---------------+ base + 0x1000
 * | text/data/bss |
 * | from ELF img  |
 * +------------+--+ <- brk = _end; start depends on ELF size
 * | available  |  |    this area is available for allocation
 * |            v  |
 * +---------------+ <- $sp = $29: current top of stack
 * | stack      ^  |
 * |            |  |
 * +------------+--+ base + memsz
 *
 * The executable must have been linked with the following flags:
 *
 * -mips1 -Wl,-q -nostdlib -Ttext 0x1000 -mno-check-zero-division -mlong-calls
 *
 *
 * and must define _start function as its entry point, which will also be
 * set as entry point in the ELF header.
 *
 * The code is suboptimal at places, but the goal is to avoid the use of 
 * any library routines.
 */

#include "types.h"
#include "cpu.h"
#include "elf.h"
#include <stdio.h>

static int check_eh_limits(const Elf32_Ehdr*, size_t);
static int load_segments(struct mips_cpu*);
static int load_segment(struct mips_cpu*, const Elf32_Phdr*);
static int get_symtab(const char*, size_t, Elf32_Shdr**, Elf32_Shdr**);
static const char *get_string(const char*, size_t, const Elf32_Shdr*, size_t);
static Elf32_Sym *get_sym(const char*, size_t, const Elf32_Shdr*, size_t);
static Elf32_Phdr *get_phdr(const char*, size_t, size_t);
static Elf32_Shdr *get_shdr(const char*, size_t, size_t);
static int Sequal(const char*, const char*);

int mips_elf_load(struct mips_cpu *pcpu, const char *elf, size_t elfsz)
{
	Elf32_Ehdr *eh = (Elf32_Ehdr*)elf;
	Elf32_Sym  *symgp;				/* global pointer value */
	Elf32_Sym  *symend;				/* end of data; break start */

	pcpu->elf     = elf;
	pcpu->elfsz   = elfsz;

	/* Check ELF format and load segments to their proper places.  Don't allow
	 * segment data to overflow into area reserved for stack. */
	
	if(check_eh_limits(eh, elfsz)) {
		fprintf(stderr, "ELF header is off limits!\n");
		return -1;
	}
	if(load_segments(pcpu)) {
		fprintf(stderr, "Cannot load segments!\n");
		return -1;
	}
	if(get_symtab(elf, elfsz, &pcpu->shsymtab, &pcpu->shsymstr)) {
		fprintf(stderr, "Cannot load symbols!\n");
		return -1;
	}
	if(eh->e_entry >= pcpu->memsz - pcpu->stksz) {
		fprintf(stderr, "Invalid entry address!\n");
		return -1;
	}
	
	/* Segments successfully loaded; set up registers for program start. */

	symgp  = mips_elf_find_symbol(pcpu, "_gp");
	if(!symgp || (symgp->st_shndx != SHN_ABS)
	   || (symgp->st_value >= pcpu->memsz - pcpu->stksz)
	   || (symgp->st_value < MIPS_LOWBASE)) {
	   	fprintf(stderr, "sym _gp error %p\n", symgp);
		return -1;
	}
		
	symend = mips_elf_find_symbol(pcpu, "_end");
	if(!symend || (symend->st_shndx != SHN_ABS)
	   || (symend->st_value >= pcpu->memsz - pcpu->stksz)
	   || (symend->st_value < MIPS_LOWBASE)) {
	   	fprintf(stderr, "sym _end error\n");
		return -1;
	}
	
	if((eh->e_entry >= pcpu->memsz - pcpu->stksz)
	   || (eh->e_entry < MIPS_LOWBASE)) {
	   	fprintf(stderr, "entry error\n");
		return -1;
	}
	
	pcpu->brk      = symend->st_value;
	pcpu->pc       = eh->e_entry;
	pcpu->r.ur[28] = symgp->st_value;	/* global data pointer */
	pcpu->r.ur[29] = pcpu->memsz - 4;	/* stack pointer */
	pcpu->r.ur[31] = 0;					/* link register */

	return 0;
}

Elf32_Sym *mips_elf_find_symbol(struct mips_cpu *pcpu, const char *name)
{
	Elf32_Shdr *shsymtab = pcpu->shsymtab;
	unsigned    n =shsymtab->sh_size / shsymtab->sh_entsize;
	unsigned    i;
	
	if(shsymtab->sh_offset  + shsymtab->sh_size > pcpu->elfsz)
		return NULL;
	
	for(i = 0; i < n; i++) {
	    Elf32_Sym  *sym = get_sym(pcpu->elf, pcpu->elfsz, shsymtab, i);
	    const char *symname;

	    if(!sym)
			return NULL;
	    if(ELF_ST_BIND(sym->st_info) != STB_GLOBAL)
			continue;
		
	    symname = get_string(pcpu->elf, pcpu->elfsz,
				pcpu->shsymstr, sym->st_name);
	    if(!symname)
			return NULL;
	    if(Sequal(name, symname))
			return sym;
	}
	return NULL;
}

Elf32_Sym *mips_elf_find_address(struct mips_cpu *pcpu, Elf32_Addr addr)
{
	Elf32_Shdr *shsymtab = pcpu->shsymtab;
	unsigned    n = shsymtab->sh_size / shsymtab->sh_entsize;
	Elf32_Addr  min = (unsigned)-1;
	Elf32_Sym  *ret = NULL;
	unsigned    i;

	if((shsymtab->sh_offset  + shsymtab->sh_size > pcpu->elfsz)
	|| (addr >= pcpu->memsz))
		return NULL;
	
	for(i = 0; i < n; i++) {
		Elf32_Sym  *sym = get_sym(pcpu->elf, pcpu->elfsz, shsymtab, i);

		if(!sym)
			return NULL;
		if((ELF_ST_BIND(sym->st_info) != STB_GLOBAL) ||
				(ELF_ST_TYPE(sym->st_info) >= STT_SECTION))
			continue;
		
		if(sym->st_value <= addr) {
			Elf32_Addr diff = addr - sym->st_value;
			
			if(diff < min) {
				min = diff;
				ret = sym;
				if(diff == 0)
					break;
			}
		}
	}
	return ret;
}

const char *mips_elf_get_symname(struct mips_cpu *pcpu, Elf32_Sym *sym)
{
	return get_string(pcpu->elf, pcpu->elfsz, pcpu->shsymstr, sym->st_name);
}

static int check_eh_limits(const Elf32_Ehdr *eh, size_t sz)
{
	if(eh->e_ehsize != sizeof(*eh))
		return -1;
	
	if((eh->e_ident[0] != ELFMAG0) || (eh->e_ident[1] != ELFMAG1) ||
			(eh->e_ident[2] != ELFMAG2) || (eh->e_ident[3] != ELFMAG3))
		return -1;
	if((eh->e_ident[EI_CLASS] != ELFCLASS32) ||
			(eh->e_ident[EI_DATA] != ELFDATA2LSB))
		return -1;
	if((eh->e_type != ET_EXEC) ||(eh->e_machine != EM_MIPS))
		return -1;
	
	if(eh->e_phoff + eh->e_phnum*eh->e_phentsize > sz)
		return -1;
	if((eh->e_shoff + eh->e_shnum*eh->e_shentsize > sz) ||
			(eh->e_shstrndx >= eh->e_shnum))
		return -1;
	
	/* All checks passed. */
	return 0;		
}

/**
 * Iterate program headers and copy PT_LOAD segments at their proper
 * locations.  If any segment type other than PT_NULL or PT_LOAD is
 * encountered, return an error.
 */
static int load_segments(struct mips_cpu *pcpu)
{
	const char *elf   = pcpu->elf;
	size_t      elfsz = pcpu->elfsz;
	Elf32_Ehdr *eh    = (Elf32_Ehdr*)elf;
	unsigned    i;
	
	for(i = 0; i < eh->e_phnum; i++) {
		Elf32_Phdr *ph = get_phdr(elf, elfsz, i);
		
		if(!ph)
			return -1;
		switch(ph->p_type) {
		case PT_NULL:
			break;
		case PT_LOAD:
			if(load_segment(pcpu, ph))
				return -1;
			break;
		default:
			break;
		}
	}
	return 0;
}

/**
 * Perform necessary bound checks and copy segment contents to the proper
 * memory location.  If p_memsz > p_filesz, the gap is filled with 0s.  Also
 * fail if the segment starting address is < MIPS_LOWBASE.
 */
static int load_segment(struct mips_cpu *pcpu, const Elf32_Phdr *ph)
{
	const char *elf = pcpu->elf;
	size_t elfsz = pcpu->elfsz;
	size_t memsz = pcpu->memsz - pcpu->stksz;
	unsigned i;
	
	if(ph->p_offset + ph->p_filesz > elfsz)
		return -1;
	if(ph->p_vaddr + ph->p_memsz > memsz) {
		fprintf(stderr, "Virtual address + elf memsz > %u\n", (unsigned int)memsz);
		return -1;
	}
	if(ph->p_vaddr < MIPS_LOWBASE) {
		fprintf(stderr, "Illegal virtual address: %.8x\n", (unsigned int)ph->p_vaddr);
		return -1;
	}
	if((ph->p_filesz % 4) || (ph->p_memsz % 4) || (ph->p_memsz < ph->p_filesz))
		return -1;
	
	/*
	  Because of the above checks, the pokes below shall not throw.  The
	  segment data is copied verbatim (identity_poke).  However, the zeros
	  are written through vectored poke in order to have correct data in
	  case memory transformation (e.g. encryption) is applied.
	*/

	for(i = 0; i < ph->p_filesz; i += 4)
		mips_identity_poke_uw(pcpu, ph->p_vaddr+i,
							  *(mips_uword*)(elf + ph->p_offset+i));
	for(; i < ph->p_memsz; i += 4)
		mips_poke_uw(pcpu, ph->p_vaddr+i, 0);

	return 0;
}

/**
 * Return section headers of the symbol table and the associated string table.
 * There must be exactly one symbol table and its name must be ".symtab" and
 * its associated string table must have name ".strtab".
 */
static int get_symtab(const char *elf, size_t elfsz,
		Elf32_Shdr **shsymtab, Elf32_Shdr **shsymstr)
{
	Elf32_Ehdr *eh = (Elf32_Ehdr*)elf;
	Elf32_Shdr *shshstr = NULL;
	const char *secname;
	unsigned i;
	
	*shsymtab = *shsymstr = NULL;
	
	for(i = 0; i < eh->e_shnum; i++) {
		Elf32_Shdr *sh = get_shdr(elf, elfsz, i);
		
		if(!sh)
			return -1;
		if(i == eh->e_shstrndx) {
			shshstr = sh;
		} else if(sh->sh_type == SHT_SYMTAB) {
			if(*shsymtab)		/* Don't allow multiple symtabs. */
				return -1;
			*shsymtab = sh;
		}
	}
	
	if(!*shsymtab || !shshstr)
		return -1;
	
	secname = get_string(elf, elfsz, shshstr, (*shsymtab)->sh_name);
	if((*shsymtab)->sh_link >= eh->e_shnum ||
			!secname || !Sequal(secname, ".symtab"))
		return -1;
	
	*shsymstr = get_shdr(elf, elfsz, (*shsymtab)->sh_link);
	if(!*shsymstr)
		return -1;
	secname = get_string(elf, elfsz, shshstr, (*shsymstr)->sh_name);
	if(((*shsymstr)->sh_type != SHT_STRTAB) ||
			!secname || !Sequal(secname, ".strtab"))
		return -1;
	
	return 0;
}

/**
 * Return string at index ndx from the given string table section.  Returns
 * NULL on failure, else pointer to the string.
 */
static const char *get_string(const char *elf, size_t elfsz,
		const Elf32_Shdr *sh, size_t ndx)
{
	Elf32_Off	off = sh->sh_offset;
	size_t		i;

	if((sh->sh_type != SHT_STRTAB) || (ndx >= sh->sh_size) || (off >= elfsz))
		return NULL;
	
	/* Check that the string doesn't go out of bounds of its section.  No
	 * other way than to search for 0-terminator.  NULL-terminated strings
	 * just suck. */

	for(i = ndx; i < sh->sh_size; i++)
		if(!elf[off+i])
			return elf+off+ndx;
	
	/* 0-terminator not found before section end. */
	return NULL;
}

static Elf32_Sym *get_sym(const char *elf, size_t elfsz,
		const Elf32_Shdr *sh, size_t ndx)
{
	Elf32_Off toff = ndx*sh->sh_entsize;
	Elf32_Off off  = sh->sh_offset + toff;
	
	if((sh->sh_type != SHT_SYMTAB) || (toff >= sh->sh_size) || (off >= elfsz))
		return NULL;
	return (Elf32_Sym*)(elf + off);
}


static Elf32_Phdr *get_phdr(const char *elf, size_t elfsz, size_t ndx)
{
	Elf32_Ehdr *eh  = (Elf32_Ehdr*)elf;
	Elf32_Off   off = eh->e_phoff + ndx * eh->e_phentsize;

	if((ndx >= eh->e_phnum) || (off >= elfsz))
		return NULL;
	return (Elf32_Phdr*)(elf + off);
}

static Elf32_Shdr *get_shdr(const char *elf, size_t elfsz, size_t ndx)
{
	Elf32_Ehdr *eh  = (Elf32_Ehdr*)elf;
	Elf32_Off   off = eh->e_shoff + ndx * eh->e_shentsize;
	
	if((ndx >= eh->e_shnum) || (off >= elfsz))
		return NULL;
	return (Elf32_Shdr*)(elf + off);
}

/** Return true if two zero-terminated strings are equal. */
static int Sequal(const char *s1, const char *s2)
{
	while(*s1 == *s2) {
		if(!*s1)
			return 1;
		++s1; ++s2;
	}
	return 0;
}

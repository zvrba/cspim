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
#include "mips_endian.h"

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
	
	if(check_eh_limits(eh, elfsz))
		return -1;
	if(load_segments(pcpu))
		return -1;
	if(get_symtab(elf, elfsz, &pcpu->shsymtab, &pcpu->shsymstr))
		return -1;
	if(te32toh(eh->e_entry) >= pcpu->memsz - pcpu->stksz)
		return -1;
	
	/* Segments successfully loaded; set up registers for program start. */

	symgp  = mips_elf_find_symbol(pcpu, "_gp");
	if(!symgp || (te16toh(symgp->st_shndx) != SHN_ABS)
	   || (te32toh(symgp->st_value) >= pcpu->memsz - pcpu->stksz)
	   || (te32toh(symgp->st_value) < MIPS_LOWBASE))
		return -1;
		
	symend = mips_elf_find_symbol(pcpu, "_end");
	if(!symend || (te16toh(symend->st_shndx) != SHN_ABS)
	   || (te32toh(symend->st_value) >= pcpu->memsz - pcpu->stksz)
	   || (te32toh(symend->st_value) < MIPS_LOWBASE))
		return -1;
	
	if((te32toh(eh->e_entry) >= pcpu->memsz - pcpu->stksz)
	   || (te32toh(eh->e_entry) < MIPS_LOWBASE))
		return -1;
	
	pcpu->brk      = te32toh(symend->st_value);
	pcpu->pc       = te32toh(eh->e_entry);
	pcpu->r.ur[28] = te32toh(symgp->st_value);	/* global data pointer */
	pcpu->r.ur[29] = pcpu->memsz - 4;	/* stack pointer */
	pcpu->r.ur[31] = 0;					/* link register */

	return 0;
}

Elf32_Sym *mips_elf_find_symbol(struct mips_cpu *pcpu, const char *name)
{
	Elf32_Shdr *shsymtab = pcpu->shsymtab;
	unsigned    n = te32toh(shsymtab->sh_size) / te32toh(shsymtab->sh_entsize);
	unsigned    i;
	
	if(te32toh(shsymtab->sh_offset)  + te32toh(shsymtab->sh_size) > pcpu->elfsz)
		return NULL;
	
	for(i = 0; i < n; i++) {
	    Elf32_Sym  *sym = get_sym(pcpu->elf, pcpu->elfsz, shsymtab, i);
	    const char *symname;

	    if(!sym)
			return NULL;
	    if(ELF_ST_BIND(te8toh(sym->st_info)) != STB_GLOBAL)
			continue;
		
	    symname = get_string(pcpu->elf, pcpu->elfsz,
				pcpu->shsymstr, te32toh(sym->st_name));
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
	unsigned    n = te32toh(shsymtab->sh_size) / te32toh(shsymtab->sh_entsize);
	Elf32_Addr  min = (unsigned)-1;
	Elf32_Sym  *ret = NULL;
	unsigned    i;

	if((te32toh(shsymtab->sh_offset) + te32toh(shsymtab->sh_size) > pcpu->elfsz)
	|| (addr >= pcpu->memsz))
		return NULL;
	
	for(i = 0; i < n; i++) {
		Elf32_Sym  *sym = get_sym(pcpu->elf, pcpu->elfsz, shsymtab, i);

		if(!sym)
			return NULL;
		if((ELF_ST_BIND(te8toh(sym->st_info)) != STB_GLOBAL) ||
				(ELF_ST_TYPE(te8toh(sym->st_info)) >= STT_SECTION))
			continue;
		
		if(te32toh(sym->st_value) <= addr) {
			Elf32_Addr diff = addr - te32toh(sym->st_value);
			
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
	return get_string(pcpu->elf, pcpu->elfsz, pcpu->shsymstr, te32toh(sym->st_name));
}

static int check_eh_limits(const Elf32_Ehdr *eh, size_t sz)
{
#if defined(MIPS_BE)
	const unsigned char ElfDataType = ELFDATA2MSB;
#else
	const unsigned char ElfDataType = ELFDATA2LSB;
#endif

	if(te16toh(eh->e_ehsize) != sizeof(*eh))
		return -1;

	if((eh->e_ident[0] != '\177') || (eh->e_ident[1] != 'E') ||
			(eh->e_ident[2] != 'L') || (eh->e_ident[3] != 'F'))
		return -1;
	if((eh->e_ident[EI_CLASS] != ELFCLASS32) ||
			(eh->e_ident[EI_DATA] != ElfDataType))
		return -1;
	if((te16toh(eh->e_type) != ET_EXEC) ||(te16toh(eh->e_machine) != EM_MIPS))
		return -1;
	
	if(te32toh(eh->e_phoff) + te16toh(eh->e_phnum) * te16toh(eh->e_phentsize) > sz)
		return -1;
	if((te32toh(eh->e_shoff) + te16toh(eh->e_shnum) * te16toh(eh->e_shentsize) > sz) ||
			(te16toh(eh->e_shstrndx) >= te16toh(eh->e_shnum)))
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
	
	for(i = 0; i < te16toh(eh->e_phnum); i++) {
		Elf32_Phdr *ph = get_phdr(elf, elfsz, i);
		
		if(!ph)
			return -1;
		switch(te32toh(ph->p_type)) {
		case PT_NULL:
			break;
		case PT_LOAD:
			if(load_segment(pcpu, ph))
				return -1;
			break;
		default:
			return -1;
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
	
	if(te32toh(ph->p_offset) + te32toh(ph->p_filesz) > elfsz)
		return -1;
	if(te32toh(ph->p_vaddr) + te32toh(ph->p_memsz) > memsz)
		return -1;
	if(te32toh(ph->p_vaddr) < MIPS_LOWBASE)
		return -1;
	if((te32toh(ph->p_filesz) % 4) || (te32toh(ph->p_memsz) % 4) || (te32toh(ph->p_memsz) < te32toh(ph->p_filesz)))
		return -1;
	
	/*
	  Because of the above checks, the pokes below shall not throw.  The
	  segment data is copied verbatim (identity_poke).  However, the zeros
	  are written through vectored poke in order to have correct data in
	  case memory transformation (e.g. encryption) is applied.
	*/

	for(i = 0; i < te32toh(ph->p_filesz); i += 4)
		mips_identity_poke_uw(pcpu, te32toh(ph->p_vaddr) + i,
							  te32toh(*(mips_uword*)(elf + te32toh(ph->p_offset) + i)));
	for(; i < te32toh(ph->p_memsz); i += 4)
		mips_poke_uw(pcpu, te32toh(ph->p_vaddr) + i, 0);

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
	
	for(i = 0; i < te16toh(eh->e_shnum); i++) {
		Elf32_Shdr *sh = get_shdr(elf, elfsz, i);
		
		if(!sh)
			return -1;
		if(i == te16toh(eh->e_shstrndx)) {
			shshstr = sh;
		} else if(te32toh(sh->sh_type) == SHT_SYMTAB) {
			if(*shsymtab)		/* Don't allow multiple symtabs. */
				return -1;
			*shsymtab = sh;
		}
	}
	
	if(!*shsymtab || !shshstr)
		return -1;
	
	secname = get_string(elf, elfsz, shshstr, te32toh((*shsymtab)->sh_name));
	if(te32toh((*shsymtab)->sh_link) >= te16toh(eh->e_shnum) ||
			!secname || !Sequal(secname, ".symtab"))
		return -1;
	
	*shsymstr = get_shdr(elf, elfsz, te32toh((*shsymtab)->sh_link));
	if(!*shsymstr)
		return -1;
	secname = get_string(elf, elfsz, shshstr, te32toh((*shsymstr)->sh_name));
	if((te32toh((*shsymstr)->sh_type) != SHT_STRTAB) ||
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
	Elf32_Off	off = te32toh(sh->sh_offset);
	size_t		i;

	if((te32toh(sh->sh_type) != SHT_STRTAB) || (ndx >= te32toh(sh->sh_size)) || (off >= elfsz))
		return NULL;
	
	/* Check that the string doesn't go out of bounds of its section.  No
	 * other way than to search for 0-terminator.  NULL-terminated strings
	 * just suck. */

	for(i = ndx; i < te32toh(sh->sh_size); i++)
		if(!elf[off+i])
			return elf+off+ndx;
	
	/* 0-terminator not found before section end. */
	return NULL;
}

static Elf32_Sym *get_sym(const char *elf, size_t elfsz,
		const Elf32_Shdr *sh, size_t ndx)
{
	Elf32_Off toff = ndx * te32toh(sh->sh_entsize);
	Elf32_Off off  = te32toh(sh->sh_offset) + toff;
	
	if((te32toh(sh->sh_type) != SHT_SYMTAB) || (toff >= te32toh(sh->sh_size)) || (off >= elfsz))
		return NULL;
	return (Elf32_Sym*)(elf + off);
}


static Elf32_Phdr *get_phdr(const char *elf, size_t elfsz, size_t ndx)
{
	Elf32_Ehdr *eh  = (Elf32_Ehdr*)elf;
	Elf32_Off   off = te32toh(eh->e_phoff) + ndx * te16toh(eh->e_phentsize);

	if((ndx >= te16toh(eh->e_phnum)) || (off >= elfsz))
		return NULL;
	return (Elf32_Phdr*)(elf + off);
}

static Elf32_Shdr *get_shdr(const char *elf, size_t elfsz, size_t ndx)
{
	Elf32_Ehdr *eh  = (Elf32_Ehdr*)elf;
	Elf32_Off   off = te32toh(eh->e_shoff) + ndx * te16toh(eh->e_shentsize);
	
	if((ndx >= te16toh(eh->e_shnum)) || (off >= elfsz))
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

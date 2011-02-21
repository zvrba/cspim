/* 
 * File:    util.c
 * Author:  zvrba
 * Created: 2008-12-01
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
 * Level 1 run: MIPS simulator that executes another MIPS program (at level 2).
 * This program is actually adapted to running cputorture. Host application is
 * at level 0 (run-lvl0).
 */

#include "cpu.h"
#include "spim.h"				/* At L1, we may execute SPIM syscalls. */

#define	ELFSZ	(1U << 16)		/* max nested (l2) ELF image size */
#define	MEMSZ	(1U << 20)		/* memory for the nested program */
#define	STKSZ	(16U << 10)		/* stack for the nested program */

/* Outwards-visible interfaces. */

char			*l2_memory;
char			l2_elf[ELFSZ];
unsigned		l2_elf_size;
struct mips_cpu	*pcpu;

#define BREAK(n)	do { asm volatile("BREAK %0" : : "K" (n)); } while(0)

static int beginswith(const char *str, const char *pfx);
static void printaddr(const char*, unsigned);

int main(void)
{
	unsigned last_branch = 0;
	enum mips_exception err;
	Elf32_Sym *sym;
	const char *symname;
	int opcode;

	/* Prepare the code */

	l2_memory = sbrk(MEMSZ);
	if(!l2_memory)
		BREAK(1);
	mips_init();
	pcpu = mips_init_cpu(l2_memory, MEMSZ, STKSZ);
	if(mips_elf_load(pcpu, l2_elf, l2_elf_size) < 0)
		BREAK(2);

	/* Execute it */

	while(1) {
		if(pcpu->delay_slot)
			last_branch = pcpu->delay_slot-4;
		
		/* Print all labels as they are encountered. */
		if((sym = mips_elf_find_address(pcpu, pcpu->pc)) && 
		   (sym->st_value == pcpu->pc) &&
		   (symname = mips_elf_get_symname(pcpu, sym))) {
			printaddr("PC=", pcpu->pc);
			printaddr(",last_branch=", last_branch);
			print_char('\n');
		}
	
		if((err = mips_execute(pcpu)) == MIPS_E_OK)
			continue;
		if(err == MIPS_E_BREAK)
			break;
		
		/* Expected exceptions must exactly match PC. */
		if((sym = mips_elf_find_address(pcpu, pcpu->pc)) && 
		   (sym->st_value == pcpu->pc) &&
		   (symname = mips_elf_get_symname(pcpu, sym)) &&
		   (beginswith(symname, "EXN") == 0)) {
			pcpu->pc += 8;	
		} else {
			break;
		}
	}
	
	print_string("L1 FINISHED: exception="); print_int(err);
	print_string(", code="); print_int(mips_break_code(pcpu, &opcode));
	print_string(", "); printaddr("last_branch=", last_branch);
	print_char('\n');
	printaddr("PC=", pcpu->pc);
	print_char('\n');
	
	return 0;
}

static int beginswith(const char *str, const char *pfx)
{
	while(*str++ == *pfx++)
		;						/* empty */
	return *pfx == 0;
}

static void printaddr(const char *label, unsigned addr)
{
	Elf32_Sym *sym;
	const char *symname;

	print_string(label); print_int(addr);
	print_char('[');
	if((sym = mips_elf_find_address(pcpu, addr)) &&
	   (symname = mips_elf_get_symname(pcpu, sym))) {
		print_string(symname);
	} else {
		print_string("(n/a)");
	}
	print_char(']');
}

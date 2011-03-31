/* 
 * File:    torture.c
 * Author:  zvrba
 * Created: 2008-02-24
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
 * MIPS torture test runner.  This is different from mipsexec because the
 * exception handling convention is very specific to the test cases.  Also
 * supports encrypted execution.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "util.h"

#define MEMSZ (2U << 24)
#define STKSZ (16U << 10)

static void execute(struct mips_cpu*);
static void printaddr(struct mips_cpu*, const char*, unsigned);

int main(int argc, char **argv)
{
	char *base;
	struct mips_cpu *pcpu;
	
	if((argc != 2) && (argc != 3)) {
		fprintf(stderr, "USAGE: %s ELF\n", argv[0]);
		exit(1);
	}
	if(!(base = malloc(MEMSZ))) {
		perror("malloc");
		exit(1);
	}

	mips_init(); 
	pcpu = mips_init_cpu(base, MEMSZ, STKSZ);
	prepare_cpu(pcpu, argv[1], (argc == 3) ? argv[2] : NULL);
	execute(pcpu);
	
    return 0;
}

static void execute(struct mips_cpu *pcpu)
{
	size_t last_branch = 0;
	enum mips_exception err;
	Elf32_Sym *sym;
	const char *symname;
	int opcode;

	while(1) {
		if(pcpu->delay_slot)
			last_branch = pcpu->delay_slot-4;
		//printf("%08x\n", pcpu->pc);

		/* Print all labels as they are encountered. */
		if((sym = mips_elf_find_address(pcpu, pcpu->pc)) && 
		   (sym->st_value == pcpu->pc) &&
		   (symname = mips_elf_get_symname(pcpu, sym))) {
			printaddr(pcpu, "PC=", pcpu->pc);
			printaddr(pcpu, ",last_branch=", last_branch);
			printf("\n");
		}

		if((err = mips_execute(pcpu)) == MIPS_E_OK)
			continue;
		
		/* Tolerated exceptions must exactly match PC. */
		if((sym = mips_elf_find_address(pcpu, pcpu->pc)) && 
		   (sym->st_value == pcpu->pc) &&
		   (symname = mips_elf_get_symname(pcpu, sym)) &&
		   (strncmp(symname, "EXN", 3) == 0)) {
			/* Must fix up PC to skip over break that would be triggered had
			 * exception not be taken. */
			pcpu->pc += 8;	
			//printf("Tripped over expected exception %s, continuing at %08x\n",
			//	   symname, pcpu->pc);
		} else {
			break;
		}
	}

	printf("finished: exception=%u, code=0x%x, last_branch=0x%lx",
		   (unsigned)err, mips_break_code(pcpu, &opcode),
		   (unsigned long)last_branch);
	if((sym = mips_elf_find_address(pcpu, last_branch)) &&
	   (symname = mips_elf_get_symname(pcpu, sym)))
		printf("(near %s)", symname);
	printf("\n");
	
	mips_dump_cpu(pcpu);
	
	if((sym = mips_elf_find_address(pcpu, pcpu->pc)) &&
	   (symname = mips_elf_get_symname(pcpu, sym)))
		printf("(near %s)\n", symname);
}


static void printaddr(struct mips_cpu *pcpu, const char *label, unsigned addr)
{
	Elf32_Sym *sym;
	const char *symname;

	printf("%s%.8x[", label, (unsigned int)addr);
	if((sym = mips_elf_find_address(pcpu, addr)) &&
	   (symname = mips_elf_get_symname(pcpu, sym))) {
		printf("%s", symname);
	} else {
		printf("(n/a)");
	}
	printf("]");
}

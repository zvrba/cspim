/* 
 * File:    load-lvl1.c
 * Author:  zvrba
 * Created: 2008-12-03
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
 * Load a MIPS executable into level 1 MIPS interpreter and execute it.  Thus,
 * the executable is now at level 2.  This is proof-of-concept
 * metainterpretation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "rc5-16.h"
#include "util.h"

#define MEMSZ (2U << 20)
#define STKSZ (16U << 10)

int main(int argc, char **argv)
{
	char *base, *l1elf, *l2elf;
	size_t l1sz, l2sz;
	Elf32_Sym *sl2_elf, *sl2_elf_size;
	struct mips_cpu *pcpu;
	
	if(argc != 3) {
		fprintf(stderr, "USAGE: %s LVL1-INTERP LVL2-ELF\n", argv[0]);
		exit(1);
	}
	if(!(base = malloc(MEMSZ))) {
		perror("malloc");
		exit(1);
	}

	/* Prepare L1 interpreter. */

	mips_init();
	pcpu = mips_init_cpu(base, MEMSZ, STKSZ);
	read_elf(argv[1], &l1elf, &l1sz);
	if(mips_elf_load(pcpu, l1elf, l1sz) < 0) {
		fprintf(stderr, "error preparing ELF for execution\n");
		exit(1);
	}

	/* Load L1 interpreter with L2 executable. */

	read_elf(argv[2], &l2elf, &l2sz);

	sl2_elf = mips_elf_find_symbol(pcpu, "l2_elf");
	sl2_elf_size = mips_elf_find_symbol(pcpu, "l2_elf_size");
	if(!sl2_elf || !sl2_elf_size) {
		fprintf(stderr, "L1 interpreter is missing relevant symbols.\n");
		exit(1);
	}
	if(sl2_elf->st_size < l2sz) {
		fprintf(stderr, "insufficient space in L1 interpreter for L2 ELF\n");
		exit(1);
	}

	/* TODO: should be mips_copyout if encryption is in place... */
	memcpy(base + sl2_elf->st_value, l2elf, l2sz);
	*(unsigned int*)(base + sl2_elf_size->st_value) = l2sz;

	/* Execute stuff. */

	execute_loop(pcpu);
	mips_dump_cpu(pcpu);

    return 0;
}


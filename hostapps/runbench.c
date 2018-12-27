/* 
 * File:    run.c
 * Author:  zvrba
 * Created: 2009-08-08
 *
 * ===========================================================================
 * COPYRIGHT (c) 2009 Zeljko Vrba <zvrba.external@zvrba.net>
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
 * Run (a potentially encrypted) benchmark.  Benchmark is special in that
 * they define two symbols: PARAMS, which is an array that specifies input
 * parameters to the benchmark, and TIME, which records the execution time
 * spent within the benchmark.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "mips_endian.h"
#include "rc5-16.h"
#include "util.h"

#define MEMSZ (16U << 20)		/* 16MB, needed for mmult */
#define STKSZ (16U << 10)

/* This destructively modifies the string. */
static void parse_params(struct mips_cpu *pcpu, Elf32_Sym *params, char *argv)
{
	/* The magic constant 4 is sizeof(unsigned) on MIPS I. */
	unsigned addr = te32toh(params->st_value), n = te32toh(params->st_size) / 4;
	unsigned i;
	char *tok = strtok(argv, ",");
	
	for(i = 0; i < n; ++i) {
		if(!tok) {
			fprintf(stderr, "ERROR: too few parameters (%u needed)\n", n);
			exit(1);
		}
		mips_poke_uw(pcpu, addr + 4*i, atoi(tok));
		tok = strtok(NULL, ",");
	}
}

int main(int argc, char **argv)
{
	char *base, *params, *key;
	struct mips_cpu *pcpu;
	Elf32_Sym *s_params, *s_time;
	unsigned long long TIME;
	
	if((argc != 3) && (argc != 4)) {
		fprintf(stderr, "USAGE: %s ELF PARAMS [KEY]\n", argv[0]);
		exit(1);
	}
	params = argv[2];
	key = argc == 4 ? argv[3] : NULL;

	if(!(base = malloc(MEMSZ))) {
		perror("malloc");
		exit(1);
	}

	mips_init();
	pcpu = mips_init_cpu(base, MEMSZ, STKSZ);
	prepare_cpu(pcpu, argv[1], key);

	if(!(s_params = mips_elf_find_symbol(pcpu, "PARAMS")) ||
	   !(s_time = mips_elf_find_symbol(pcpu, "TIME"))) {
		fprintf(stderr, "ERROR: 'PARAMS' and/or 'TIME' symbols not found\n");
		exit(1);
	}

	if((te32toh(s_params->st_size) % 4) || !te32toh(s_params->st_size)) {
		fprintf(stderr, "ERROR: 'PARAMS' has invalid size %u\n", te32toh(s_params->st_size));
		exit(1);
	}

	if(te32toh(s_time->st_size) != 8) {
		fprintf(stderr, "ERROR: 'TIME' has size %u != 8\n", te32toh(s_time->st_size));
		exit(1);
	}

	parse_params(pcpu, s_params, params);
	execute_loop(pcpu);

	TIME = mips_peek_uw(pcpu, te32toh(s_time->st_value) + 4); /* high word */
	TIME = (TIME << 32) | mips_peek_uw(pcpu, te32toh(s_time->st_value)); /* low word */
	printf("%llu\n", TIME);

    return 0;
}

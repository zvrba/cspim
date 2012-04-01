/* 
 * File:    util.c
 * Author:  zvrba
 * Created: 2008-04-09
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
 * Common routines for run and torture.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "rc5-16.h"

static struct rc5_key Gkey;

static mips_uword rc5_peek(MIPS_CPU *pcpu, mips_uword addr);
static void rc5_poke(MIPS_CPU *pcpu, mips_uword addr, mips_uword w);

void read_elf(const char *fname, char **elf, size_t *elfsz)
{
	FILE *f;
	long sz;
	
	if(!(f = fopen(fname, "rb"))) {
		perror("fopen");
		exit(1);
	}
	if(fseek(f, 0, SEEK_END) < 0) {
		perror("fseek");
		exit(1);
	}
	if((sz = ftell(f)) < 0) {
		perror("ftell");
		exit(1);
	}
	if(fseek(f, 0, SEEK_SET) < 0) {
		perror("fseek");
		exit(1);
	}
	if(!(*elf = malloc(sz))) {
		perror("malloc");
		exit(1);
	}
	if(fread(*elf, 1, sz, f) != sz) {
		perror("fread");
		exit(1);
	}
	
	fclose(f);
	*elfsz = sz;
}

int rc5_convert_key(struct rc5_key *pk, const char *hex)
{
	unsigned v;
	int i;
	
	if(strlen(hex) != 32)
		return 0;
	for(i = 0; i < 32; i += 2) {
		if(sscanf(hex+i, "%2x", &v) != 1)
			return 0;
		pk->key[i/2] = v;
	}
	return 1;
}

void prepare_cpu(MIPS_CPU *pcpu, const char *exename, const char *asckey)
{
	char *elf;
	size_t elfsz;
	
	read_elf(exename, &elf, &elfsz);
	if(asckey) {
		if(!rc5_convert_key(&Gkey, asckey)) {
			fprintf(stderr, "can't convert key\n");
			exit(1);
		}
		rc5_setup(&Gkey);
		pcpu->peek_uw = rc5_peek;
		pcpu->poke_uw = rc5_poke;
	}
	if(mips_elf_load(pcpu, elf, elfsz) < 0) {
		fprintf(stderr, "error preparing ELF for execution\n");
		exit(1);
	}
}

void execute_loop(MIPS_CPU *pcpu)
{
	enum mips_exception err;
	Elf32_Sym *sym;
	const char *symname;
	int opcode, break_code;

execute:
	while((err = mips_execute(pcpu)) == MIPS_E_OK)
		;
	break_code = mips_break_code(pcpu, &opcode);
	switch(opcode) {
	case MIPS_I_BREAK:
		fprintf(stderr, "END: BREAK %d\n", break_code);
		return;
	case MIPS_I_SYSCALL:
		if(break_code != MIPS_SPIM_SYSCALL) {
			fprintf(stderr, "END: INVALID SYSCALL CODE %d\n", break_code);
			return;
		}
		if((err = mips_spim_syscall(pcpu)) != 0) {
			fprintf(stderr, "END: SPIM SERVICE %d FAULTED (%d)\n",
					pcpu->r.ur[2], err);
			return;
		}
		mips_resume(pcpu);
		goto execute;
	default:
		fprintf(stderr, "END: EXCEPTION %d AT PC=%08x", err, pcpu->pc);
		if((sym = mips_elf_find_address(pcpu, pcpu->pc)) &&
		   (symname = mips_elf_get_symname(pcpu, sym)))
			fprintf(stderr, " (near %s)", symname);
		fprintf(stderr, "\n");
		break;
	}
	return;
}

static mips_uword rc5_peek(MIPS_CPU *pcpu, mips_uword addr)
{
	mips_uword ret = mips_identity_peek_uw(pcpu, addr);
	unsigned char ctr[4];
	
	ctr[0] = addr;
	ctr[1] = (addr) >> 8;
	ctr[2] = (addr) >> 16;
	ctr[3] = (addr) >> 24;

	rc5_ctr_decrypt(&Gkey, &ctr, &ret, &ret);
	return ret;
}

static void rc5_poke(MIPS_CPU *pcpu, mips_uword addr, mips_uword w)
{
	unsigned char ctr[4];
	
	ctr[0] = addr;
	ctr[1] = (addr) >> 8;
	ctr[2] = (addr) >> 16;
	ctr[3] = (addr) >> 24;

	rc5_ctr_encrypt(&Gkey, &ctr, &w, &w);
	mips_identity_poke_uw(pcpu, addr, w);
}

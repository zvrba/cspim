/* 
 * File:    util.h
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
 *
 * Common utility functions used by apps.
 */

#ifndef UTIL_H__
#define UTIL_H__

#include "cpu.h"
#include "rc5-16.h"

/** Allocate space and read the complete ELF into memory. */
void read_elf(const char *fname, char **elf, size_t *elfsz);

/** Convert key from a string of 32 hex digits. */
int rc5_convert_key(struct rc5_key *pk, const char *hex);

/** Prepare CPU for execution with optional encryption key. */
void prepare_cpu(MIPS_CPU *pcpu, const char *exename, const char *asckey);

/**
 * Execute until exception and report status to stdout.  Handles SPIM
 * syscalls.
 */
void execute_loop(MIPS_CPU *pcpu);

#endif	/* UTIL_H__ */

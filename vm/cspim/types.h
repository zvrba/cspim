/* 
 * File:    types.h
 * Author:  zvrba
 * Created: 2008-11-30
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
 * Basic MIPS I types; version for running within CSPIM (self-hosted).
 */

#ifndef MIPS_TYPES_H_
#define	MIPS_TYPES_H_

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct mips_cpu MIPS_CPU;

typedef unsigned int	mips_insn;
typedef unsigned int	mips_uword;
typedef int				mips_sword;
typedef unsigned short	mips_uhalf;
typedef short			mips_shalf;
typedef unsigned char	mips_ubyte;
typedef signed char		mips_sbyte;
typedef unsigned int	size_t;

/* No C library; we have to implement own setjmp/longjmp. */

typedef struct jmp_buf
{
	mips_uword	ra, sp, fp, gp;
	mips_uword	s[8];
} jmp_buf[1];

int setjmp(jmp_buf);
void longjmp(jmp_buf, int);

#define	NULL	((void*)0)

#ifdef	__cplusplus
}
#endif

#endif	/* MIPS_TYPES_H_ */


/* 
 * File:    types.h
 * Author:  zvrba
 * Created: 2008-04-12
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
 * Basic MIPS I types; hosted version.
 */

#ifndef MIPS_TYPES_H_
#define	MIPS_TYPES_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <setjmp.h>
#include <stddef.h>

typedef struct mips_cpu MIPS_CPU;

#if defined(_MSC_VER) && (_MSC_VER < 1600)

typedef	unsigned int	uint32_t;
typedef int				int32_t;
typedef	unsigned short	uint16_t;
typedef short			int16_t;
typedef unsigned char	uint8_t;
typedef char			int8_t;

#else	/* _MSC_VER < 1600 */
#include <stdint.h>
#endif	/* _MSC_VER < 1600 */

typedef uint32_t mips_insn;
typedef uint32_t mips_uword;
typedef int32_t  mips_sword;
typedef uint16_t mips_uhalf;
typedef int16_t  mips_shalf;
typedef uint8_t  mips_ubyte;
typedef int8_t   mips_sbyte;

#ifdef	__cplusplus
}
#endif

#endif	/* MIPS_TYPES_H_ */


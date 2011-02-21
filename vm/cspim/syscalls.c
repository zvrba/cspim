/* 
 * File:    syscalls.c
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
 *
 * MIPS I system call interface; CSPIM implementation.  Syscalls are just
 * forwarded to the host.
 */

#include "../cpu.h"

#define SYSCALL_MAX 17

static int do_forward(MIPS_CPU*); /* 1 */
typedef int (*syscall_handler)(MIPS_CPU*);

static syscall_handler sys_dispatch[SYSCALL_MAX+1] = {
	0, do_forward, 0, 0, do_forward, do_forward, 0, 0,
	do_forward, do_forward, 0, do_forward, do_forward,
	do_forward, do_forward, do_forward, do_forward, do_forward
};

static int do_forward(MIPS_CPU *pcpu)
{
	return 0;
}


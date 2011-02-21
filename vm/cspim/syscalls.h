/* 
 * File:    syscalls.h
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
 * System calls for CSPIM host.
 */

#ifndef MIPS_SYSCALLS_H_
#define	MIPS_SYSCALLS_H_

#ifdef	__cplusplus
extern "C" {
#endif

/** Magic code in syscall instruction signifying SPIM syscall. */
#define MIPS_SPIM_SYSCALL 0x9107C

/**
 * Process SPIM syscall.
 * 
 * @param pcpu CPU state.
 * @return 0 if the syscall was successfully handled, or -1 if an invalid
 * syscall has been requested, which may happen for two reasons:
 * - Invalid code field in the SYSCALL instruction (not MIPS_SPIM_SYSCALL).
 * - A non-existent service has been requested.
 * In addition, some MIPS_E_* error codes may be returned.  When this happens,
 * an environment may have already been modified.
 *
 * @note "Successfully handled" just means that the system call has been
 * successfully forwarded to the host environment.  Whether the call itself
 * actually succeeded is returned to the simulated program.
 *
 * @note File paths in open() are limited to 256 characters (including \0).
 * Longer filenames shall return MIPS_E_ADDRESS.
 *
 * @note read()/write() syscalls allocate memory.  malloc() failure is reported
 * through MIPS_E_ABORT return code.
 */
int mips_spim_syscall(MIPS_CPU *pcpu);

/** Debug routine: print out CPU state to stdout. */
void mips_dump_cpu(MIPS_CPU *pcpu);

#ifdef	__cplusplus
}
#endif

#endif	/* MIPS_SYSCALLS_H_ */


/* 
 * File:    cpu.h
 * Author:  zvrba
 * Created: 2008-02-11
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
 * MIPS I emulator interfaces.
 * @todo Introduce const-qualifiers where appropriate.
 */

#ifndef MIPS_CPU_H_
#define	MIPS_CPU_H_

#include "opcodes.h"
#include "types.h"
#include "elf.h"
#include "syscalls.h"

#ifdef	__cplusplus
extern "C" {
#endif

/** Lowest valid address in a program. */
#define MIPS_LOWBASE		0x1000

/** Maximum number of file descriptors available to the guest. */
#define MIPS_MAXFDS			16

/** Magic code in syscall instruction signifying SPIM syscall. */
#define MIPS_SPIM_SYSCALL	0x9107C

/**
 * Type of the function which peeks a word at the given address within the MIPS
 * address space (i.e. the address is an offset from pcpu->base).  The caller
 * must ensure that the address is properly aligned and does not point outside
 * of the addressable range.  Otherwise, the result is undefined.
 */
typedef mips_uword (*mips_peek_uw_f)(MIPS_CPU*, mips_uword);

/**
 * Default peek function which performs no transformations.
 * @note Currently works only for little-endian hosts!
 */
mips_uword mips_identity_peek_uw(MIPS_CPU *pcpu, mips_uword addr);

/**
 * Type of function which pokes a word at the given address (an offset from
 * pcpu->base).  Same precautions apply as for the peek function.
 * @see mips_peek_uw_f
 */
typedef void (*mips_poke_uw_f)(MIPS_CPU*, mips_uword, mips_uword);

/**
 * Default poke function which performs no transformations.
 * @note Currently works only for little-endian hosts!
 */
void mips_identity_poke_uw(MIPS_CPU *pcpu, mips_uword addr, mips_uword val);

/** MIPS CPU state. */
struct mips_cpu {
	union {
		mips_sword  sr[32];	
		mips_uword  ur[32];
	} r;								/**!< General-purpose registers. */
	mips_uword		hi, lo;				/**!< mult/div registers. */
	mips_uword		pc;					/**!< Program counter. */
	mips_uword		delay_slot;			/**!< If != 0, delay slot to be executed first. */
	char			*base;				/**!< Memory base address. */
	size_t			memsz;				/**!< Total memory size. */
	size_t			stksz;				/**!< Size reserved for stack. */
	mips_uword		brk;				/**!< The "break". */
	const char		*elf;				/**!< ELF image. */
	size_t			elfsz;				/**!< Size of the ELF image. */
	Elf32_Shdr		*shsymtab;			/**!< Symbol table section header. */
	Elf32_Shdr		*shsymstr;			/**!< Symbol table's string section. */
	jmp_buf			exn;				/**!< Exception handler. */
	mips_peek_uw_f	peek_uw;			/**!< How to read words from memory. */
	mips_poke_uw_f	poke_uw;			/**!< How to write words to memory. */
	int				fds[MIPS_MAXFDS];	/**!< File descriptor map. */
};

/** Execution exception code. */
enum mips_exception {
	MIPS_E_OK = 0,			/**!< No error. */
	MIPS_E_OVERFLOW,		/**!< Integer arithmetic overflow. */
	MIPS_E_ADDRESS,			/**!< Address error (invalid/unaligned). */
	MIPS_E_INVALID,			/**!< Invalid instruction. */
	MIPS_E_SYSCALL,			/**!< SYSCALL instruction. */
	MIPS_E_BREAK,			/**!< BREAK instruction. */
	MIPS_E_ABORT			/**!< Internal emulator error. */
};

/**
 * Global initialization function; must be called before any other
 * mips_* function.
 */
void mips_init(void);

/**
 * Initialize a preallocated CPU state:
 * - fill most of it with 0s.
 * - initialize the file descriptor mapping table: identity mapping for fds
 *   0, 1 and 2; the rest is set to invalid (-1).
 * - set memory access functions to identity mapping
 *
 * @param base  Start of memory area allocated for MIPS memory.
 * @param memsz Size of MIPS memory.
 * @param stksz Stack size reserved for the program.  Stack is carved out
 * 				from memsz.
 * @return Pointer to initialized CPU state.
 *
 * This must be used before mips_elf_load.  Also, memory access functions
 * (peek_uw, poke_uw) should be set immediately after this function has been
 * called, i.e. before mips_elf_load.
 */
MIPS_CPU *mips_init_cpu(char *base, size_t memsz, size_t stksz);

/**
 * Prepare the given ELF image for execution.  This moves segments to
 * appropriate addresses and sets up registers.  No memory is allocated or
 * freed; the start of base is used for the CPU state.  The elf image must not
 * be freed as long as the simulator is running.
 *
 * @param pcpu  Pointer to initialized CPU state.
 * @param elf   Pointer to ELF image to be prepared.
 * @param elfsz Size in bytes of the ELF image.
 *
 * @return 0 on success, or -1 on failure.  There may be a number of reasons
 * because only a small class of specially linked executables is supported, so
 * there is no way to distinguish the exact condition that lead to failure.
 *
 * @note If memory transformation functions are used, two things are
 * noteworthy:
 * - ELF segment data will be loaded into memory VERBATIM, i.e. without any
 *   transformation taking place
 * - however, the BSS segment SHALL use the memory xform function when filling
 *   uninitialized data with 0s.
 * This solution has certain problems, and the solution is yet to be designed.
 */
int mips_elf_load(MIPS_CPU *pcpu, const char *elf, size_t elfsz);

/**
 * Find GLOBAL symbol by name.  This is rahter slow, since the search is
 * seuqential; this is in turn due to how ELF symbol table is organized.
 * Returns pointer to the symbol descriptor, or NULL if the symbol doesn't
 * exist.
 *
 * @param pcpu Pointer to the CPU state.
 * @param name Pointer to null-terminated string.
 * @return Pointer to the ELF symbol structure, or NULL if not found.
 *
 * @note Since GLOBAL symbols must have unique names within the whole
 * executable, there is no possibility of returning multiple results
 * (local names may be identical and be associated with different addresses).
 */
Elf32_Sym *mips_elf_find_symbol(MIPS_CPU *pcpu, const char *name);

/**
 * Find nearest GLOBAL symbol whose address is less than or equal to addr. 
 * Returns NULL only in exceptional cases (e.g. corrupt ELF file).  STT_SECTION
 * and STT_FILE symbols types are ignored during the search.
 *
 * @param pcpu Pointer to CPU state.
 * @param addr Address that has to be located.
 * @return Pointer to the ELF symbol structure.  NULL is returned only in
 * exceptional cases (e.g. corrupt ELF file), because every valid executable
 * must have at least one GLOBAL symbol (_start).
 *
 * @note Even though only GLOBAL symbols are considered, it may still happen
 * that multiple symbols are associated with the same address.  In that
 * case, it is unspecified which symbol will be returned.
 *
 * @todo Design an interface that allows iterating over all symbols
 * associated with an address.
 */
Elf32_Sym *mips_elf_find_address(MIPS_CPU *pcpu, Elf32_Addr addr);

/**
 * Return the symbol's name.
 *
 * @param pcpu Pointer to CPU state.
 * @param sym  Pointer to ELF symbol structure.
 * @return Pointer to null-terminated string, or NULL.
 */
const char *mips_elf_get_symname(MIPS_CPU *pcpu, Elf32_Sym *sym);

/**
 * Decode MIPS instruction.  This just decodes the instruction and successful
 * return value does not guarantee that the complete encoding is valid:
 * certain invalid combinations of operands are detected only during 
 * simulation.
 *
 * @param insn A single MIPS instruction (all are exactly 32 bits).
 * @return One of MIPS_I_* constants, or -1 if the instruction is invalid.
 */
int mips_decode(mips_insn insn);

/**
 * Execute a single instruction.  If the instruction cannot be executed, the
 * CPU's state is left unchanged, and an exception is raised.
 *
 * @param pcpu Pointer to CPU state.
 * @return None, but may throw an exception on failure.
 */
enum mips_exception mips_execute(MIPS_CPU *pcpu);

/**
 * Check whether the execution stopped due to SYSCALL/BREAK instruction,
 * and if so get the code field.
 *
 * @param pcpu   CPU state
 * @param opcode Opcode (MIPS_I_*) of the faulting instruction.
 * @return If the faulting instruction is SYSCALL/BREAK, the code field is
 * returned.  Otherwise, -1 is returned.
 */
int mips_break_code(MIPS_CPU *pcpu, int *opcode);

/**
 * Resume execution after exception.  If the exception was caused by SYSCALL
 * or BREAK, advances the PC first.  Otherwise, it restarts the faulting
 * instruction.
 *
 * @param pcpu CPU state.
 * @return 0 on success, -1 on failure (invalid CPU state).
 */
int mips_resume(MIPS_CPU *pcpu);

/*@{*/
/**
 * Memory access to the simulated CPU with range and alignment checks.  Peek
 * functions come in signed/unsigned variants for bytes, halfwords and words,
 * while poke functions come only in unsigned variant.  These functions are
 * implemented in terms of peek_uw and poke_uw.
 *
 * @param pcpu CPU state.
 * @param addr Address to read/write.
 * @param val  Value to write (in case of poke).
 * @return Read value.
 *
 * @warning These functions have undefined behavior if addr is not within the
 * range of MIPS address space!  It is strongly recommended to use
 * mips_copyout and mips_copyin for transfers of all sizes!
 */
mips_sbyte mips_peek_sb(MIPS_CPU *pcpu, mips_uword addr);
mips_shalf mips_peek_sh(MIPS_CPU *ppcu, mips_uword addr);
mips_ubyte mips_peek_ub(MIPS_CPU *pcpu, mips_uword addr);
mips_uhalf mips_peek_uh(MIPS_CPU *pcpu, mips_uword addr);
mips_uword mips_peek_uw(MIPS_CPU *pcpu, mips_uword addr);
void mips_poke_ub(MIPS_CPU *pcpu, mips_uword addr, mips_ubyte val);
void mips_poke_uh(MIPS_CPU *pcpu, mips_uword addr, mips_uhalf val);
void mips_poke_uw(MIPS_CPU *pcpu, mips_uword addr, mips_uword val);
/*@}*/

/*@{*/
/**
 * Copy (potentially unaligned) data from host to the simulator (out), or from
 * simulator to the host (in).  In the case of failure, the state of the MIPS
 * simulator has not been altered.
 *
 * @param pcpu CPU state.
 * @param dst  Destination address in MIPS (out) or host (in).
 * @param src  Source address in host (out) or MIPS (in).
 * @param n    Number of bytes to copy.
 * @return 0 on success, -1 on failure (i.e. the transfer would access memory
 * outside of MIPS segment).
 */
int mips_copyout(MIPS_CPU *pcpu, mips_uword dst, void *src, mips_uword n);
int mips_copyin(MIPS_CPU *pcpu, void *dst, mips_uword src, mips_uword n);
/*@}*/

#ifdef	__cplusplus
}
#endif

#endif	/* MIPS_CPU_H_ */


/* 
 * File:    cpuemu.c
 * Author:  zvrba
 * Created: 2008-02-13
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
 * MIPS I execution engine.  ONLY little-endian functionality is implemented.
 * This in general means that the host execution engine has to have the same
 * endianness as the simulated CPU!
 *
 * @todo change documentation with regard to jump instruction exceptions!
 */

#include "cpu.h"
#include "mips_endian.h"

#if defined(_MSC_VER) && (_MSC_VER < 1600)
#define	inline	__inline
#endif

static void do_dispatch(int, int, MIPS_CPU*);
static void do_jump(int, int, MIPS_CPU*);
static void do_branch(int, int, MIPS_CPU*);
static void do_alui(int, int, MIPS_CPU*);
static void do_loadstore(int, int, MIPS_CPU*);
static void do_alu3(int, int, MIPS_CPU*);
static void do_divmult(int, int, MIPS_CPU*);

static inline void validate_address(MIPS_CPU*, mips_uword, int);
static inline mips_sword add_ovf(MIPS_CPU*, mips_sword, mips_sword);
static inline mips_sword sub_ovf(MIPS_CPU*, mips_sword, mips_sword);
static inline void multu(mips_uword, mips_uword, mips_uword*, mips_uword*);
static inline void mult(mips_sword, mips_sword, mips_uword*, mips_uword*);

#define PC (pcpu->pc)
#define DELAY_SLOT (pcpu->delay_slot)
#define MEMSZ (pcpu->memsz)

#define uR(i) (pcpu->r.ur[i])
#define sR(i) (pcpu->r.sr[i])
#define fRS ((insn >> 21) & 0x1F)
#define fRT ((insn >> 16) & 0x1F)
#define fRD ((insn >> 11) & 0x1F)
#define uRS uR(fRS)
#define sRS sR(fRS)
#define uRT uR(fRT)
#define sRT sR(fRT)
#define uRD uR(fRD)
#define sRD sR(fRD)

#define uW(r, expr) if(f ## r) u ## r = expr
#define sW(r, expr) if(f ## r) s ## r = expr

#define SEXTH2W(n) (((mips_sword)(n) << 16) >> 16)
#define sIMM SEXTH2W(insn & 0xFFFF) /* sign-ext immediate as signed */
#define uIMM ((mips_uword)sIMM)		/* sign-ext immediate as unsigned */
#define zIMM (insn & 0xFFFF)		/* zero-ext immediate as unsigned */

#define sMEMB(i) mips_peek_sb(pcpu, i)
#define uMEMB(i) mips_peek_ub(pcpu, i)
#define sMEMH(i) mips_peek_sh(pcpu, i)
#define uMEMH(i) mips_peek_uh(pcpu, i)
#define uMEMW(i) mips_peek_uw(pcpu, i)

#define THROW(pcpu, code) longjmp((pcpu)->exn, code)

void mips_init_hostdata(MIPS_CPU*);

MIPS_CPU *mips_init_cpu(char *base, size_t memsz, size_t stksz)
{
	MIPS_CPU *pcpu = (MIPS_CPU*)base;
	unsigned i;
	
	/* Reset all fields. */

	for(i = 0; i < sizeof(*pcpu); i++)
		base[i] = 0;

	/* Initialize file descriptor map (make 0,1,2 available) */

	for(i = 0; i < MIPS_MAXFDS; i++)
		pcpu->fds[i] = (i < 3) ? i : -1;

	/* Initialize the rest. */
	
	pcpu->base  = base;
	pcpu->memsz = memsz;
	pcpu->stksz = stksz;
	pcpu->elf   = NULL;
	pcpu->elfsz = 0;

	pcpu->peek_uw = mips_identity_peek_uw;
	pcpu->poke_uw = mips_identity_poke_uw;

	return pcpu;
}

/**
 * @note Using 0 to mark that nothing is to be executed in the DELAY_SLOT is OK
 * becase the execution is not allowed to wrap around memory limit, and
 * delay slot is always after a branch instruction (i.e. there cannot be
 * an instruction "before" address 0).
 */
enum mips_exception mips_execute(MIPS_CPU *pcpu)
{
	int fdelay, opcode, err;
	mips_uword insn;
	
	if((err = setjmp(pcpu->exn)) == 0) {
		if(DELAY_SLOT) {
			insn   = uMEMW(DELAY_SLOT);
			fdelay = 1;		/* mark that delay slot is being executed */
		} else {
			insn   = uMEMW(PC);
			fdelay = 0;		/* delay slot is NOT being executed */
		}

		opcode = mips_decode(insn);

		if((opcode == MIPS_I_SPECIAL) || (opcode == MIPS_I_REGIMM))
			THROW(pcpu, MIPS_E_ABORT);
		if(opcode < 0) THROW(pcpu, MIPS_E_INVALID);

		if(uR(0) != 0) THROW(pcpu, MIPS_E_ABORT);
		do_dispatch(insn, opcode, pcpu);
		if(uR(0) != 0) THROW(pcpu, MIPS_E_ABORT);

		/* This point is reached only if the instruction is successfully executed,
		 * so update PC/delay.  If delay slot has just been executed (fdelay true),
		 * reset it and do NOT update PC (it has been set by the previous branch
		 * instruction).  Otherwise, delay slot must have been 0 before instruction
		 * execution, so update PC only if the delay slot has NOT been set (i.e. a
		 * branch instruction, which itself adjusts PC, has NOT been executed). */

		if(fdelay)
			DELAY_SLOT = 0;
		else if(!DELAY_SLOT)
			PC += 4;
		return MIPS_E_OK;
	}
	return err;
}

/**
 * @note The MIPS documentation states that BREAK instruction accepts a code
 * of 20 bits as part encoding.  However, gcc assembler accepts only 10 bits
 * (constant in range 0-1023) and encodes it in bits 16-25.  This is taken
 * into account in this function.
 */
int mips_break_code(MIPS_CPU *pcpu, int *opcode)
{
	if(setjmp(pcpu->exn) == 0) {
		mips_insn insn = uMEMW(PC);

		*opcode = mips_decode(insn);
		switch(*opcode) {
		case MIPS_I_BREAK:
			return (insn >> 16) & 0x3FF;
		case MIPS_I_SYSCALL:
			return (insn >> 6) & 0xFFFFF;
		}
	}
	return -1;
}

int mips_resume(MIPS_CPU *pcpu)
{
	if(setjmp(pcpu->exn) == 0) {
		mips_insn insn = uMEMW(PC);
		int opcode = mips_decode(insn);

		if((opcode == MIPS_I_BREAK) || (opcode == MIPS_I_SYSCALL)) {
			PC += 4;
			return 0;
		}
	}
	return -1;
}

/** Instruction dispatcher. */
static void do_dispatch(int insn, int opcode, MIPS_CPU *pcpu)
{
	switch(opcode) {
	case MIPS_I_JAL:	case MIPS_I_J:	case MIPS_I_JR:
	case MIPS_I_JALR:
		do_jump(insn, opcode, pcpu);
		break;
		
	case MIPS_I_BEQ:	case MIPS_I_BNE:	case MIPS_I_BLEZ:
	case MIPS_I_BGTZ:	case MIPS_I_BLTZ:	case MIPS_I_BGEZ:
	case MIPS_I_BLTZAL:	case MIPS_I_BGEZAL:
		do_branch(insn, opcode, pcpu);
		break;

	case MIPS_I_ADDI:	case MIPS_I_ADDIU:	case MIPS_I_SLTI:
	case MIPS_I_SLTIU:	case MIPS_I_ANDI:	case MIPS_I_ORI:
	case MIPS_I_XORI:
		do_alui(insn, opcode, pcpu);
		break;
		
	case MIPS_I_SLL:	case MIPS_I_SRL:	case MIPS_I_SRA:
	case MIPS_I_SLLV:	case MIPS_I_SRLV:	case MIPS_I_SRAV:
	case MIPS_I_ADD:	case MIPS_I_ADDU:	case MIPS_I_SUB:
	case MIPS_I_SUBU:	case MIPS_I_AND:	case MIPS_I_OR:
	case MIPS_I_XOR:	case MIPS_I_NOR:	case MIPS_I_SLT:
	case MIPS_I_SLTU:
		do_alu3(insn, opcode, pcpu);
		break;
		
	case MIPS_I_LB:		case MIPS_I_LH:		case MIPS_I_LWL:
	case MIPS_I_LW:		case MIPS_I_LBU:	case MIPS_I_LHU:
	case MIPS_I_LWR:	case MIPS_I_SB:		case MIPS_I_SH:
	case MIPS_I_SWL:	case MIPS_I_SW:		case MIPS_I_SWR:
	case MIPS_I_LUI:
		do_loadstore(insn, opcode, pcpu);
		break;
		
	case MIPS_I_MULT:	case MIPS_I_MULTU:	case MIPS_I_DIV:
	case MIPS_I_DIVU:	case MIPS_I_MFHI:	case MIPS_I_MTHI:
	case MIPS_I_MFLO:	case MIPS_I_MTLO:
		do_divmult(insn, opcode, pcpu);
		break;
		
	case MIPS_I_SYSCALL:
		THROW(pcpu, MIPS_E_SYSCALL);
		break;
		
	case MIPS_I_BREAK:
		THROW(pcpu, MIPS_E_BREAK);
		break;

	default:
		/* Invalid instructions are handled before switch() */
		THROW(pcpu, MIPS_E_ABORT);
	}
}

/** Jump opcodes. */
static void  do_jump(int insn, int opcode, MIPS_CPU *pcpu)
{
	mips_uword nds = PC+4;
	mips_uword npc;

	if(DELAY_SLOT) THROW(pcpu, MIPS_E_INVALID);
	
	switch(opcode) {
	case MIPS_I_JAL:
		uR(31) = PC+8;
		/* FALLTHROUGH */
	case MIPS_I_J:
		npc = (nds & 0xF8000000) | ((insn & 0x03FFFFFF) << 2);
		break;
		
	case MIPS_I_JALR:
		if(fRT)
			THROW(pcpu, MIPS_E_INVALID);
		uW(RD, PC+8);
		npc = uRS;
		break;
		
	case MIPS_I_JR:
		if((insn >> 6) & 0x7FFF)
			THROW(pcpu, MIPS_E_INVALID);
		npc = uRS;
		break;
		
	default:
		THROW(pcpu, MIPS_E_ABORT);
	}
	
	DELAY_SLOT = nds;	
	PC = npc;
}

/** Branching opcodes. */
static void do_branch(int insn, int opcode, MIPS_CPU *pcpu)
{
	int s = -1;

	if(DELAY_SLOT) THROW(pcpu, MIPS_E_INVALID);

	switch(opcode) {
	case MIPS_I_BEQ:
		s = uRS == uRT;
		break;
		
	case MIPS_I_BNE:
		s = uRS != uRT;
		break;
		
	case MIPS_I_BLEZ:	
		if(fRT != 0)
			THROW(pcpu, MIPS_E_INVALID);
		s = sRS <= 0;
		break;
		
	case MIPS_I_BGTZ:
		if(fRT != 0)
			THROW(pcpu, MIPS_E_INVALID);
		s = sRS > 0;
		break;
		
	case MIPS_I_BLTZAL:
		uR(31) = PC + 8;
		/* FALLTHROUGH */
	case MIPS_I_BLTZ:
		s = sRS < 0;
		break;
		
	case MIPS_I_BGEZAL:
		uR(31) = PC + 8;
		/* FALLTHROUGH */
	case MIPS_I_BGEZ:
		s = sRS >= 0;
		break;
		
	default:
		THROW(pcpu, MIPS_E_ABORT);
	}
	
	if(s == -1)
		THROW(pcpu, MIPS_E_ABORT);
	if(s) {
		mips_uword offset = (mips_uword)((((mips_sword)insn) << 16) >> 14);
		mips_uword nds    = PC + 4;
		mips_uword npc    = nds + offset;
	
		DELAY_SLOT = nds;	
		PC = npc;
	}
}

/** ALU operations with immediate constant. */
static void do_alui(int insn, int opcode, MIPS_CPU *pcpu)
{
	switch(opcode) {
	case MIPS_I_ADDI:	sW(RT, add_ovf(pcpu, sRS, sIMM)); break;
	case MIPS_I_ADDIU:	uW(RT, uRS + uIMM); break;
	case MIPS_I_SLTI:	uW(RT, sRS < sIMM); break;
	case MIPS_I_SLTIU:	uW(RT, uRS < uIMM); break;
	case MIPS_I_ANDI:	uW(RT, uRS & zIMM); break;
	case MIPS_I_ORI:	uW(RT, uRS | zIMM); break;
	case MIPS_I_XORI:	uW(RT, uRS ^ zIMM); break;
	default:
		THROW(pcpu, MIPS_E_ABORT);
	}
}

/** Load/store instructions. */
static void do_loadstore(int insn, int opcode, MIPS_CPU *pcpu)
{
	mips_uword ea = uRS + uIMM;

	switch(opcode) {
	case MIPS_I_LB:		sW(RT, sMEMB(ea)); break;
	case MIPS_I_LBU:	uW(RT, uMEMB(ea)); break;
	case MIPS_I_LH:		sW(RT, sMEMH(ea)); break;
	case MIPS_I_LHU:	uW(RT, uMEMH(ea)); break;
	case MIPS_I_LW:		uW(RT, uMEMW(ea)); break;
	case MIPS_I_SB:		mips_poke_ub(pcpu, ea, uRT); break;
	case MIPS_I_SH:		mips_poke_uh(pcpu, ea, uRT); break;
	case MIPS_I_SW:		mips_poke_uw(pcpu, ea, uRT); break;
		
	case MIPS_I_LUI:
		if(fRS != 0) THROW(pcpu, MIPS_E_INVALID);
		uW(RT, zIMM << 16);
		break;

	/* The code for LWL/LWR/SWL/SWR takes care to not make shifts larger
	 * than 31 bits (shifts larger or equal to word width are undefined
	 * behavior in C ).  This implementation is little-endian! */
		
	case MIPS_I_LWL: {
		int s = ea & 3;
		mips_uword utmp1 = uMEMW(ea - s) << 8*(3-s);
		mips_uword utmp2 = s != 3 ? uRT & (0xFFFFFFFFU >> 8*(s+1)) : 0;
		uW(RT, utmp1 | utmp2);
		break;
	}
		
	case MIPS_I_LWR: {
		int s = ea & 3;
		mips_uword utmp1 = uMEMW(ea - s) >> 8*s;
		mips_uword utmp2 = s != 0 ? uRT & (0xFFFFFFFFU << 8*(4-s)) : 0;
		uW(RT, utmp1 | utmp2);
		break;
	}
		
	case MIPS_I_SWL: {
		int s = ea & 3;
		mips_uword utmp1 = s != 3 ? uMEMW(ea - s) & (0xFFFFFFFFU << 8*(s+1)) : 0;
		mips_uword utmp2 = uRT >> 8*(3-s);
		mips_poke_uw(pcpu, ea-s, utmp1 | utmp2);
		break;
	}
		
	case MIPS_I_SWR: {
		int s = ea & 3;
		mips_uword utmp1 = s != 0 ? uMEMW(ea - s) & (0xFFFFFFFFU >> 8*(4-s)) : 0;
		mips_uword utmp2 = uRT << 8*s;
		mips_poke_uw(pcpu, ea-s, utmp1 | utmp2);
		break;
	}
		
	default:
		THROW(pcpu, MIPS_E_ABORT);
	}
}

static void do_alu3(int insn, int opcode, MIPS_CPU *pcpu)
{
#define Z_(f) if(f) THROW(pcpu, MIPS_E_INVALID)
	unsigned fSA = (insn >> 6) & 0x1F;

	switch(opcode) {
	case MIPS_I_SLL:	Z_(fRS); uW(RD, uRT << fSA);				break;
	case MIPS_I_SRL:	Z_(fRS); uW(RD, uRT >> fSA);				break;
	case MIPS_I_SRA:	Z_(fRS); sW(RD, sRT >> fSA);				break;
	case MIPS_I_SLLV:	Z_(fSA); uW(RD, uRT << (uRS & 0x1F));		break;
	case MIPS_I_SRLV:	Z_(fSA); uW(RD, uRT >> (uRS & 0x1F));		break; 
	case MIPS_I_SRAV:	Z_(fSA); sW(RD, sRT >> (uRS & 0x1F));		break;
	case MIPS_I_ADD:	Z_(fSA); sW(RD, add_ovf(pcpu, sRS, sRT));	break;
	case MIPS_I_ADDU:	Z_(fSA); uW(RD, uRS + uRT);					break;
	case MIPS_I_SUB:	Z_(fSA); sW(RD, sub_ovf(pcpu, sRS, sRT));	break;
	case MIPS_I_SUBU:	Z_(fSA); uW(RD, uRS - uRT);					break;
	case MIPS_I_AND:	Z_(fSA); uW(RD, uRS & uRT);					break;
	case MIPS_I_OR:		Z_(fSA); uW(RD, uRS | uRT);					break;
	case MIPS_I_XOR:	Z_(fSA); uW(RD, uRS ^ uRT);					break;
	case MIPS_I_NOR:	Z_(fSA); uW(RD, ~(uRS | uRT));				break;
	case MIPS_I_SLT:	Z_(fSA); uW(RD, sRS < sRT);					break;
	case MIPS_I_SLTU:	Z_(fSA); uW(RD, uRS < uRT);					break;
	default:
		THROW(pcpu, MIPS_E_ABORT);
	}
#undef Z_
}

static void do_divmult(int insn, int opcode, MIPS_CPU *pcpu)
{
#define Z_(or, mask) if((or) || ((insn >> 6) & mask)) THROW(pcpu, MIPS_E_INVALID)
#define D_(rs, rt) pcpu->lo = pcpu->hi = (unsigned)-1; if(rt) { pcpu->lo = rs / rt; pcpu->hi = rs % rt; }
	switch(opcode) {
	case MIPS_I_MULT:	Z_(0, 0x3FF);  mult(sRS, sRT, &pcpu->hi, &pcpu->lo);  break;
	case MIPS_I_MULTU:  Z_(0, 0x3FF);  multu(uRS, uRT, &pcpu->hi, &pcpu->lo); break;
	case MIPS_I_MTHI:	Z_(0, 0x7FFF); pcpu->hi = uRS;                        break;
	case MIPS_I_MTLO:	Z_(0, 0x7FFF); pcpu->lo = uRS;                        break;
	case MIPS_I_DIV:	Z_(0, 0x3FF);  D_(sRS, sRT);                          break;
	case MIPS_I_DIVU:	Z_(0, 0x3FF);  D_(uRS, uRT);                          break;
	case MIPS_I_MFHI:	Z_(fRS || fRT, 0x1F); uW(RD, pcpu->hi);               break;
	case MIPS_I_MFLO:	Z_(fRS || fRT, 0x1F); uW(RD, pcpu->lo);               break;
	default:
		THROW(pcpu, MIPS_E_ABORT);
	}
#undef Z_
#undef D_
}

/**
 * @note Address validation is done in mips_{peek,poke}_* functions, so it
 * is not neccessary to do it here again.
 */
mips_uword mips_identity_peek_uw(MIPS_CPU *pcpu, mips_uword addr)
{
	return te32toh(*(mips_uword*)(pcpu->base + addr));
}

/** Identity function to poke a word. */
void mips_identity_poke_uw(MIPS_CPU *pcpu, mips_uword addr, mips_uword w)
{
	*(mips_uword*)(pcpu->base + addr) = htote32(w);
}

/* The peek and poke functions must be implemented exclusively in terms of
 * mips_peek_uw and mips_poke_uw functions.  They do not check for proper
 * alignment for the datatype; this must be done in higher-level routines. */

mips_sbyte mips_peek_sb(MIPS_CPU *pcpu, mips_uword addr)
{
	int s = addr & 3U;
	validate_address(pcpu, addr, 0);
	{
		mips_uword w = te32toh(pcpu->peek_uw(pcpu, addr-s));
		return (mips_sbyte)(w >> (8*s));
	}
}

mips_shalf mips_peek_sh(MIPS_CPU *pcpu, mips_uword addr)
{
	int s = addr & 3U;
	validate_address(pcpu, addr, 1);
	{
		mips_uword w = te32toh(pcpu->peek_uw(pcpu, addr-s));
		return (mips_shalf)htote16((mips_uhalf)(w >> (8*s)));
	}
}

mips_ubyte mips_peek_ub(MIPS_CPU *pcpu, mips_uword addr)
{
	int s = addr & 3U;
	validate_address(pcpu, addr, 0);
	{
		mips_uword w = te32toh(pcpu->peek_uw(pcpu, addr-s));
		return (mips_ubyte)(w >> (8*s));
	}
}

mips_uhalf mips_peek_uh(MIPS_CPU *pcpu, mips_uword addr)
{
	int s = addr & 3U;
	validate_address(pcpu, addr, 1);
	{
		mips_uword w = te32toh(pcpu->peek_uw(pcpu, addr-s));
		return htote16((mips_uhalf)(w >> (8*s)));
	}
}

mips_uword mips_peek_uw(MIPS_CPU *pcpu, mips_uword addr)
{
	validate_address(pcpu, addr, 3);
	return pcpu->peek_uw(pcpu, addr);
}

// TODO: will these byte to word transformations work even with byte swapping? We want the byte to always land in the same place within the word
void mips_poke_ub(MIPS_CPU *pcpu, mips_uword addr, mips_ubyte v)
{
	int s = addr & 3U;
	validate_address(pcpu, addr, 0);
	{
		mips_uword w = te32toh(pcpu->peek_uw(pcpu, addr-s));
		mips_uword m = ~(0xFFU << (8*s));
		pcpu->poke_uw(pcpu, addr-s, (w & m) | ((mips_uword)v << (8*s)));
	}
}

void mips_poke_uh(MIPS_CPU *pcpu, mips_uword addr, mips_uhalf v)
{
	int s = addr & 3U;
	validate_address(pcpu, addr, 1);
	{
		mips_uword w = te32toh(pcpu->peek_uw(pcpu, addr-s));
		mips_uword m = ~(0xFFFFU << (8*s));
		pcpu->poke_uw(pcpu, addr-s, (w & m) | ((mips_uword)v << (8*s)));
	}
}

void mips_poke_uw(MIPS_CPU *pcpu, mips_uword addr, mips_uword v)
{
	validate_address(pcpu, addr, 3);
	pcpu->poke_uw(pcpu, addr, v);
}

int mips_copyout(MIPS_CPU *pcpu, mips_uword dst, void *src, mips_uword n)
{
	mips_ubyte *pch = src;
	
	if(dst + n >= pcpu->memsz)
		return -1;
	
	/* TODO: this is _very_ inefficient (look up the implementation of byte
	 * peek/poke).  Should be fixed to do copying in at least words, after
	 * the unaligned part has been copied. */
	
	while(n--)
		mips_poke_ub(pcpu, dst++, *pch++);
	return 0;
}

int mips_copyin(MIPS_CPU *pcpu, void *dst, mips_uword src, mips_uword n)
{
	mips_ubyte *pch = dst;

	/* TODO: this is _very_ inefficient (look up the implementation of byte
	 * peek/poke).  Should be fixed to do copying in at least words, after
	 * the unaligned part has been copied. */
	
	if(src + n >= pcpu->memsz)
		return -1;
	
	while(n--)
		*pch++ = mips_peek_ub(pcpu, src++);
	return 0;
}

/**
 * Check that addr is within the MIPS memory range and is aligned at align,
 * which must be 1 less than the required alignment (e.g. align == 3 if
 * alignment at 4-byte boundary is required.  Throws MIPS_E_ADDRESS if the
 * constraings are not satisfied.
 */
static inline void validate_address(MIPS_CPU *pcpu,
		mips_uword addr, int align)
{
	if((addr < MIPS_LOWBASE) || (addr >= pcpu->memsz) || (addr & align))
		THROW(pcpu, MIPS_E_ADDRESS);
}

/** Perform signed addition, but throw exception in case of overflow. */
static inline mips_sword add_ovf(MIPS_CPU *pcpu,
		mips_sword x, mips_sword y)
{
	long long z = (long long)x + (long long)y;
	
	if((z < -2147483648LL) || (z > 2147483647LL))
		THROW(pcpu, MIPS_E_OVERFLOW);
	return (mips_sword)z;
}

/** Perform signed subtraction, but throw exception in case of overflow. */
static inline mips_sword sub_ovf(MIPS_CPU *pcpu,
		mips_sword x, mips_sword y)
{
	long long z = (long long)x - (long long)y;

	if((z < -2147483648LL) || (z > 2147483647LL))
		THROW(pcpu, MIPS_E_OVERFLOW);
	return (mips_sword)z;
}

/** Calculate unsigned 32x32->64 product using only 16x16->32 multiply. */
static inline void multu(mips_uword x, mips_uword y,
		mips_uword *hi, mips_uword *lo)
{
	unsigned long long r = (unsigned long long)x * (unsigned long long)y;
	*hi = r >> 32;
	*lo = r & 0xFFFFFFFFU;
}

/** Calculate signed 32x32->64 product using only 16x16->32 multiply. */
static inline void mult(mips_sword x, mips_sword y,
		mips_uword *hi, mips_uword *lo)
{
	long long r = (long long)x * (long long)y;
	*hi = (unsigned long long)r >> 32;
	*lo = (unsigned long long)r & 0xFFFFFFFFU;
}

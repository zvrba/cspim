/* 
 * File:    opcodes.c
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
 * MIPS I instruction decoder.
 */

#include "opcodes.h"
#include "types.h"

static unsigned long Gvalid_opcodes[8];

#define NINSN 60
static int Gopcode_list[NINSN] = {
	MIPS_I_SPECIAL,
	MIPS_I_REGIMM,
	MIPS_I_J,
	MIPS_I_JAL,
	MIPS_I_BEQ,
	MIPS_I_BNE,
	MIPS_I_BLEZ,
	MIPS_I_BGTZ,
	MIPS_I_ADDI,
	MIPS_I_ADDIU,
	MIPS_I_SLTI,
	MIPS_I_SLTIU,
	MIPS_I_ANDI,
	MIPS_I_ORI,
	MIPS_I_XORI,
	MIPS_I_LUI,
	MIPS_I_LB,
	MIPS_I_LH,
	MIPS_I_LWL,
	MIPS_I_LW,
	MIPS_I_LBU,
	MIPS_I_LHU,
	MIPS_I_LWR,
	MIPS_I_SB,
	MIPS_I_SH,
	MIPS_I_SWL,
	MIPS_I_SW,
	MIPS_I_SWR,
	MIPS_I_SLL,
	MIPS_I_SRL,
	MIPS_I_SRA,
	MIPS_I_SLLV,
	MIPS_I_SRLV,
	MIPS_I_SRAV,
	MIPS_I_JR,
	MIPS_I_JALR,
	MIPS_I_SYSCALL,
	MIPS_I_BREAK,
	MIPS_I_MFHI,
	MIPS_I_MTHI,
	MIPS_I_MFLO,
	MIPS_I_MTLO,
	MIPS_I_MULT,
	MIPS_I_MULTU,
	MIPS_I_DIV,
	MIPS_I_DIVU,
	MIPS_I_ADD,
	MIPS_I_ADDU,
	MIPS_I_SUB,
	MIPS_I_SUBU,
	MIPS_I_AND,
	MIPS_I_OR,
	MIPS_I_XOR,
	MIPS_I_NOR,
	MIPS_I_SLT,
	MIPS_I_SLTU,
	MIPS_I_BLTZ,
	MIPS_I_BGEZ,
	MIPS_I_BLTZAL,
	MIPS_I_BGEZAL
};

#define SETBIT(v, n) (v[n>>5] |= (1U << (n&31)))
#define GETBIT(v, n) (v[n>>5] & (1U << (n&31)))

void mips_init(void)
{
	int i;
	
	for(i = 0; i < 8; i++)
		Gvalid_opcodes[i] = 0;
	for(i = 0; i < NINSN; i++)
		SETBIT(Gvalid_opcodes, Gopcode_list[i]);
}

#define fOPCODE(insn) (insn >> 26)
#define fFUNCTION(insn) (insn & 0x3F)
#define fRT(insn) ((insn >> 16) & 0x1F)

int mips_decode(mips_insn insn)
{
	int opcode = fOPCODE(insn);
	
	switch(opcode) {
	case MIPS_I_SPECIAL:
		opcode = fFUNCTION(insn) | 0100;
		break;

	case MIPS_I_REGIMM:
		opcode = fRT(insn) | 0200;
		break;

	default:
		break;
	}
	
	return GETBIT(Gvalid_opcodes, opcode) ? opcode : -1;
}

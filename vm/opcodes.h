/* 
 * File:    opcodes.h
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
 * MIPS I opcode list.
 */

#ifndef MIPS_OPCODES_H_
#define	MIPS_OPCODES_H_

/**
 * MIPS opcodes.  Note the octal constants!  The MSD decides about the group
 * to which the opcode belongs: 0=main opcode, 1=special, 2=regimm.
 */
enum mips_opcode {
	/* Main opcode field. */
	MIPS_I_SPECIAL = 0000,
	MIPS_I_REGIMM  = 0001,
	MIPS_I_J       = 0002,
	MIPS_I_JAL     = 0003,
	MIPS_I_BEQ     = 0004,
	MIPS_I_BNE     = 0005,
	MIPS_I_BLEZ    = 0006,
	MIPS_I_BGTZ    = 0007,
	
	MIPS_I_ADDI    = 0010,
	MIPS_I_ADDIU   = 0011,
	MIPS_I_SLTI    = 0012,
	MIPS_I_SLTIU   = 0013,
	MIPS_I_ANDI    = 0014,
	MIPS_I_ORI     = 0015,
	MIPS_I_XORI    = 0016,
	MIPS_I_LUI     = 0017,
	
	MIPS_I_LB      = 0040,
	MIPS_I_LH      = 0041,
	MIPS_I_LWL     = 0042,
	MIPS_I_LW      = 0043,
	MIPS_I_LBU     = 0044,
	MIPS_I_LHU     = 0045,
	MIPS_I_LWR     = 0046,

	MIPS_I_SB      = 0050,
	MIPS_I_SH      = 0051,
	MIPS_I_SWL     = 0052,
	MIPS_I_SW      = 0053,
	MIPS_I_SWR     = 0056,

	/* Function field for MIPS_I_SPECIAL. */
	MIPS_I_SLL     = 0100,
	MIPS_I_SRL     = 0102,
	MIPS_I_SRA     = 0103,
	MIPS_I_SLLV    = 0104,
	MIPS_I_SRLV    = 0106,
	MIPS_I_SRAV    = 0107,
	
	MIPS_I_JR      = 0110,
	MIPS_I_JALR    = 0111,
	MIPS_I_SYSCALL = 0114,
	MIPS_I_BREAK   = 0115,
	
	MIPS_I_MFHI    = 0120,
	MIPS_I_MTHI    = 0121,
	MIPS_I_MFLO    = 0122,
	MIPS_I_MTLO    = 0123,

	MIPS_I_MULT    = 0130,
	MIPS_I_MULTU   = 0131,
	MIPS_I_DIV     = 0132,
	MIPS_I_DIVU    = 0133,

	MIPS_I_ADD     = 0140,
	MIPS_I_ADDU    = 0141,
	MIPS_I_SUB     = 0142,
	MIPS_I_SUBU    = 0143,
	MIPS_I_AND     = 0144,
	MIPS_I_OR      = 0145,
	MIPS_I_XOR     = 0146,
	MIPS_I_NOR     = 0147,
	
	MIPS_I_SLT     = 0152,
	MIPS_I_SLTU    = 0153,
	
	/* RT field for MIPS_I_REGIMM */
	MIPS_I_BLTZ    = 0200,
	MIPS_I_BGEZ    = 0201,
	
	MIPS_I_BLTZAL  = 0220,
	MIPS_I_BGEZAL  = 0221
};

#endif	/* MIPS_OPCODES_H_ */


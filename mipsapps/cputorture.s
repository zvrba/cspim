#  ===========================================================================
#  COPYRIGHT (c) 2008 Zeljko Vrba <zvrba.external@zvrba.net>
# 
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#  ===========================================================================
#
# Torture test for every individual instruction.  Some instructions are
# expected to generate an exception, and such instructions are marked by
# a label of the form EXNxxxx (where xxxx is a number).  Such instructions
# (and the one following it) are silently skipped over by the controlling
# program.  If the instruction does NOT cause an exception, a BREAK happens.
# Test failures are reported through BREAKs with non-0 code.  Successful
# completion of all tests ends by BREAK with code 0.
#
# Break codes:
# 0 = everything OK
# 1 = $0 has been modified
# 2 = result was not as expected
# 3 = arithmetic/memory exception expected, but not taken
#
# This source file is to be preprocessed by M4 to generate the source which
# can be fed into assembler.
#
# Additional operations recognized by assembler:
# li   = load 32-bit immediate
# la   = load address
# nop  = no operation
# move = move between registers
#
# How to build and run:
#
# m4 cputorture.sm4 > cputorture.s
# mipsel-elf-gcc cputorture.s -nostdlib -Ttext 0x1000 -o cputorture
# ./dist/Debug/Sun-Generic/mipstorture cputorture 
#
# If everything goes OK, the execution should end with exception 5, code 0
# and PC near the label SUCCESS.
#

#
# Macros used from different sources.
#

#
# Expand R(N) to $N, e.g. R(1) -> $1
#

#
# Generate unique ID.
#



#
# Code to test ALU operations with immediate operand (including shifts by
# constant amount).
# Arguments: (1:insn, 2:dst, 3:src, 4:srcop, 5:imm, 6:result) 
# Temporary regs: $23, $24
#


#
# Code to test 3-operand ALU operations.
# Arguments: (1:insn, 2:dst, 3:src1, 4:src2, 5:op1, 6:op2, 7:result)
# Temporary regs: $23, $24, $25
#


		.text
		.set	noreorder
		.set	nobopt

		.globl _start
_start:
#  ===========================================================================
#  COPYRIGHT (c) 2008 Zeljko Vrba <zvrba.external@zvrba.net>
# 
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#  ===========================================================================
#
# Included as part of cputorture.sm4.  Tests arithmetic.
#
# Registers are cyclically used to test everything thoroughly.  $23-$25 should
# not be used by tests as macros already use them, $26-$27 should be avoided
# for compatibility with other simulators.
#

#
# Code to test {mult,div}{,u}.  The assembler macro-expands mul/div
# instructions, where it inserts extra checks (e.g. overflows).  We do NOT
# want that, and there is no way to disable this expansion in assembler, so
# the instruction is hard-coded as literal word.  Operands (op1, op2) are
# loaded into registers $2, $3 and this should be taken into account in
# instruction encoding too.
#
# Arguments: (1:insn, 2:op1, 3:op2, 4:result_hi, 5:result_lo)
# Temporary regs: $23-$25
#


#
# Code to test arithmetic overflows; immediate variant.  There is no expected
# result, as overflow is expected.
# Arguments: (1:insn, 2:src, 3:srcop, 4:imm, 5:label)
# Temporary regs: $23
#


#
# Code to test arithmetic overflows; 3-operand variant.  There is no expected
# result, as overflow is expected.
# Arguments: (1:insn, 2:src1, 3:src2, 4:srcop1, 5:srcop2, 6:label)
# Temporary regs: $23
#


		.text

		# 4 sign combinations for each of {add,sub,mult,div}{,u}.

		# Instructions with sign-extended immediate operands.

		
t_alui_addi_1:
		li		$3, 0x12345678			# 1st source operand
		li		$23, 0x12349679		# expected result
		move	$24, $3		# back up src
		li		$2, 0			# dst := 0
		addi		$0, $3, 0x4001	# try to change r0
		bne		$0, $2, B1	# taken if r0 or $2 modified
		addi		$2, $3, 0x4001		# dst := src (addi) imm
		bne		$2, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$3, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addi_2:
		li		$5, 0x12345678			# 1st source operand
		li		$23, 0x12345677		# expected result
		move	$24, $5		# back up src
		li		$4, 0			# dst := 0
		addi		$0, $5, 0xFFFF	# try to change r0
		bne		$0, $4, B1	# taken if r0 or $4 modified
		addi		$4, $5, 0xFFFF		# dst := src (addi) imm
		bne		$4, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$5, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addi_3:
		li		$7, 0xFFFFFFF0			# 1st source operand
		li		$23, 0xFFFFFFF1		# expected result
		move	$24, $7		# back up src
		li		$6, 0			# dst := 0
		addi		$0, $7, 0x0001	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		addi		$6, $7, 0x0001		# dst := src (addi) imm
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addi_4:
		li		$9, 0xFFFFFFF1			# 1st source operand
		li		$23, 0xFFFFFFF0		# expected result
		move	$24, $9		# back up src
		li		$8, 0			# dst := 0
		addi		$0, $9, 0xFFFF	# try to change r0
		bne		$0, $8, B1	# taken if r0 or $8 modified
		addi		$8, $9, 0xFFFF		# dst := src (addi) imm
		bne		$8, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$9, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		
t_alui_addiu_5:
		li		$11, 0x12345678			# 1st source operand
		li		$23, 0x12349679		# expected result
		move	$24, $11		# back up src
		li		$10, 0			# dst := 0
		addiu		$0, $11, 0x4001	# try to change r0
		bne		$0, $10, B1	# taken if r0 or $10 modified
		addiu		$10, $11, 0x4001		# dst := src (addiu) imm
		bne		$10, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$11, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addiu_6:
		li		$13, 0x12345678			# 1st source operand
		li		$23, 0x12345677		# expected result
		move	$24, $13		# back up src
		li		$12, 0			# dst := 0
		addiu		$0, $13, 0xFFFF	# try to change r0
		bne		$0, $12, B1	# taken if r0 or $12 modified
		addiu		$12, $13, 0xFFFF		# dst := src (addiu) imm
		bne		$12, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$13, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addiu_7:
		li		$15, 0xFFFFFFF0			# 1st source operand
		li		$23, 0xFFFFFFF1		# expected result
		move	$24, $15		# back up src
		li		$14, 0			# dst := 0
		addiu		$0, $15, 0x0001	# try to change r0
		bne		$0, $14, B1	# taken if r0 or $14 modified
		addiu		$14, $15, 0x0001		# dst := src (addiu) imm
		bne		$14, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$15, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addiu_8:
		li		$17, 0xFFFFFFF1			# 1st source operand
		li		$23, 0xFFFFFFF0		# expected result
		move	$24, $17		# back up src
		li		$16, 0			# dst := 0
		addiu		$0, $17, 0xFFFF	# try to change r0
		bne		$0, $16, B1	# taken if r0 or $16 modified
		addiu		$16, $17, 0xFFFF		# dst := src (addiu) imm
		bne		$16, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$17, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		# 3-register arithmetic instructions.

		
t_alu3_add_9:
		li		$7, 0x12345678			# 1st source operand
		li		$8, 0x600A9001			# 2nd source operand
		li		$23, 0x723EE679		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		add		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		add		$6, $7, $8		# dst := src1 (add) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_add_10:
		li		$7, 0x12345678			# 1st source operand
		li		$8, 0xFFFFFFFE			# 2nd source operand
		li		$23, 0x12345676		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		add		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		add		$6, $7, $8		# dst := src1 (add) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_add_11:
		li		$7, 0xFFFFFFF0			# 1st source operand
		li		$8, 0x00000001			# 2nd source operand
		li		$23, 0xFFFFFFF1		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		add		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		add		$6, $7, $8		# dst := src1 (add) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_add_12:
		li		$7, 0xFFFFFFF1			# 1st source operand
		li		$8, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0xFFFFFFF0		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		add		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		add		$6, $7, $8		# dst := src1 (add) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot


		
t_alu3_addu_13:
		li		$7, 0x12345678			# 1st source operand
		li		$8, 0x600A9001			# 2nd source operand
		li		$23, 0x723EE679		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		addu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		addu		$6, $7, $8		# dst := src1 (addu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_addu_14:
		li		$7, 0x12345678			# 1st source operand
		li		$8, 0xFFFFFFFE			# 2nd source operand
		li		$23, 0x12345676		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		addu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		addu		$6, $7, $8		# dst := src1 (addu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_addu_15:
		li		$7, 0xFFFFFFF0			# 1st source operand
		li		$8, 0x00000001			# 2nd source operand
		li		$23, 0xFFFFFFF1		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		addu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		addu		$6, $7, $8		# dst := src1 (addu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_addu_16:
		li		$7, 0xFFFFFFF1			# 1st source operand
		li		$8, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0xFFFFFFF0		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		addu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		addu		$6, $7, $8		# dst := src1 (addu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot


		
t_alu3_sub_17:
		li		$7, 0x723EE679			# 1st source operand
		li		$8, 0x600A9001			# 2nd source operand
		li		$23, 0x12345678		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		sub		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		sub		$6, $7, $8		# dst := src1 (sub) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sub_18:
		li		$7, 0x12345676			# 1st source operand
		li		$8, 0xFFFFFFFE			# 2nd source operand
		li		$23, 0x12345678		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		sub		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		sub		$6, $7, $8		# dst := src1 (sub) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sub_19:
		li		$7, 0xFFFFFFF1			# 1st source operand
		li		$8, 0x00000001			# 2nd source operand
		li		$23, 0xFFFFFFF0		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		sub		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		sub		$6, $7, $8		# dst := src1 (sub) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sub_20:
		li		$7, 0xFFFFFFF0			# 1st source operand
		li		$8, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0xFFFFFFF1		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		sub		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		sub		$6, $7, $8		# dst := src1 (sub) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot


		
t_alu3_subu_21:
		li		$7, 0x723EE679			# 1st source operand
		li		$8, 0x600A9001			# 2nd source operand
		li		$23, 0x12345678		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		subu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		subu		$6, $7, $8		# dst := src1 (subu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_subu_22:
		li		$7, 0x12345676			# 1st source operand
		li		$8, 0xFFFFFFFE			# 2nd source operand
		li		$23, 0x12345678		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		subu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		subu		$6, $7, $8		# dst := src1 (subu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_subu_23:
		li		$7, 0xFFFFFFF1			# 1st source operand
		li		$8, 0x00000001			# 2nd source operand
		li		$23, 0xFFFFFFF0		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		subu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		subu		$6, $7, $8		# dst := src1 (subu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_subu_24:
		li		$7, 0xFFFFFFF0			# 1st source operand
		li		$8, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0xFFFFFFF1		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		subu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		subu		$6, $7, $8		# dst := src1 (subu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot


		# Multiplication and division.  Due to assembler's expansion of these
		# instructions, we hard-code the instruction with $2,$3 as operands.

		# mult
		
t_muldiv_25:
		li		$2, 0x12345678		# 1st source operand
		li		$3, 0x600A9001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430018						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0x06D460B5		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0xE387D678		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_26:
		li		$2, 0x12345678		# 1st source operand
		li		$3, 0xFFFFFFFE		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430018						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0xFFFFFFFF		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0xDB975310		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_27:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0x600A9001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430018						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0xD0D26625		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0x449CF679		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_28:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0x80000001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430018						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0x3EE08CC3		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0x023EE679		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot


		# multu
		
t_muldiv_29:
		li		$2, 0x12345678		# 1st source operand
		li		$3, 0x600A9001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430019						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0x06D460B5		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0xE387D678		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_30:
		li		$2, 0x12345678		# 1st source operand
		li		$3, 0xFFFFFFFE		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430019						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0x12345677		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0xDB975310		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_31:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0x600A9001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430019						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0x30DCF626		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0x449CF679		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_32:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0x80000001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x00430019						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0x411F733D		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0x023EE679		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot


		# div. expected result is remainder,quotient
		
t_muldiv_33:
		li		$2, 0x600A9001		# 1st source operand
		li		$3, 0x02000021		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001A						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0x000A89D1		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0x00000030		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_34:
		li		$2, 0x600A9001		# 1st source operand
		li		$3, 0xFFFFFFE3		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001A						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0x00000001		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0xFCB03000		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_35:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0x200A9001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001A						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0xE25E967C 		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0xFFFFFFFD		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_36:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0xFFFFFFF7		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001A						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0xFFFFFFFF		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0x0DF902D6		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot


		# divu. expected result is remainder,quotient		
		
t_muldiv_37:
		li		$2, 0x600A9001		# 1st source operand
		li		$3, 0x02000021		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001B						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0x000A89D1		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0x00000030		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_38:
		li		$2, 0x600A9001		# 1st source operand
		li		$3, 0xFFFFFFE3		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001B						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0x600A9001		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0x00000000		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_39:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0x200A9001		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001B						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0x0214A675		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0x00000004		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot

		
t_muldiv_40:
		li		$2, 0x823EE679		# 1st source operand
		li		$3, 0xFFFFFFF7		# 2nd source operand
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		.word 0x0043001B						# pseudo-op to emit proper instruction
		bne		$2, $24, B2	# taken if src1 modified
		li		$24, 0x823EE679		# load result hi
		bne		$3, $25, B2	# taken if src2 modified
		li		$25, 0x00000000		# load result lo
		mfhi	$23			# get result hi; branch if not correct
		bne		$23, $24,  B2
		mflo	$23			# get result lo; branch if not correct
		bne		$23, $25, B2
		nop						# branch slot


		# Test that add/sub (do not) trigger overflow in right circumstances
		# and that unsigned variants give correct results.  Note that sub
		# with immediate argument does not exist.

		
t_alui_addi_41:
		li		$3, 0x7FFFFFFE			# 1st source operand
		li		$23, 0x7FFFFFFF		# expected result
		move	$24, $3		# back up src
		li		$2, 0			# dst := 0
		addi		$0, $3, 0x0001	# try to change r0
		bne		$0, $2, B1	# taken if r0 or $2 modified
		addi		$2, $3, 0x0001		# dst := src (addi) imm
		bne		$2, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$3, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_ovfi_addi_42:
		.globl	EXN01
		li		$6, 0x7FFFFFFF			# source operand
EXN01:		addi		$23, $6, 0x0001	# label used to mark expected exception
		break	3				# if not skipped by exn handler

		
t_alui_addiu_43:
		li		$7, 0x7FFFFFFF			# 1st source operand
		li		$23, 0x80000000		# expected result
		move	$24, $7		# back up src
		li		$6, 0			# dst := 0
		addiu		$0, $7, 0x0001	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		addiu		$6, $7, 0x0001		# dst := src (addiu) imm
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_addi_44:
		li		$5, 0x80000000			# 1st source operand
		li		$23, 0x80000001		# expected result
		move	$24, $5		# back up src
		li		$4, 0			# dst := 0
		addi		$0, $5, 0x0001	# try to change r0
		bne		$0, $4, B1	# taken if r0 or $4 modified
		addi		$4, $5, 0x0001		# dst := src (addi) imm
		bne		$4, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$5, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_ovfi_addi_45:
		.globl	EXN02
		li		$7, 0x80000000			# source operand
EXN02:		addi		$23, $7, 0xFFFF	# label used to mark expected exception
		break	3				# if not skipped by exn handler

		
t_alui_addiu_46:
		li		$9, 0x80000000			# 1st source operand
		li		$23, 0x7FFFFFFF		# expected result
		move	$24, $9		# back up src
		li		$8, 0			# dst := 0
		addiu		$0, $9, 0xFFFF	# try to change r0
		bne		$0, $8, B1	# taken if r0 or $8 modified
		addiu		$8, $9, 0xFFFF		# dst := src (addiu) imm
		bne		$8, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$9, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		
t_alu3_add_47:
		li		$11, 0x7FFFFFFE			# 1st source operand
		li		$12, 0x00000001			# 2nd source operand
		li		$23, 0x7FFFFFFF		# expected result
		move	$24, $11		# back up src1
		move	$25, $12		# back up src2
		li		$10, 0			# dst := 0
		add		$0, $11, $12	# try to change r0
		bne		$0, $10, B1	# taken if r0 or $10 modified
		add		$10, $11, $12		# dst := src1 (add) src2
		bne		$10, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$11, $24, B2	# src1 modified
		nop						# branch slot
		bne		$12, $25, B2	# operand2 modified
		nop						# branch slot

		
t_ovf3_add_48:
		.globl	EXN05
		li		$13, 0x7FFFFFFF			# 1st source operand
		li		$14, 0x00000001			# 2nd source operand
EXN05:		add		$23, $13, $14	# label used to mark expected exception
		break	3				# if not skipped by exn handler

		
t_alu3_addu_49:
		li		$14, 0x7FFFFFFF			# 1st source operand
		li		$15, 0x00000001			# 2nd source operand
		li		$23, 0x80000000		# expected result
		move	$24, $14		# back up src1
		move	$25, $15		# back up src2
		li		$13, 0			# dst := 0
		addu		$0, $14, $15	# try to change r0
		bne		$0, $13, B1	# taken if r0 or $13 modified
		addu		$13, $14, $15		# dst := src1 (addu) src2
		bne		$13, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$14, $24, B2	# src1 modified
		nop						# branch slot
		bne		$15, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_add_50:
		li		$17, 0x80000000			# 1st source operand
		li		$18, 0x00000001			# 2nd source operand
		li		$23, 0x80000001		# expected result
		move	$24, $17		# back up src1
		move	$25, $18		# back up src2
		li		$16, 0			# dst := 0
		add		$0, $17, $18	# try to change r0
		bne		$0, $16, B1	# taken if r0 or $16 modified
		add		$16, $17, $18		# dst := src1 (add) src2
		bne		$16, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$17, $24, B2	# src1 modified
		nop						# branch slot
		bne		$18, $25, B2	# operand2 modified
		nop						# branch slot

		
t_ovf3_add_51:
		.globl	EXN06
		li		$19, 0x80000000			# 1st source operand
		li		$20, 0xFFFFFFFF			# 2nd source operand
EXN06:		add		$23, $19, $20	# label used to mark expected exception
		break	3				# if not skipped by exn handler

		
t_alu3_addu_52:
		li		$28, 0x80000000			# 1st source operand
		li		$29, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0x7FFFFFFF		# expected result
		move	$24, $28		# back up src1
		move	$25, $29		# back up src2
		li		$22, 0			# dst := 0
		addu		$0, $28, $29	# try to change r0
		bne		$0, $22, B1	# taken if r0 or $22 modified
		addu		$22, $28, $29		# dst := src1 (addu) src2
		bne		$22, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$28, $24, B2	# src1 modified
		nop						# branch slot
		bne		$29, $25, B2	# operand2 modified
		nop						# branch slot


		
t_alu3_sub_53:
		li		$31, 0x7FFFFFFE			# 1st source operand
		li		$2, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0x7FFFFFFF		# expected result
		move	$24, $31		# back up src1
		move	$25, $2		# back up src2
		li		$30, 0			# dst := 0
		sub		$0, $31, $2	# try to change r0
		bne		$0, $30, B1	# taken if r0 or $30 modified
		sub		$30, $31, $2		# dst := src1 (sub) src2
		bne		$30, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$31, $24, B2	# src1 modified
		nop						# branch slot
		bne		$2, $25, B2	# operand2 modified
		nop						# branch slot

		
t_ovf3_sub_54:
		.globl	EXN07
		li		$3, 0x7FFFFFFF			# 1st source operand
		li		$4, 0xFFFFFFFF			# 2nd source operand
EXN07:		sub		$23, $3, $4	# label used to mark expected exception
		break	3				# if not skipped by exn handler

		
t_alu3_subu_55:
		li		$7, 0x7FFFFFFF			# 1st source operand
		li		$8, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0x80000000		# expected result
		move	$24, $7		# back up src1
		move	$25, $8		# back up src2
		li		$6, 0			# dst := 0
		subu		$0, $7, $8	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		subu		$6, $7, $8		# dst := src1 (subu) src2
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# src1 modified
		nop						# branch slot
		bne		$8, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sub_56:
		li		$10, 0xFFFFFFFF			# 1st source operand
		li		$11, 0x00000001			# 2nd source operand
		li		$23, 0xFFFFFFFE		# expected result
		move	$24, $10		# back up src1
		move	$25, $11		# back up src2
		li		$9, 0			# dst := 0
		sub		$0, $10, $11	# try to change r0
		bne		$0, $9, B1	# taken if r0 or $9 modified
		sub		$9, $10, $11		# dst := src1 (sub) src2
		bne		$9, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$10, $24, B2	# src1 modified
		nop						# branch slot
		bne		$11, $25, B2	# operand2 modified
		nop						# branch slot

		
t_ovf3_sub_57:
		.globl	EXN08
		li		$12, 0x80000000			# 1st source operand
		li		$13, 0x00000001			# 2nd source operand
EXN08:		sub		$23, $12, $13	# label used to mark expected exception
		break	3				# if not skipped by exn handler

		
t_alu3_subu_58:
		li		$16, 0x80000000			# 1st source operand
		li		$17, 0x00000001			# 2nd source operand
		li		$23, 0x7FFFFFFF		# expected result
		move	$24, $16		# back up src1
		move	$25, $17		# back up src2
		li		$15, 0			# dst := 0
		subu		$0, $16, $17	# try to change r0
		bne		$0, $15, B1	# taken if r0 or $15 modified
		subu		$15, $16, $17		# dst := src1 (subu) src2
		bne		$15, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$16, $24, B2	# src1 modified
		nop						# branch slot
		bne		$17, $25, B2	# operand2 modified
		nop						# branch slot


		# Explicit m{t,f}{hi,lo} tests.

t_hilo:
.globl	t_hilo
		xor		$18, $18, $18
		li		$30, 0x12345678
		mthi	$30
		li		$31, 0x9ABCDEF0
		mtlo	$31
		mfhi	$0					# check for $0 modif
		bne		$0, $18, B1
		mfhi	$2
		bne		$2, $30, B2			# branch if read hi != stored
		mflo	$0					# check for $0 modif
		bne		$0, $18, B1
		mflo	$2					# delay slot
		bne		$2, $31, B2			# branch if read lo != stored
		nop							# delay slot

			# arithmetic operations + m{f,t}{hi,lo}
#  ===========================================================================
#  COPYRIGHT (c) 2008 Zeljko Vrba <zvrba.external@zvrba.net>
# 
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#  ===========================================================================
#
# Included as part of cputorture.sm4.  Tests logic operations.
#
# Registers are cyclically used to test everything thoroughly.  $23-$25 should
# not be used by tests as macros already use them, $26-$27 should be avoided
# for compatibility with other simulators.
#

		.text

		# Bit operations with zero-extended immediate.

		
t_alui_andi_59:
		li		$22, 0x12345678			# 1st source operand
		li		$23, 0x00005678		# expected result
		move	$24, $22		# back up src
		li		$21, 0			# dst := 0
		andi		$0, $22, 0xFFFF	# try to change r0
		bne		$0, $21, B1	# taken if r0 or $21 modified
		andi		$21, $22, 0xFFFF		# dst := src (andi) imm
		bne		$21, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$22, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_ori_60:
		li		$29, 0x12345678			# 1st source operand
		li		$23, 0x1234F679		# expected result
		move	$24, $29		# back up src
		li		$28, 0			# dst := 0
		ori		$0, $29, 0xA001	# try to change r0
		bne		$0, $28, B1	# taken if r0 or $28 modified
		ori		$28, $29, 0xA001		# dst := src (ori) imm
		bne		$28, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$29, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_xori_61:
		li		$31, 0x12345678			# 1st source operand
		li		$23, 0x1234A987		# expected result
		move	$24, $31		# back up src
		li		$30, 0			# dst := 0
		xori		$0, $31, 0xFFFF	# try to change r0
		bne		$0, $30, B1	# taken if r0 or $30 modified
		xori		$30, $31, 0xFFFF		# dst := src (xori) imm
		bne		$30, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$31, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		# Bit operations on two registers.

		
t_alu3_and_62:
		li		$9, 0x12345678			# 1st source operand
		li		$10, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0x12345678		# expected result
		move	$24, $9		# back up src1
		move	$25, $10		# back up src2
		li		$8, 0			# dst := 0
		and		$0, $9, $10	# try to change r0
		bne		$0, $8, B1	# taken if r0 or $8 modified
		and		$8, $9, $10		# dst := src1 (and) src2
		bne		$8, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$9, $24, B2	# src1 modified
		nop						# branch slot
		bne		$10, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_or_63:
		li		$12, 0x12345678			# 1st source operand
		li		$13, 0xA001100A			# 2nd source operand
		li		$23, 0xB235567A		# expected result
		move	$24, $12		# back up src1
		move	$25, $13		# back up src2
		li		$11, 0			# dst := 0
		or		$0, $12, $13	# try to change r0
		bne		$0, $11, B1	# taken if r0 or $11 modified
		or		$11, $12, $13		# dst := src1 (or) src2
		bne		$11, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$12, $24, B2	# src1 modified
		nop						# branch slot
		bne		$13, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_xor_64:
		li		$15, 0x12345678			# 1st source operand
		li		$16, 0xFFFFFFFF			# 2nd source operand
		li		$23, 0xEDCBA987		# expected result
		move	$24, $15		# back up src1
		move	$25, $16		# back up src2
		li		$14, 0			# dst := 0
		xor		$0, $15, $16	# try to change r0
		bne		$0, $14, B1	# taken if r0 or $14 modified
		xor		$14, $15, $16		# dst := src1 (xor) src2
		bne		$14, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$15, $24, B2	# src1 modified
		nop						# branch slot
		bne		$16, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_nor_65:
		li		$18, 0x12345678			# 1st source operand
		li		$19, 0x11111111			# 2nd source operand
		li		$23, 0xECCAA886		# expected result
		move	$24, $18		# back up src1
		move	$25, $19		# back up src2
		li		$17, 0			# dst := 0
		nor		$0, $18, $19	# try to change r0
		bne		$0, $17, B1	# taken if r0 or $17 modified
		nor		$17, $18, $19		# dst := src1 (nor) src2
		bne		$17, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$18, $24, B2	# src1 modified
		nop						# branch slot
		bne		$19, $25, B2	# operand2 modified
		nop						# branch slot


		# Shifts with immediate count.

		
t_alui_sll_66:
		li		$3, 0x40000000			# 1st source operand
		li		$23, 0x80000000		# expected result
		move	$24, $3		# back up src
		li		$2, 0			# dst := 0
		sll		$0, $3, 0x01	# try to change r0
		bne		$0, $2, B1	# taken if r0 or $2 modified
		sll		$2, $3, 0x01		# dst := src (sll) imm
		bne		$2, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$3, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_sll_67:
		li		$5, 0x80000000			# 1st source operand
		li		$23, 0x00000000		# expected result
		move	$24, $5		# back up src
		li		$4, 0			# dst := 0
		sll		$0, $5, 0x01	# try to change r0
		bne		$0, $4, B1	# taken if r0 or $4 modified
		sll		$4, $5, 0x01		# dst := src (sll) imm
		bne		$4, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$5, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_sll_68:
		li		$7, 0x00000002			# 1st source operand
		li		$23, 0x00000020		# expected result
		move	$24, $7		# back up src
		li		$6, 0			# dst := 0
		sll		$0, $7, 0x04	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		sll		$6, $7, 0x04		# dst := src (sll) imm
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		
t_alui_srl_69:
		li		$9, 0x40000000			# 1st source operand
		li		$23, 0x04000000		# expected result
		move	$24, $9		# back up src
		li		$8, 0			# dst := 0
		srl		$0, $9, 0x04	# try to change r0
		bne		$0, $8, B1	# taken if r0 or $8 modified
		srl		$8, $9, 0x04		# dst := src (srl) imm
		bne		$8, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$9, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_srl_70:
		li		$11, 0x00000002			# 1st source operand
		li		$23, 0x00000001		# expected result
		move	$24, $11		# back up src
		li		$10, 0			# dst := 0
		srl		$0, $11, 0x01	# try to change r0
		bne		$0, $10, B1	# taken if r0 or $10 modified
		srl		$10, $11, 0x01		# dst := src (srl) imm
		bne		$10, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$11, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_srl_71:
		li		$13, 0x00000001			# 1st source operand
		li		$23, 0x00000000		# expected result
		move	$24, $13		# back up src
		li		$12, 0			# dst := 0
		srl		$0, $13, 0x01	# try to change r0
		bne		$0, $12, B1	# taken if r0 or $12 modified
		srl		$12, $13, 0x01		# dst := src (srl) imm
		bne		$12, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$13, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		
t_alui_sra_72:
		li		$15, 0x40000000			# 1st source operand
		li		$23, 0x04000000		# expected result
		move	$24, $15		# back up src
		li		$14, 0			# dst := 0
		sra		$0, $15, 0x04	# try to change r0
		bne		$0, $14, B1	# taken if r0 or $14 modified
		sra		$14, $15, 0x04		# dst := src (sra) imm
		bne		$14, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$15, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_sra_73:
		li		$17, 0x80000000			# 1st source operand
		li		$23, 0xC0000000		# expected result
		move	$24, $17		# back up src
		li		$16, 0			# dst := 0
		sra		$0, $17, 0x01	# try to change r0
		bne		$0, $16, B1	# taken if r0 or $16 modified
		sra		$16, $17, 0x01		# dst := src (sra) imm
		bne		$16, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$17, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot

		
t_alui_sra_74:
		li		$19, 0xF0000000			# 1st source operand
		li		$23, 0xF8000000		# expected result
		move	$24, $19		# back up src
		li		$18, 0			# dst := 0
		sra		$0, $19, 0x01	# try to change r0
		bne		$0, $18, B1	# taken if r0 or $18 modified
		sra		$18, $19, 0x01		# dst := src (sra) imm
		bne		$18, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$19, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot


		# Shifts with variable count (in register).

		
t_alu3_sllv_75:
		li		$21, 0x40000000			# 1st source operand
		li		$22, 0x01			# 2nd source operand
		li		$23, 0x80000000		# expected result
		move	$24, $21		# back up src1
		move	$25, $22		# back up src2
		li		$20, 0			# dst := 0
		sllv		$0, $21, $22	# try to change r0
		bne		$0, $20, B1	# taken if r0 or $20 modified
		sllv		$20, $21, $22		# dst := src1 (sllv) src2
		bne		$20, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$21, $24, B2	# src1 modified
		nop						# branch slot
		bne		$22, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sllv_76:
		li		$29, 0x80000000			# 1st source operand
		li		$30, 0x01			# 2nd source operand
		li		$23, 0x00000000		# expected result
		move	$24, $29		# back up src1
		move	$25, $30		# back up src2
		li		$28, 0			# dst := 0
		sllv		$0, $29, $30	# try to change r0
		bne		$0, $28, B1	# taken if r0 or $28 modified
		sllv		$28, $29, $30		# dst := src1 (sllv) src2
		bne		$28, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$29, $24, B2	# src1 modified
		nop						# branch slot
		bne		$30, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sllv_77:
		li		$2, 0x00000002			# 1st source operand
		li		$3, 0x04			# 2nd source operand
		li		$23, 0x00000020		# expected result
		move	$24, $2		# back up src1
		move	$25, $3		# back up src2
		li		$31, 0			# dst := 0
		sllv		$0, $2, $3	# try to change r0
		bne		$0, $31, B1	# taken if r0 or $31 modified
		sllv		$31, $2, $3		# dst := src1 (sllv) src2
		bne		$31, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$2, $24, B2	# src1 modified
		nop						# branch slot
		bne		$3, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_sllv_78:
		li		$5, 0x00000002			# 1st source operand
		li		$6, 0xC4			# 2nd source operand
		li		$23, 0x00000020		# expected result
		move	$24, $5		# back up src1
		move	$25, $6		# back up src2
		li		$4, 0			# dst := 0
		sllv		$0, $5, $6	# try to change r0
		bne		$0, $4, B1	# taken if r0 or $4 modified
		sllv		$4, $5, $6		# dst := src1 (sllv) src2
		bne		$4, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$5, $24, B2	# src1 modified
		nop						# branch slot
		bne		$6, $25, B2	# operand2 modified
		nop						# branch slot


		
t_alu3_srlv_79:
		li		$8, 0x40000000			# 1st source operand
		li		$9, 0xC4			# 2nd source operand
		li		$23, 0x04000000		# expected result
		move	$24, $8		# back up src1
		move	$25, $9		# back up src2
		li		$7, 0			# dst := 0
		srlv		$0, $8, $9	# try to change r0
		bne		$0, $7, B1	# taken if r0 or $7 modified
		srlv		$7, $8, $9		# dst := src1 (srlv) src2
		bne		$7, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$8, $24, B2	# src1 modified
		nop						# branch slot
		bne		$9, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_srlv_80:
		li		$11, 0x40000000			# 1st source operand
		li		$12, 0x04			# 2nd source operand
		li		$23, 0x04000000		# expected result
		move	$24, $11		# back up src1
		move	$25, $12		# back up src2
		li		$10, 0			# dst := 0
		srlv		$0, $11, $12	# try to change r0
		bne		$0, $10, B1	# taken if r0 or $10 modified
		srlv		$10, $11, $12		# dst := src1 (srlv) src2
		bne		$10, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$11, $24, B2	# src1 modified
		nop						# branch slot
		bne		$12, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_srlv_81:
		li		$14, 0x00000002			# 1st source operand
		li		$15, 0x01			# 2nd source operand
		li		$23, 0x00000001		# expected result
		move	$24, $14		# back up src1
		move	$25, $15		# back up src2
		li		$13, 0			# dst := 0
		srlv		$0, $14, $15	# try to change r0
		bne		$0, $13, B1	# taken if r0 or $13 modified
		srlv		$13, $14, $15		# dst := src1 (srlv) src2
		bne		$13, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$14, $24, B2	# src1 modified
		nop						# branch slot
		bne		$15, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_srlv_82:
		li		$17, 0x00000001			# 1st source operand
		li		$18, 0x01			# 2nd source operand
		li		$23, 0x00000000		# expected result
		move	$24, $17		# back up src1
		move	$25, $18		# back up src2
		li		$16, 0			# dst := 0
		srlv		$0, $17, $18	# try to change r0
		bne		$0, $16, B1	# taken if r0 or $16 modified
		srlv		$16, $17, $18		# dst := src1 (srlv) src2
		bne		$16, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$17, $24, B2	# src1 modified
		nop						# branch slot
		bne		$18, $25, B2	# operand2 modified
		nop						# branch slot


		
t_alu3_srav_83:
		li		$20, 0x40000000			# 1st source operand
		li		$21, 0xC4			# 2nd source operand
		li		$23, 0x04000000		# expected result
		move	$24, $20		# back up src1
		move	$25, $21		# back up src2
		li		$19, 0			# dst := 0
		srav		$0, $20, $21	# try to change r0
		bne		$0, $19, B1	# taken if r0 or $19 modified
		srav		$19, $20, $21		# dst := src1 (srav) src2
		bne		$19, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$20, $24, B2	# src1 modified
		nop						# branch slot
		bne		$21, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_srav_84:
		li		$28, 0x40000000			# 1st source operand
		li		$29, 0x04			# 2nd source operand
		li		$23, 0x04000000		# expected result
		move	$24, $28		# back up src1
		move	$25, $29		# back up src2
		li		$22, 0			# dst := 0
		srav		$0, $28, $29	# try to change r0
		bne		$0, $22, B1	# taken if r0 or $22 modified
		srav		$22, $28, $29		# dst := src1 (srav) src2
		bne		$22, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$28, $24, B2	# src1 modified
		nop						# branch slot
		bne		$29, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_srav_85:
		li		$31, 0x80000000			# 1st source operand
		li		$2, 0x01			# 2nd source operand
		li		$23, 0xC0000000		# expected result
		move	$24, $31		# back up src1
		move	$25, $2		# back up src2
		li		$30, 0			# dst := 0
		srav		$0, $31, $2	# try to change r0
		bne		$0, $30, B1	# taken if r0 or $30 modified
		srav		$30, $31, $2		# dst := src1 (srav) src2
		bne		$30, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$31, $24, B2	# src1 modified
		nop						# branch slot
		bne		$2, $25, B2	# operand2 modified
		nop						# branch slot

		
t_alu3_srav_86:
		li		$4, 0xF0000000			# 1st source operand
		li		$5, 0x01			# 2nd source operand
		li		$23, 0xF8000000		# expected result
		move	$24, $4		# back up src1
		move	$25, $5		# back up src2
		li		$3, 0			# dst := 0
		srav		$0, $4, $5	# try to change r0
		bne		$0, $3, B1	# taken if r0 or $3 modified
		srav		$3, $4, $5		# dst := src1 (srav) src2
		bne		$3, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$4, $24, B2	# src1 modified
		nop						# branch slot
		bne		$5, $25, B2	# operand2 modified
		nop						# branch slot


		# Comparison predicates with sign-extended immediate operand.
		
		
t_alui_slti_87:
		li		$7, 0x12345678			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $7		# back up src
		li		$6, 0			# dst := 0
		slti		$0, $7, 0x1234	# try to change r0
		bne		$0, $6, B1	# taken if r0 or $6 modified
		slti		$6, $7, 0x1234		# dst := src (slti) imm
		bne		$6, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$7, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/+, >
		
t_alui_slti_88:
		li		$9, 0x00001234			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $9		# back up src
		li		$8, 0			# dst := 0
		slti		$0, $9, 0x1234	# try to change r0
		bne		$0, $8, B1	# taken if r0 or $8 modified
		slti		$8, $9, 0x1234		# dst := src (slti) imm
		bne		$8, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$9, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/+, =
		
t_alui_slti_89:
		li		$11, 0x00001234			# 1st source operand
		li		$23, 1		# expected result
		move	$24, $11		# back up src
		li		$10, 0			# dst := 0
		slti		$0, $11, 0x5678	# try to change r0
		bne		$0, $10, B1	# taken if r0 or $10 modified
		slti		$10, $11, 0x5678		# dst := src (slti) imm
		bne		$10, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$11, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/+, <
		
t_alui_slti_90:
		li		$13, 0x12345678			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $13		# back up src
		li		$12, 0			# dst := 0
		slti		$0, $13, 0x8000	# try to change r0
		bne		$0, $12, B1	# taken if r0 or $12 modified
		slti		$12, $13, 0x8000		# dst := src (slti) imm
		bne		$12, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$13, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/-
		
t_alui_slti_91:
		li		$15, 0xFFFFFFFF			# 1st source operand
		li		$23, 1		# expected result
		move	$24, $15		# back up src
		li		$14, 0			# dst := 0
		slti		$0, $15, 0x0001	# try to change r0
		bne		$0, $14, B1	# taken if r0 or $14 modified
		slti		$14, $15, 0x0001		# dst := src (slti) imm
		bne		$14, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$15, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/+
		
t_alui_slti_92:
		li		$17, 0xFFFFFFFF			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $17		# back up src
		li		$16, 0			# dst := 0
		slti		$0, $17, 0x8000	# try to change r0
		bne		$0, $16, B1	# taken if r0 or $16 modified
		slti		$16, $17, 0x8000		# dst := src (slti) imm
		bne		$16, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$17, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/-, >
		
t_alui_slti_93:
		li		$19, 0xFFFFFFFF			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $19		# back up src
		li		$18, 0			# dst := 0
		slti		$0, $19, 0xFFFF	# try to change r0
		bne		$0, $18, B1	# taken if r0 or $18 modified
		slti		$18, $19, 0xFFFF		# dst := src (slti) imm
		bne		$18, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$19, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/-, =
		
t_alui_slti_94:
		li		$21, 0x80000000			# 1st source operand
		li		$23, 1		# expected result
		move	$24, $21		# back up src
		li		$20, 0			# dst := 0
		slti		$0, $21, 0xFFFF	# try to change r0
		bne		$0, $20, B1	# taken if r0 or $20 modified
		slti		$20, $21, 0xFFFF		# dst := src (slti) imm
		bne		$20, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$21, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/-, <

		
t_alui_sltiu_95:
		li		$28, 0x12345678			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $28		# back up src
		li		$22, 0			# dst := 0
		sltiu		$0, $28, 0x1234	# try to change r0
		bne		$0, $22, B1	# taken if r0 or $22 modified
		sltiu		$22, $28, 0x1234		# dst := src (sltiu) imm
		bne		$22, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$28, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/+, >
		
t_alui_sltiu_96:
		li		$30, 0x00001234			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $30		# back up src
		li		$29, 0			# dst := 0
		sltiu		$0, $30, 0x1234	# try to change r0
		bne		$0, $29, B1	# taken if r0 or $29 modified
		sltiu		$29, $30, 0x1234		# dst := src (sltiu) imm
		bne		$29, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$30, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/+, =
		
t_alui_sltiu_97:
		li		$2, 0x00001234			# 1st source operand
		li		$23, 1		# expected result
		move	$24, $2		# back up src
		li		$31, 0			# dst := 0
		sltiu		$0, $2, 0x5678	# try to change r0
		bne		$0, $31, B1	# taken if r0 or $31 modified
		sltiu		$31, $2, 0x5678		# dst := src (sltiu) imm
		bne		$31, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$2, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/+, <
		
t_alui_sltiu_98:
		li		$4, 0x12345678			# 1st source operand
		li		$23, 1		# expected result
		move	$24, $4		# back up src
		li		$3, 0			# dst := 0
		sltiu		$0, $4, 0x8000	# try to change r0
		bne		$0, $3, B1	# taken if r0 or $3 modified
		sltiu		$3, $4, 0x8000		# dst := src (sltiu) imm
		bne		$3, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$4, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# +/-
		
t_alui_sltiu_99:
		li		$6, 0xFFFFFFFF			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $6		# back up src
		li		$5, 0			# dst := 0
		sltiu		$0, $6, 0x0001	# try to change r0
		bne		$0, $5, B1	# taken if r0 or $5 modified
		sltiu		$5, $6, 0x0001		# dst := src (sltiu) imm
		bne		$5, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$6, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/+
		
t_alui_sltiu_100:
		li		$8, 0xFFFFFFFF			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $8		# back up src
		li		$7, 0			# dst := 0
		sltiu		$0, $8, 0x8000	# try to change r0
		bne		$0, $7, B1	# taken if r0 or $7 modified
		sltiu		$7, $8, 0x8000		# dst := src (sltiu) imm
		bne		$7, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$8, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/-, >
		
t_alui_sltiu_101:
		li		$10, 0xFFFFFFFF			# 1st source operand
		li		$23, 0		# expected result
		move	$24, $10		# back up src
		li		$9, 0			# dst := 0
		sltiu		$0, $10, 0xFFFF	# try to change r0
		bne		$0, $9, B1	# taken if r0 or $9 modified
		sltiu		$9, $10, 0xFFFF		# dst := src (sltiu) imm
		bne		$9, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$10, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/-, =
		
t_alui_sltiu_102:
		li		$12, 0x80000000			# 1st source operand
		li		$23, 1		# expected result
		move	$24, $12		# back up src
		li		$11, 0			# dst := 0
		sltiu		$0, $12, 0xFFFF	# try to change r0
		bne		$0, $11, B1	# taken if r0 or $11 modified
		sltiu		$11, $12, 0xFFFF		# dst := src (sltiu) imm
		bne		$11, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$12, $24, B2	# source or backup reg spuriously modified
		nop						# branch slot
			# -/-, <

		# 3-registed comparison predicates.

		
t_alu3_slt_103:
		li		$5, 0x12345678			# 1st source operand
		li		$6, 0x02345678			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $5		# back up src1
		move	$25, $6		# back up src2
		li		$4, 0			# dst := 0
		slt		$0, $5, $6	# try to change r0
		bne		$0, $4, B1	# taken if r0 or $4 modified
		slt		$4, $5, $6		# dst := src1 (slt) src2
		bne		$4, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$5, $24, B2	# src1 modified
		nop						# branch slot
		bne		$6, $25, B2	# operand2 modified
		nop						# branch slot
	# +/+, >
		
t_alu3_slt_104:
		li		$8, 0x12345678			# 1st source operand
		li		$9, 0x12345678			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $8		# back up src1
		move	$25, $9		# back up src2
		li		$7, 0			# dst := 0
		slt		$0, $8, $9	# try to change r0
		bne		$0, $7, B1	# taken if r0 or $7 modified
		slt		$7, $8, $9		# dst := src1 (slt) src2
		bne		$7, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$8, $24, B2	# src1 modified
		nop						# branch slot
		bne		$9, $25, B2	# operand2 modified
		nop						# branch slot
	# +/+, =
		
t_alu3_slt_105:
		li		$10, 0x12345678			# 1st source operand
		li		$11, 0x23456789			# 2nd source operand
		li		$23, 1		# expected result
		move	$24, $10		# back up src1
		move	$25, $11		# back up src2
		li		$9, 0			# dst := 0
		slt		$0, $10, $11	# try to change r0
		bne		$0, $9, B1	# taken if r0 or $9 modified
		slt		$9, $10, $11		# dst := src1 (slt) src2
		bne		$9, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$10, $24, B2	# src1 modified
		nop						# branch slot
		bne		$11, $25, B2	# operand2 modified
		nop						# branch slot
	# +/+, <
		
t_alu3_slt_106:
		li		$13, 0x12345678			# 1st source operand
		li		$14, 0x80000000			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $13		# back up src1
		move	$25, $14		# back up src2
		li		$12, 0			# dst := 0
		slt		$0, $13, $14	# try to change r0
		bne		$0, $12, B1	# taken if r0 or $12 modified
		slt		$12, $13, $14		# dst := src1 (slt) src2
		bne		$12, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$13, $24, B2	# src1 modified
		nop						# branch slot
		bne		$14, $25, B2	# operand2 modified
		nop						# branch slot
	# +/-
		
t_alu3_slt_107:
		li		$16, 0x80000000			# 1st source operand
		li		$17, 0x12345678			# 2nd source operand
		li		$23, 1		# expected result
		move	$24, $16		# back up src1
		move	$25, $17		# back up src2
		li		$15, 0			# dst := 0
		slt		$0, $16, $17	# try to change r0
		bne		$0, $15, B1	# taken if r0 or $15 modified
		slt		$15, $16, $17		# dst := src1 (slt) src2
		bne		$15, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$16, $24, B2	# src1 modified
		nop						# branch slot
		bne		$17, $25, B2	# operand2 modified
		nop						# branch slot
	# -/+
		
t_alu3_slt_108:
		li		$18, 0xFFFFFFFF			# 1st source operand
		li		$19, 0x80000000			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $18		# back up src1
		move	$25, $19		# back up src2
		li		$17, 0			# dst := 0
		slt		$0, $18, $19	# try to change r0
		bne		$0, $17, B1	# taken if r0 or $17 modified
		slt		$17, $18, $19		# dst := src1 (slt) src2
		bne		$17, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$18, $24, B2	# src1 modified
		nop						# branch slot
		bne		$19, $25, B2	# operand2 modified
		nop						# branch slot
	# -/-, >
		
t_alu3_slt_109:
		li		$21, 0x90000000			# 1st source operand
		li		$22, 0x90000000			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $21		# back up src1
		move	$25, $22		# back up src2
		li		$20, 0			# dst := 0
		slt		$0, $21, $22	# try to change r0
		bne		$0, $20, B1	# taken if r0 or $20 modified
		slt		$20, $21, $22		# dst := src1 (slt) src2
		bne		$20, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$21, $24, B2	# src1 modified
		nop						# branch slot
		bne		$22, $25, B2	# operand2 modified
		nop						# branch slot
	# -/-, =
		
t_alu3_slt_110:
		li		$29, 0x90000000			# 1st source operand
		li		$30, 0xF0000000			# 2nd source operand
		li		$23, 1		# expected result
		move	$24, $29		# back up src1
		move	$25, $30		# back up src2
		li		$28, 0			# dst := 0
		slt		$0, $29, $30	# try to change r0
		bne		$0, $28, B1	# taken if r0 or $28 modified
		slt		$28, $29, $30		# dst := src1 (slt) src2
		bne		$28, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$29, $24, B2	# src1 modified
		nop						# branch slot
		bne		$30, $25, B2	# operand2 modified
		nop						# branch slot
	# -/-, <

		
t_alu3_sltu_111:
		li		$5, 0x12345678			# 1st source operand
		li		$6, 0x02345678			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $5		# back up src1
		move	$25, $6		# back up src2
		li		$4, 0			# dst := 0
		sltu		$0, $5, $6	# try to change r0
		bne		$0, $4, B1	# taken if r0 or $4 modified
		sltu		$4, $5, $6		# dst := src1 (sltu) src2
		bne		$4, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$5, $24, B2	# src1 modified
		nop						# branch slot
		bne		$6, $25, B2	# operand2 modified
		nop						# branch slot
	# +/+, >
		
t_alu3_sltu_112:
		li		$8, 0x12345678			# 1st source operand
		li		$9, 0x12345678			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $8		# back up src1
		move	$25, $9		# back up src2
		li		$7, 0			# dst := 0
		sltu		$0, $8, $9	# try to change r0
		bne		$0, $7, B1	# taken if r0 or $7 modified
		sltu		$7, $8, $9		# dst := src1 (sltu) src2
		bne		$7, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$8, $24, B2	# src1 modified
		nop						# branch slot
		bne		$9, $25, B2	# operand2 modified
		nop						# branch slot
	# +/+, =
		
t_alu3_sltu_113:
		li		$10, 0x12345678			# 1st source operand
		li		$11, 0x23456789			# 2nd source operand
		li		$23, 1		# expected result
		move	$24, $10		# back up src1
		move	$25, $11		# back up src2
		li		$9, 0			# dst := 0
		sltu		$0, $10, $11	# try to change r0
		bne		$0, $9, B1	# taken if r0 or $9 modified
		sltu		$9, $10, $11		# dst := src1 (sltu) src2
		bne		$9, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$10, $24, B2	# src1 modified
		nop						# branch slot
		bne		$11, $25, B2	# operand2 modified
		nop						# branch slot
	# +/+, <
		
t_alu3_sltu_114:
		li		$13, 0x12345678			# 1st source operand
		li		$14, 0x80000000			# 2nd source operand
		li		$23, 1		# expected result
		move	$24, $13		# back up src1
		move	$25, $14		# back up src2
		li		$12, 0			# dst := 0
		sltu		$0, $13, $14	# try to change r0
		bne		$0, $12, B1	# taken if r0 or $12 modified
		sltu		$12, $13, $14		# dst := src1 (sltu) src2
		bne		$12, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$13, $24, B2	# src1 modified
		nop						# branch slot
		bne		$14, $25, B2	# operand2 modified
		nop						# branch slot
	# +/-
		
t_alu3_sltu_115:
		li		$16, 0x80000000			# 1st source operand
		li		$17, 0x12345678			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $16		# back up src1
		move	$25, $17		# back up src2
		li		$15, 0			# dst := 0
		sltu		$0, $16, $17	# try to change r0
		bne		$0, $15, B1	# taken if r0 or $15 modified
		sltu		$15, $16, $17		# dst := src1 (sltu) src2
		bne		$15, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$16, $24, B2	# src1 modified
		nop						# branch slot
		bne		$17, $25, B2	# operand2 modified
		nop						# branch slot
	# -/+
		
t_alu3_sltu_116:
		li		$18, 0xFFFFFFFF			# 1st source operand
		li		$19, 0x80000000			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $18		# back up src1
		move	$25, $19		# back up src2
		li		$17, 0			# dst := 0
		sltu		$0, $18, $19	# try to change r0
		bne		$0, $17, B1	# taken if r0 or $17 modified
		sltu		$17, $18, $19		# dst := src1 (sltu) src2
		bne		$17, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$18, $24, B2	# src1 modified
		nop						# branch slot
		bne		$19, $25, B2	# operand2 modified
		nop						# branch slot
	# -/-, >
		
t_alu3_sltu_117:
		li		$21, 0x90000000			# 1st source operand
		li		$22, 0x90000000			# 2nd source operand
		li		$23, 0		# expected result
		move	$24, $21		# back up src1
		move	$25, $22		# back up src2
		li		$20, 0			# dst := 0
		sltu		$0, $21, $22	# try to change r0
		bne		$0, $20, B1	# taken if r0 or $20 modified
		sltu		$20, $21, $22		# dst := src1 (sltu) src2
		bne		$20, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$21, $24, B2	# src1 modified
		nop						# branch slot
		bne		$22, $25, B2	# operand2 modified
		nop						# branch slot
	# -/-, =
		
t_alu3_sltu_118:
		li		$29, 0x90000000			# 1st source operand
		li		$30, 0xF0000000			# 2nd source operand
		li		$23, 1		# expected result
		move	$24, $29		# back up src1
		move	$25, $30		# back up src2
		li		$28, 0			# dst := 0
		sltu		$0, $29, $30	# try to change r0
		bne		$0, $28, B1	# taken if r0 or $28 modified
		sltu		$28, $29, $30		# dst := src1 (sltu) src2
		bne		$28, $23, B2	# taken if incorrect result
		nop						# branch slot
		bne		$29, $24, B2	# src1 modified
		nop						# branch slot
		bne		$30, $25, B2	# operand2 modified
		nop						# branch slot
	# -/-, <
			# bit operations and comparisons
#  ===========================================================================
#  COPYRIGHT (c) 2008 Zeljko Vrba <zvrba.external@zvrba.net>
# 
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#  ===========================================================================
#
# Included as part of cputorture.sm4.  Tests load/store instructions for
# little-endian mode.
#

t_lui:
.globl t_lui
		li		$2, 1			# Test that lui zeroes low-order bits
		lui		$2, 0
		bne		$2, $0, B2

		# Constants for further tests.

		lui		$23, 0
		la		$28, w1
		la		$29, w2
		la		$30, w3
		la		$31, w4

		# Just test that it's possible to modify $26, $27

		la		$26, w1
		bne		$26, $28, B2
		la		$27, w2
		bne		$27, $29, B2

#
# Test load instructions.
# Arguments: (1:insn, 2:r1, 3:r2, 4:address, 5:expected)
# Temporary registers: $24, $25
#


#
# Test that unaligned loads raise exception.
# Arguments: (1:insn, 2:reg, 3:address, 4:label)
#


#
# Test that unaligned stores raise exception.
# Arguments: (1:insn, 2:address, 3:label)
#


		
t_ld_lb_119:
		lb		$2, 0($28)				# load from memory
		li		$3, 0x00000078				# load expected as immediate
		bne		$2, $3, B2			# branch if not equal
		lb		$0, 0($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_120:
		lb		$4, 1($28)				# load from memory
		li		$5, 0x00000056				# load expected as immediate
		bne		$4, $5, B2			# branch if not equal
		lb		$0, 1($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_121:
		lb		$6, 2($28)				# load from memory
		li		$7, 0x00000034				# load expected as immediate
		bne		$6, $7, B2			# branch if not equal
		lb		$0, 2($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_122:
		lb		$8, 3($28)				# load from memory
		li		$9, 0x00000012				# load expected as immediate
		bne		$8, $9, B2			# branch if not equal
		lb		$0, 3($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_123:
		lb		$10, 0($29)				# load from memory
		li		$11, 0xFFFFFFF8				# load expected as immediate
		bne		$10, $11, B2			# branch if not equal
		lb		$0, 0($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_124:
		lb		$12, 1($29)				# load from memory
		li		$13, 0xFFFFFFE7				# load expected as immediate
		bne		$12, $13, B2			# branch if not equal
		lb		$0, 1($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_125:
		lb		$14, 2($29)				# load from memory
		li		$15, 0xFFFFFFD6				# load expected as immediate
		bne		$14, $15, B2			# branch if not equal
		lb		$0, 2($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lb_126:
		lb		$16, 3($29)				# load from memory
		li		$17, 0xFFFFFFC5				# load expected as immediate
		bne		$16, $17, B2			# branch if not equal
		lb		$0, 3($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		
t_ld_lbu_127:
		lbu		$2, 0($28)				# load from memory
		li		$3, 0x00000078				# load expected as immediate
		bne		$2, $3, B2			# branch if not equal
		lbu		$0, 0($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_128:
		lbu		$4, 1($28)				# load from memory
		li		$5, 0x00000056				# load expected as immediate
		bne		$4, $5, B2			# branch if not equal
		lbu		$0, 1($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_129:
		lbu		$6, 2($28)				# load from memory
		li		$7, 0x00000034				# load expected as immediate
		bne		$6, $7, B2			# branch if not equal
		lbu		$0, 2($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_130:
		lbu		$8, 3($28)				# load from memory
		li		$9, 0x00000012				# load expected as immediate
		bne		$8, $9, B2			# branch if not equal
		lbu		$0, 3($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_131:
		lbu		$10, 0($29)				# load from memory
		li		$11, 0x000000F8				# load expected as immediate
		bne		$10, $11, B2			# branch if not equal
		lbu		$0, 0($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_132:
		lbu		$12, 1($29)				# load from memory
		li		$13, 0x000000E7				# load expected as immediate
		bne		$12, $13, B2			# branch if not equal
		lbu		$0, 1($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_133:
		lbu		$14, 2($29)				# load from memory
		li		$15, 0x000000D6				# load expected as immediate
		bne		$14, $15, B2			# branch if not equal
		lbu		$0, 2($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lbu_134:
		lbu		$16, 3($29)				# load from memory
		li		$17, 0x000000C5				# load expected as immediate
		bne		$16, $17, B2			# branch if not equal
		lbu		$0, 3($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		
t_ld_lh_135:
		lh		$18, 0($28)				# load from memory
		li		$19, 0x00005678				# load expected as immediate
		bne		$18, $19, B2			# branch if not equal
		lh		$0, 0($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lh_136:
		lh		$20, 2($28)				# load from memory
		li		$21, 0x00001234				# load expected as immediate
		bne		$20, $21, B2			# branch if not equal
		lh		$0, 2($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lh_137:
		lh		$22, 0($29)				# load from memory
		li		$2, 0xFFFFE7F8				# load expected as immediate
		bne		$22, $2, B2			# branch if not equal
		lh		$0, 0($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lh_138:
		lh		$3, 2($29)				# load from memory
		li		$4, 0xFFFFC5D6				# load expected as immediate
		bne		$3, $4, B2			# branch if not equal
		lh		$0, 2($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		
t_ld_lhu_139:
		lhu		$5, 0($28)				# load from memory
		li		$6, 0x00005678				# load expected as immediate
		bne		$5, $6, B2			# branch if not equal
		lhu		$0, 0($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lhu_140:
		lhu		$7, 2($28)				# load from memory
		li		$8, 0x00001234				# load expected as immediate
		bne		$7, $8, B2			# branch if not equal
		lhu		$0, 2($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lhu_141:
		lhu		$9, 0($29)				# load from memory
		li		$10, 0x0000E7F8				# load expected as immediate
		bne		$9, $10, B2			# branch if not equal
		lhu		$0, 0($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lhu_142:
		lhu		$11, 2($29)				# load from memory
		li		$12, 0x0000C5D6				# load expected as immediate
		bne		$11, $12, B2			# branch if not equal
		lhu		$0, 2($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		
t_ld_lw_143:
		lw		$13, 0($28)				# load from memory
		li		$14, 0x12345678				# load expected as immediate
		bne		$13, $14, B2			# branch if not equal
		lw		$0, 0($28)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lw_144:
		lw		$15, 0($29)				# load from memory
		li		$16, 0xC5D6E7F8				# load expected as immediate
		bne		$15, $16, B2			# branch if not equal
		lw		$0, 0($29)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		# Exercise store instructions to recheck with loads
	
t_store:
.globl t_store
		li		$2, 0x12345678
		sb		$2, 1($30)
		sh		$2, 2($30)
		sw		$2, 0($31)
		
t_ld_lw_145:
		lw		$17, 0($30)				# load from memory
		li		$18, 0x56787800				# load expected as immediate
		bne		$17, $18, B2			# branch if not equal
		lw		$0, 0($30)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot

		
t_ld_lw_146:
		lw		$19, 0($31)				# load from memory
		li		$20, 0x12345678				# load expected as immediate
		bne		$19, $20, B2			# branch if not equal
		lw		$0, 0($31)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		# Check for exceptions on unaligned loads/stores as well as on
		# accesses to addresses below MIPS_LOWBASE (0x1000).  Also check that
		# failed loads and stores do not change the target.

t_ldst_exn:
.globl t_ldst_exn
		li		$2, 0x12345678
		move	$3, $2
		move	$4, $0
		
t_unl_lh_147:
		.globl	EXN10
EXN10:		lh		$2, 1($30)				# label to mark exception
		break	3					# if not skipped by exn handler

		
t_unl_lhu_148:
		.globl	EXN11
EXN11:		lhu		$2, 1($30)				# label to mark exception
		break	3					# if not skipped by exn handler

		
t_unl_lw_149:
		.globl	EXN12
EXN12:		lw		$2, 1($30)				# label to mark exception
		break	3					# if not skipped by exn handler

		
t_unl_lw_150:
		.globl	EXN13
EXN13:		lw		$2, 2($30)				# label to mark exception
		break	3					# if not skipped by exn handler

		
t_unl_lw_151:
		.globl	EXN14
EXN14:		lw		$2, 3($30)				# label to mark exception
		break	3					# if not skipped by exn handler

		
t_unl_lw_152:
		.globl	EXN15
EXN15:		lw		$2, 0($4)				# label to mark exception
		break	3					# if not skipped by exn handler
		# null ptr check
		
t_unl_lw_153:
		.globl	EXN16
EXN16:		lw		$2, 0xFFC($4)				# label to mark exception
		break	3					# if not skipped by exn handler
	# null ptr check
		bne		$2, $3, B2

		
t_uns_sh_154:
		.globl	EXN17
EXN17:		sh		$0, 1($30)			# label to mark exception
		break	3					# if not skipped by exn handler

		
t_uns_sw_155:
		.globl	EXN18
EXN18:		sw		$0, 1($30)			# label to mark exception
		break	3					# if not skipped by exn handler

		
t_uns_sw_156:
		.globl	EXN19
EXN19:		sw		$0, 2($30)			# label to mark exception
		break	3					# if not skipped by exn handler

		
t_uns_sw_157:
		.globl	EXN20
EXN20:		sw		$0, 3($30)			# label to mark exception
		break	3					# if not skipped by exn handler


		
t_ld_lw_158:
		lw		$2, 0($30)				# load from memory
		li		$3, 0x56787800				# load expected as immediate
		bne		$2, $3, B2			# branch if not equal
		lw		$0, 0($30)			# try to change R(0)
		bne		$0, $23, B1		# branch if R(0) or R(23) changed
		nop							# branch slot


		
t_uns_sw_159:
		.globl	EXN21
EXN21:		sw		$0, 0($4)			# label to mark exception
		break	3					# if not skipped by exn handler
				# null ptr check
		
t_uns_sw_160:
		.globl	EXN22
EXN22:		sw		$0, 0xFFC($4)			# label to mark exception
		break	3					# if not skipped by exn handler
			# null ptr check

		# Test lwl/lwr/swl/swr.  TODO: should test all mod 4 combinations!

t_unaligned:
.globl t_unaligned
		lw		$2, 0($28)
		lwl		$2, 2($29)
		li		$3, 0xD6E7F878
		bne		$2, $3, B2
		lwr		$2, 3($28)
		li		$3, 0xD6E7F812
		bne		$2, $3, B2
		nop

		li		$2, 0x12345678
		sw		$0, 0($30)
		sw		$0, 0($31)
		swl		$2, 1($31)
		li		$3, 0x00001234
		lw		$4, 0($31)
		bne		$3, $4, B2
		swr		$2, 2($30)
		li		$3, 0x56780000
		lw		$4, 0($30)
		bne		$3, $4, B2
		nop

		.data	# Data used by load/store tests
		.align	4
w1:		.byte	0x78, 0x56, 0x34, 0x12
w2:		.byte	0xF8, 0xE7, 0xD6, 0xC5
w3:		.byte	0x00, 0x00, 0x00, 0x00
w4:		.byte	0x00, 0x00, 0x00, 0x00
		.text	# Switch back to .text segment
		# load/store tests
#  ===========================================================================
#  COPYRIGHT (c) 2008 Zeljko Vrba <zvrba.external@zvrba.net>
# 
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#  ===========================================================================
#
# Included as part of cputorture.sm4.  Tests branching + delay slots.
#

		move	$2, $0
		j		t_j
		addi	$2, $0, 7		# add 7 in branch slot
		break	2					# should never be reached

t_j:
.globl t_j
		addi	$2, $2, -7		# subtract 7 to get 0
		bne		$2, $0, B2		# branch if != 0
		nop							# branch slot

t_jal:
.globl t_jal
		li		$2, 11
		jal		t_link				# call subroutine
		addi	$2, $2, 1		# branch slot; add 1 to get R(2)==12
		addi	$2, $2, 1		# executed after return, set R(2) to 14
		li		$3, 14
		bne		$2, $3, B2
		nop							# branch slot

t_jalr:
.globl t_jalr
		li		$2, 11			# same as above, but with jalr
		la		$4, t_link		# to test indirect call
		jalr	$31, $4			# test explicit store of retaddr
		addi	$2, $2, 1		# branch slot; sets R(2)==12
		addi	$2, $2, 1		# executed after return, set R(2) to 14
		li		$3, 14
		bne		$2, $3, B2
		nop							# branch slot
		j		t_beq				# continue execution
		nop							# branch slot
		
t_link:
.globl t_link
		li		$3, 12			# Entered with R(2)==12
		bne		$2, $3, B2		# Branch if not equal
		jr		$31				# Return
		addi	$2, $2, 1		# Branch slot: set R(2) to 13
		break	2					# Should never be executed

		# Test forward jumps, backward jumps (bltzal/bgtzal to t_link)
		# and branch slots.

t_beq:
.globl t_beq
		li		$3, 1
		beq		$3, $0, B2		# Not equal, should NOT branch
		xor		$2, $2, $2	# Set to 0
		beq		$0, $2, t_bne	# Equal, SHOULD branch
		addi	$2, $2, 1
		break	2

t_bne:
.globl t_bne
		bne		$0, $0, B2		# equal, should NOT branch
		nop			  				# branch slot
		bne		$2, $3, B2		# must be 1
		addi	$3, $3, 1
		bne		$2, $0, t_blez	# not equal, SHOULD branch
		addi	$2, $2, 1
		break	2

t_blez:
.globl t_blez
		bne		$2, $3, B2		# did last branch slot got executed?
		li		$4, -1
		blez	$4, 1f			# R(4) < 0, should be taken
		addi	$4, $4, 2		# set R(4) to +1 in branch slot
		break	2
1:		blez	$4, B2			# R(4)==1, should not be taken
		addi	$4, $4, -1		# set R(4) to 0
		blez	$4, t_bltz		# should be taken
		addi	$4, $4, 1		# set R(4) to 1
		break	2

t_bltz:
.globl t_bltz
		bltz	$4, B2			# should not branch as R(4)==+1
		addi	$4, $4, -1		# set R(4)==0
		bltz	$4, B2			# should not branch (as 0 < 0 == false)
		addi	$4, $4, -1		# set R(4) to -1
		bltz	$4, t_bltzal		# must branch
		addi	$4, $4, 1		# set to 0 in branch slot
		break	2

t_bltzal:
.globl t_bltzal
		bne		$4, $0, B2
		addi	$4, $4, 1		# set R(4) to 1
		bltzal	$4, B2			# should not branch as R(4)==+1
		addi	$4, $4, -1		# set R(4) to 0
		li		$2, 11
		bltzal	$4, B2			# should not branch as R(4)==0
		addi	$4, $4, -1		# set R(4) to -1
		li		$2, 11
		bltzal	$4, t_link		# should branch as R(4)==-1
		addi	$2, $2, 1
		addi	$2, $2, 1
		addi	$2, $2, -14
		bne		$2, $0, B2		# R(2) must be 14
		nop

t_bgez:
.globl t_bgez
		li		$4, -1
		bgez	$4, B2			# must NOT branch
		addi	$4, $4, 1		# set R(4) to 0
		bgez	$4, 1f			# must branch as R(4)==0
		addi	$4, $4, 1		# set R(4) to 1 in branch slot
		break	2
1:		bgez	$4, t_bgtz		# must branch
		addi	$4, $4, 1
		break	2

t_bgtz:
.globl t_bgtz
		bgtz	$4, 1f			# R(4)==2, must branch
		addi	$4, $4, -2		# set R(4)=0
		break	2
1:		bgtz	$4, B2			# R(4)==0, must not branch
		addi	$4, $4, -1
		bgtz	$4, B2			# R(4)==-1, must not branch
		nop

t_bgezal:
.globl t_bgezal
		li		$2, 11
		bgezal	$4, B2			# should not branch as R(4)==-1
		addi	$4, $4, 1
		bgezal	$4, t_link		# should branch as R(4)==0
		addi	$2, $2, 1
		addi	$2, $2, 1
		addi	$2, $2, -14
		bne		$2, $0, B2		# R(2) must be 14
		addi	$4, $4, 1		# set R(4) to 1
		li		$2, 11
		bgezal	$4, t_link		# should branch as R(4)==1
		addi	$2, $2, 1
		addi	$2, $2, 1
		addi	$2, $2, -14
		bne		$2, $0, B2		# R(2) must be 14
		nop
			# branch tests
		.globl	SUCCESS
SUCCESS:
		break 0				# mark successful completion

		# Failure labels
		.globl	FAILURE
FAILURE:
B1:		break	1
B2:		break	2
B3:		break	3

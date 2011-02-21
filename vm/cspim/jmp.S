.set mips1
.text
.align 4
	
/*
	Calling convention:
	Input arguments: $a0 - $a3 ($4-$7)
	Result: $v0 ($2)
	Return address in $ra ($31)
*/	

setjmp:
.globl setjmp
.type setjmp,@function
	sw		$ra, 0($4)
	sw		$sp, 4($4)
	sw		$fp, 8($4)
	sw		$gp, 12($4)
	sw		$16, 16($4)
	sw		$17, 20($4)
	sw		$18, 24($4)
	sw		$19, 28($4)
	sw		$20, 32($4)
	sw		$21, 36($4)
	sw		$22, 40($4)
	sw		$23, 44($4)
	move	$2, $0
	jr		$31

longjmp:
.globl longjmp
.type longjmp, @function
	lw		$ra, 0($4)
	lw		$sp, 4($4)
	lw		$fp, 8($4)
	lw		$gp, 12($4)
	lw		$16, 16($4)
	lw		$17, 20($4)
	lw		$18, 24($4)
	lw		$19, 28($4)
	lw		$20, 32($4)
	lw		$21, 36($4)
	lw		$22, 40($4)
	lw		$23, 44($4)
	move	$2, $5
	bne		$2, $0, 1f
	li		$2, 1
1:	jr		$31

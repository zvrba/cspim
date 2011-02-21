#include "spim.h"

int main(void)
{
	print_string("Hello world!.. we should exit with invalid SYSCALL, code=17\n");
	//to test traps on invalid memory access
	//asm volatile("xor $1, $1, $1 ; jr $1");
	asm volatile("SYSCALL 17");
	return 1;
}

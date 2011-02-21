#include "spim.h"

/* Test recursion. */
static unsigned fact(unsigned n)
{
	return (n == 1) ? 1 : n*fact(n-1);
}

int main(void)
{
	unsigned n;

	print_string("n=");
	n = read_int();
	n = fact(n);
	print_string("n!=");
	print_int(n);
	print_char('\n');
	return 0;
}

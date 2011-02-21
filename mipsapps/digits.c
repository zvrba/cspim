#include "spim.h"

static const char *names[10] = {
	"zero", "one", "two", "three", "four",
	"five", "six", "seven", "eight", "nine"
};

/* Digits are deliberately written backwards to test divide/modulus. */
static void print_digits(int x)
{
	do {
		int d = x % 10;
		print_string(names[d]);
		print_char(' ');
	} while(x /= 10);
	print_char('\n');
}

int main(void)
{
	int n;
	
	print_string("? ");
	n = read_int();
	if(n < 0)
		print_string("invalid input\n");
	else
		print_digits(n);
	return 0;
}

#include "spim.h"

static void print_move(int verbose, int from, int to)
{
	if(verbose) {
		print_int(from);
		print_string("->");
		print_int(to);
		print_char('\n');
	}
}

static void hanoi(int n, int verbose, int from, int to, int extra)
{
	if(!n)
		return;
	hanoi(n-1, verbose, from, extra, to);
	print_move(verbose, from, to);
	hanoi(n-1, verbose, extra, to, from);
}

int main(void)
{
	int n, v;
	
	print_string("n? ");
	n = read_int();
	if(n <= 0) {
		print_string("invalid input\n");
		return 1;
	}
	print_string("output moves [y/n]? ");
	v = read_char() == 'y';
	hanoi(n, v, 1, 2, 3);
	return 0;
}

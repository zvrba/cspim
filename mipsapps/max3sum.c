#include "spim.h"

int main(void)
{
	int n1, n2, n3, s;

	print_string("?1 "); n1 = read_int();
	print_string("?2 "); n2 = read_int();
	print_string("?3 "); n3 = read_int();
	
	if((n1 <= n2) && (n1 <= n3)) {
		s = n2+n3;
	} else if((n2 <= n1) && (n2 <= n3)) {
		s = n1+n3;
	} else if((n3 <= n1) && (n3 <= n2)) {
		s = n1+n2;
	} else {
		asm volatile("BREAK 0x3ff");
	}

	print_int(s);
	print_char('\n');
	return 0;
}

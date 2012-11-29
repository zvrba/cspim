#include "spim.h"

/* Just test initialization of globals. */
static unsigned sum = 10;
static unsigned v2 = 0;

int main(void)
{
	int n;
	
	if(sum != 10)
		print_string("FAIL, sum != 10\n");
	if(v2 != 0)
		print_string("FAIL, v2 != 0\n");

	while(1) {
		print_string("? ");
		n = read_int();
		if(n <= 0)
			break;
		sum += n;
	}
	print_string("S=");
	print_int(sum-10);
	print_char('\n');
	return 0;
}

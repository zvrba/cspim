#include "spim.h"

static void ioint(void)
{
	int x;
	
	print_string("Enter a number: ");
	x = read_int();
	print_string("You entered ");
	print_int(x);
	print_char('\n');
}

static void iostring(void)
{
	char buf[17];

	print_string("Enter a string [max 16 chars]: ");
	read_string(buf, sizeof(buf));
	print_string("You entered ");
	print_string(buf);
	print_char('\n');
}

int main(void)
{
	int ch;

	do {
		ioint();
		iostring();
		print_string("\nContinue [y/n]? ");
		ch = read_char();
		read_char();			/* eat \n */
	} while(ch == 'y');
	return 2;					/* just to check $2 */
}

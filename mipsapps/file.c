#include "spim.h"

void test_rdwr(int fd)
{
	unsigned i, j, off;

	for(i = 0; i < 1024; i++) {
		if(write(fd, &i, sizeof(i)) != sizeof(i)) {
			print_string("FAILED: write\n");
			return;
		}
		off = lseek(fd, 0, SEEK_CUR);
		if(off != (i+1)*sizeof(int)) {
			print_string("FAILED: wrong offset after write (");
			print_int(off);
			print_string(")\n");
			return;
		}
	}

	lseek(fd, 0, SEEK_SET);
	for(i = 0; (off = read(fd, &j, sizeof(j))) == 4; i++) {
		print_int(i);
		print_char('\n');
		if(i != j) {
			print_string("FAILED: expected and read values don't match\n");
			return;
		}
	}
	if(off != 0) {
		print_string("FAILED: short read (");
		print_int(off);
		print_string(")\n");
	}
	
	lseek(fd, -8U, SEEK_END);
	i = read(fd, &j, sizeof(i));
	if((i != sizeof(i)) || (j != 1022)) {
		print_string("FAILED: SEEK_END\n");
		return;
	}

	print_string("PASSED\n");
}


int main(void)
{
	int fd;
	
	print_string("This will create a file named DATA.\n");
	fd = open("DATA", O_RDWR, 0600);
	if(fd < 0) {
		print_string("open failed\n");
		return 1;
	}

	test_rdwr(fd);

	if(close(fd) < 0) {
		print_string("close failed\n");
		return 1;
	}
	return 0;
}

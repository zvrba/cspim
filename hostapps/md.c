/* 
 * File:    md.c
 * Author:  zvrba
 * Created: 2008-02-23
 *
 * A program to calculate multiplication/division results.  Used to assist
 * in writing test vectors for signed multiplication/division.
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	unsigned long ux, uy, uhi, ulo;
	unsigned long long up;

	if(argc != 3)
		return 1;

	ux = strtoul(argv[1], NULL, 16);
	uy = strtoul(argv[2], NULL, 16);

	printf("%lx %lx\n", ux, uy);

	up = (long long)(long)ux * (long long)(long)uy;
	uhi = up >> 32; ulo = up & 0xFFFFFFFF;
	printf("mult : %08lx %08lx\n", uhi, ulo);

	up = (unsigned long long)ux * (unsigned long long)uy;
	uhi = up >> 32; ulo = up & 0xFFFFFFFF;
	printf("multu: %08lx %08lx\n", uhi, ulo);

	uhi = (long)ux % (long)uy; ulo = (long)ux / (long)uy;
	printf("div  : %08lx %08lx\n", uhi, ulo);

	uhi = ux % uy; ulo = ux / uy;
	printf("divu : %08lx %08lx\n", uhi, ulo);

	return 0;
}

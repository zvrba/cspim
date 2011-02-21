/*
Matrix multiplication benchmark.  Problem size is set from the host, but the
space for the matrices is preallocated up to the maximum size.  We must use
unsigned arithmetic to avoid integer overflow.
*/

#ifdef NATIVE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned long long gettime(void)
{
	struct timespec tp;
	if(clock_gettime(CLOCK_MONOTONIC, &tp) < 0)
		exit(1);
	return tp.tv_sec * 1000000000ULL + tp.tv_nsec;
}

#else
#include "spim.h"
#endif

#define	MAXSIZE	1000

unsigned PARAMS[1];
unsigned long long TIME;

static unsigned
	A[MAXSIZE][MAXSIZE],
	B[MAXSIZE][MAXSIZE],
	C[MAXSIZE][MAXSIZE];

static void init(unsigned n)
{
	unsigned i, j;

	for(i = 0; i < n; ++i) {
		for(j = 0; j < n; ++j) {
			A[i][j] = i-j;
			B[i][j] = (i-j+1)*(i+j-1);
			C[i][j] = 0;
		}
	}
}

static void mult(unsigned n)
{
	unsigned i, j, k;

	for(i = 0; i < n; ++i)
		for(j = 0; j < n; ++j)
			for(k = 0; k < n; ++k)
				C[i][j] += A[i][k] * B[k][j];
}

int main(int argc, char **argv)
{
#ifdef NATIVE
	if(argc != 2) {
		fprintf(stderr, "USAGE: %s N\n", argv[0]);
		return 1;
	}
	PARAMS[0] = atoi(argv[1]);
#endif

	if(!PARAMS[0] || (PARAMS[0] > MAXSIZE))
		return 1;
	TIME = gettime();
	init(PARAMS[0]);
	mult(PARAMS[0]);
	TIME = gettime() - TIME;
#ifdef NATIVE
	printf("%llu\n", TIME);
#endif
	return 0;
}


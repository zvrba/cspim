/* Recursive towers of hanoi benchmark. */

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

unsigned PARAMS[1];
unsigned long long TIME;

static void hanoi(volatile int n, int from, int to, int extra)
{
	if(!n)
		return;
	hanoi(n-1, from, extra, to);
	hanoi(n-1, extra, to, from);
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

	if(!PARAMS[0])
		return 1;
	TIME = gettime();
	hanoi(PARAMS[0], 1, 2, 3);
	TIME = gettime() - TIME;
#ifdef NATIVE
	printf("%llu\n", TIME);
#endif
	return 0;
}

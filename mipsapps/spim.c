#include "spim.h"

#define SYSCALL(n)								\
	({											\
		register void *rv asm("$2");			\
		asm volatile("li $2, %1;"				\
					 "SYSCALL 0x9107C"			\
					 : "=r" (rv)				\
					 : "K" (n));				\
		rv;										\
	})

unsigned long long gettime(void)
{ SYSCALL(3); }

void print_int(int n)
{ SYSCALL(1); }

void print_string(const char *str)
{ SYSCALL(4); }

int read_int(void)
{ return (int)SYSCALL(5); }

void read_string(char *buf, unsigned len)
{ SYSCALL(8); }

void *sbrk(unsigned amount)
{ return SYSCALL(9); }

void print_char(char ch)
{ SYSCALL(11); }

int read_char(void)
{ return (int)SYSCALL(12); }

int open(const char *buf, int mode, int flags)
{ return (int)SYSCALL(13); }

int read(int fd, void *buf, unsigned len)
{ return (int)SYSCALL(14); }

int write(int fd, void *buf, unsigned len)
{ return (int)SYSCALL(15); }

int close(int fd)
{ return (int)SYSCALL(16); }

int lseek(int fd, unsigned off, int whence)
{ return (int)SYSCALL(2); }

void _start(void)
{ asm volatile("jal main ; break 0"); }

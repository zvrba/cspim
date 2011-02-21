/* 
 * File:    cpu.h
 * Author:  zvrba
 * Created: 2008-03-20
 *
 * Interface to SPIM system calls.
 */
#ifndef SPIM_H__
#define SPIM_H__

/* These constants must match the native system's constants!  And these modes
 * seem to be the only ones that are equal among unices (at least Solaris and
 * linux).  Therefore, open also adds O_CREAT if it detects writing mode. */

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

unsigned long long gettime(void);
void print_int(int);
void print_string(const char*);
int read_int(void);
void read_string(char*, unsigned);
void *sbrk(unsigned);
void print_char(char);
int read_char(void);
int open(const char*, int, int);
int read(int, void*, unsigned);
int write(int, void*, unsigned);
int close(int);
int lseek(int, unsigned, int);

#endif	/* SPIM_H__ */

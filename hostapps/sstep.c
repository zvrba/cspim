/* 
 * File:    sstep.c
 * Author:  zvrba
 * Created: 2008-02-30
 *
 * ===========================================================================
 * COPYRIGHT (c) 2008 Zeljko Vrba <zvrba.external@zvrba.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * ===========================================================================
 */
/**
 * @file
 * Program that uses ptrace() to single-step a target until it exits.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void do_step(pid_t);

int main(int argc, char **argv)
{
	pid_t pid;
	
	if(argc < 2) {
		fprintf(stderr, "USAGE: %s PROGRAM ARGS...\n", argv[0]);
		exit(1);
	}
	
	if((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	} else if(pid == 0) {
		char *env[] = { NULL };
		
		if(ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
			perror("ptrace");
			exit(1);
		}
		if(execve(argv[1], argv+1, env) < 0) {
			perror("execve");
			exit(1);
		}
	}
	
	/* Parent. */
	do_step(pid);
	return 0;
}

static void do_step(pid_t pid)
{
	int status;
	
	if(wait(&status) < 0) {
		perror("wait");
		exit(1);
	}
	while(WIFSTOPPED(status) && (WSTOPSIG(status) == SIGTRAP)) {
		if(ptrace(PTRACE_SINGLESTEP, pid, 0, 0) < 0) {
			perror("ptrace");
			exit(1);
		}
		if(wait(&status) < 0) {
			perror("waitpid");
			exit(1);
		}
	}
}

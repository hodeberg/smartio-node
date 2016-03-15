/*
 * newlib_os.c
 *
 *  Created on: 10 mar 2016
 *      Author: hodeberg
 *
 *      Contains OS stubs for newlib
 */

#include <sys/types.h>
#include <sys/stat.h>

caddr_t _sbrk(int incr)
{
	return NULL;
}

int _write(int file, char *ptr, int len)
{
	return -1;
}

int _close(int file)
{
	return -1;
}

int _fstat(int file, struct stat *st)
{
	return -1;
}


int _isatty(int file)
{
	return -1;
}

int _lseek(int file, int ptr, int dir)
{
	return -1;
}

int _read(int file, char *ptr, int len)
{
	return -1;
}

int _getpid(void)
{
	return -1;
}

int _kill(int pid, int sig)
{
	return -1;
}

#ifndef _CAPLIB_H_
#define _CAPLIB_H_

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#include "asf.h"

void *MALLOC(size_t size);
void FREE(void *ptr);
FILE *FOPEN(const char *file,const char *mode);
size_t FREAD(void *ptr,size_t size,size_t nitems,FILE *stream);
size_t FWRITE(const void *ptr,size_t size,size_t nitems,FILE *stream);
int FSEEK(FILE *stream,int offset,int ptrname);
int FSEEK64(FILE *stream,long long offset, int ptrname);
long long FTELL64(FILE *stream);
int FCLOSE(FILE *stream);
int FFLUSH(FILE *stream);

void programmer_error(char *mess);

#endif

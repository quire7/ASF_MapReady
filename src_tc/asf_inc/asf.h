/*Standard ASF Utility include file.*/
#ifndef __ASF_H
#define __ASF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#ifndef PI
# define PI 3.14159265358979323846
#endif
#ifndef M_PI
# define M_PI PI
#endif
#define D2R (PI/180.0)
#define R2D (180.0/PI)

#include "caplib.h"
void bail(char *message);
void StartWatch();
void StopWatch();

/*****************************************
FileUtil:
	A collection of file I/O utilities.
Actually implemented in asf.a/fileUtil.c

extExists returns whether the given
file basename and extension exist and
are readable.

fileExists returns whether the given file
name exists and is readable.

findExt returns a pointer to the beginning
(the period) of the given name's first
extension, or NULL if none exists.

appendExt allocates its return string
on the heap, so it must be free'd, or
memory will leak.  It can take a NULL extension,
whereupon it just allocates a copy of the given
string, and returns it.

fopenImage first tries to open the given image 
name, then appends ".img" and tries again.
It returns a pointer to the opened file.
*/

int extExists(const char *name,const char *newExt);
int fileExists(const char *name);
char *findExt(char *name);
char *appendExt(const char *name,const char *newExt);
FILE *fopenImage(const char *name,const char *accessType);

void create_name(char *out,const char *in,const char *newExt);

#endif


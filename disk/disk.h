/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef TC_DISK_H
#define TC_DISK_H

#include <stdio.h>

#include "theclou.h"

#define DSK_PATH_MAX 512

typedef enum {
 DISK_CHECK_FILE,
 DISK_CHECK_DIR
} DiskCheckE;

extern void dskSetRootPath(const char *newRootPath);
extern char *dskGetRootPath(char *result);

void *dskLoad(const char *Pathname);
void dskSave(char *Pathname, void *src, size_t size);
bool dskBuildPathName(DiskCheckE check,
		      const char *Directory, const char *Filename, char *Result);
void dskClose(FILE *fp);
void dskWrite(FILE *fp, void *src, size_t size);

void dskWrite_U8(FILE * fp, U8 * x);
void dskWrite_S8(FILE * fp, S8 * x);
void dskWrite_U16LE(FILE * fp, U16 * x);
void dskWrite_S16LE(FILE * fp, S16 * x);
void dskWrite_U32LE(FILE * fp, U32 * x);
void dskWrite_S32LE(FILE * fp, S32 * x);

void dskRead(FILE *fp, void *dest, size_t size);

void dskRead_U8(FILE * fp, U8 * x);
void dskRead_S8(FILE * fp, S8 * x);
void dskRead_U16LE(FILE * fp, U16 * x);
void dskRead_S16LE(FILE * fp, S16 * x);
void dskRead_U32LE(FILE * fp, U32 * x);
void dskRead_S32LE(FILE * fp, S32 * x);


size_t dskFileLength(const char *Pathname);

FILE *dskOpen(const char *Pathname, const char *Mode);

extern bool dskGetLine(char *s, int size, FILE *fp);

int stricmp(const char *s1, const char *s2);
int strnicmp(const char *s1, const char *s2, size_t n);

#endif

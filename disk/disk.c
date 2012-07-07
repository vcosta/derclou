/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include <ctype.h>

#include "error/error.h"

#include "disk/disk.h"
#include "disk/disk.eh"

char RootPathName[DSK_PATH_MAX];


void dskSetRootPath(const char *newRootPath)
{
    strcpy(RootPathName, newRootPath);
}

char *dskGetRootPath(char *result)
{
    return strcpy(result, RootPathName);
}

FILE *dskOpen(const char *Pathname, const char *Mode)
{
    FILE *fp;

    DebugMsg(ERR_DEBUG, ERROR_MODULE_DISK, "Opening :%s (%s)", Pathname, Mode);

    if (!(fp = fopen(Pathname, Mode))) {
        DebugMsg(ERR_ERROR, ERROR_MODULE_DISK, "Open :%s", Pathname);
    }

    return fp;
}

void *dskLoad(const char *Pathname)
{
    FILE *fp;
    U8 *ptr;
    size_t size, pos;

    pos  = 0;
    size = BUFSIZ;
    ptr  = malloc(size);

    if ((fp = dskOpen(Pathname, "rb"))) {
        size_t nread;

        while ((nread = fread(ptr+pos, 1, BUFSIZ, fp)) == BUFSIZ) {
            pos  += nread;
            ptr   = realloc(ptr, size+BUFSIZ);
            size += nread;
        }

        pos += nread;

        ptr = realloc(ptr, pos);
        dskClose(fp);
        return (void *)ptr;
    }
    return NULL;
}

void dskSave(char *Pathname, void *src, size_t size)
{
    FILE *fp;

    if ((fp = dskOpen(Pathname, "wb"))) {
	dskWrite(fp, src, size);
	dskClose(fp);
    }
}

static void strUpper(char *s)
{
    while (*s != '\0') {
        *s = toupper(*s);
        s++;
    }
}

static void strLower(char *s)
{
    while (*s != '\0') {
        *s = tolower(*s);
        s++;
    }
}

bool dskBuildPathName(DiskCheckE check,
		      const char *Directory, const char *Filename, char *Result)
{
    char Dir [DSK_PATH_MAX];
    char File[DSK_PATH_MAX];
    struct stat status;

    int step = 0;

    do {
        switch (step++) {
        case 0:
            strcpy(Dir, Directory);
            strcpy(File, Filename);
            break;

        case 1:
            strUpper(Dir);
            strUpper(File);
            break;

        case 2:
            strLower(Dir);
            strLower(File);
            break;

        case 3:
            sprintf(Result, "%s" DIR_SEP "%s" DIR_SEP "%s",
                RootPathName, Directory, Filename);

            DebugMsg(ERR_DEBUG, ERROR_MODULE_DISK,
                "Path failure: %s", Result);
            return false;
        }

        if (check == DISK_CHECK_FILE) {
            sprintf(Result, "%s" DIR_SEP "%s" DIR_SEP "%s",
                RootPathName, Dir, File);
         } else {
            sprintf(Result, "%s" DIR_SEP "%s", RootPathName, Dir);
         }

    } while (stat(Result, &status) == -1);

    if (check == DISK_CHECK_DIR) {
        strcat(Result, DIR_SEP);
        strcat(Result, File);
    }

    return true;
}

size_t dskFileLength(const char *Pathname)
{
    struct stat status;

    if (stat(Pathname, &status) == -1) {
        return 0;
    } else {
        return status.st_size;
    }
}

void dskClose(FILE *fp)
{
    if (fp) {
	fclose(fp);
    }
}

void dskWrite(FILE * fp, void *src, size_t size)
{
    if (fwrite(src, 1, size, fp) != size) {
	ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_WRITE_FAILED);
    }
}

void dskWrite_U8(FILE * fp, U8 * x)
{
    U8 tmp;

    tmp = *x;
    dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_S8(FILE * fp, S8 * x)
{
    S8 tmp;

    tmp = *x;
    dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_U16LE(FILE * fp, U16 * x)
{
    U8 tmp[2];

    tmp[0] = (U8) ((*x) & 0xff);
    tmp[1] = (U8) ((*x >> 8) & 0xff);
    dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_S16LE(FILE * fp, S16 * x)
{
    U8 tmp[2];

    tmp[0] = (U8) ((*x) & 0xff);
    tmp[1] = (U8) ((*x >> 8) & 0xff);
    dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_U32LE(FILE * fp, U32 * x)
{
    U8 tmp[4];

    tmp[0] = (U8) ((*x) & 0xff);
    tmp[1] = (U8) ((*x >> 8) & 0xff);
    tmp[2] = (U8) ((*x >> 16) & 0xff);
    tmp[3] = (U8) ((*x >> 24) & 0xff);
    dskWrite(fp, &tmp, sizeof(tmp));
}

void dskWrite_S32LE(FILE * fp, S32 * x)
{
    U8 tmp[4];

    tmp[0] = (U8) ((*x) & 0xff);
    tmp[1] = (U8) ((*x >> 8) & 0xff);
    tmp[2] = (U8) ((*x >> 16) & 0xff);
    tmp[3] = (U8) ((*x >> 24) & 0xff);
    dskWrite(fp, &tmp, sizeof(tmp));
}

void dskRead(FILE *fp, void *dest, size_t size)
{
    if (fread(dest, 1, size, fp) != size) {
	ErrorMsg(Disk_Defect, ERROR_MODULE_DISK, ERR_DISK_READ_FAILED);
    }
}

void dskRead_U8(FILE * fp, U8 * x)
{
    U8 tmp;

    dskRead(fp, &tmp, sizeof(tmp));
    *x = tmp;
}

void dskRead_S8(FILE * fp, S8 * x)
{
    S8 tmp;

    dskRead(fp, &tmp, sizeof(tmp));
    *x = tmp;
}

void dskRead_U16LE(FILE * fp, U16 * x)
{
    U8 tmp[2];

    dskRead(fp, &tmp, sizeof(tmp));
    *x = (U16) ((U16) tmp[0] | ((U16) tmp[1] << 8));
}

void dskRead_S16LE(FILE * fp, S16 * x)
{
    U8 tmp[2];

    dskRead(fp, &tmp, sizeof(tmp));
    *x = (S16) ((U16) tmp[0] | ((U16) tmp[1] << 8));
}

void dskRead_U32LE(FILE * fp, U32 * x)
{
    U8 tmp[4];

    dskRead(fp, &tmp, sizeof(tmp));
    *x = (U32) ((U32) tmp[0] | ((U32) tmp[1] << 8)
		| ((U32) tmp[2] << 16) | ((U32) tmp[3] << 24));
}

void dskRead_S32LE(FILE * fp, S32 * x)
{
    U8 tmp[4];

    dskRead(fp, &tmp, sizeof(tmp));
    *x = (S32) ((U32) tmp[0] | ((U32) tmp[1] << 8)
		| ((U32) tmp[2] << 16) | ((U32) tmp[3] << 24));
}

bool dskGetLine(char *s, int size, FILE *fp)
{
    int ch;

    while ((ch = getc(fp)) != EOF && ch != '\r' && size-- > 0)
	*s++ = ch;
    *s = '\0';

    while (ch != '\r' && (ch = getc(fp)) != EOF);

    if (ch == EOF)
	return false;

    getc(fp);			/* get trailing '\n' */
    return true;
}

int stricmp(const char *s1, const char *s2)
{
    while (*s1 && *s2) {
        int a, b;

        a = toupper(*s1++);
        b = toupper(*s2++);

        if (a < b) {
            return -1;
        }
        if (a > b) {
            return +1;
        }
    }

    if (*s1 == '\0' && *s2 != '\0')
        return -1;
    if (*s1 != '\0' && *s2 == '\0')
        return +1;
    return 0;
}

int strnicmp(const char *s1, const char *s2, size_t n)
{
    while (*s1 && *s2 && n--) {
        int a, b;

        a = toupper(*s1++);
        b = toupper(*s2++);

        if (a < b) {
            return -1;
        }
        if (a > b) {
            return +1;
        }
    }

    if (*s1 == '\0' && *s2 != '\0')
        return -1;
    if (*s1 != '\0' && *s2 == '\0')
        return +1;
    return 0;
}

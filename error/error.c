/*
**	$Filename: error/error.c
**	$Release:
**	$Revision:	04.10.1994 (hg)
**	$Date:
**
**	functions for error handling
**
**	(C) 1993, 1994 ...and avoid panic by, H. Gaberschek
**	    All Rights Reserved
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#define ERR_EXIT_ERROR      20L
#define ERR_EXIT_SHUTDOWN   30L

#include "base/base.h"

#include "error/error.h"

static const char *moduleNames[ERROR_MODULE_LAST] = {
  "",
  "",
  "",
  "Base",
  "Txt",
  "Dsk",
  "Mem",
  "Data",
  "GP",
  "L/S",
  "Land",
  "Liv",
  "Plan",
  "Snd",
  "Pres",
  "Gfx",
  "Input"
};

struct ErrorHandler {
    char Filename[DSK_PATH_MAX];
    bool uch_OutputToFile;
};

/* implementation */

struct ErrorHandler ErrorHandler;

bool pcErrOpen(S32 l_Mode, char *ErrorFilename)
{
    FILE *p_File;
    bool alright = false;

    switch (l_Mode) {
    case ERR_NO_OUTPUT:
        ErrorHandler.uch_OutputToFile = false;
        break;
    case ERR_OUTPUT_TO_DISK:
        ErrorHandler.uch_OutputToFile = true;

        /* lets have a look if we can open the file */
        /* and then lets clean it : */

        if ((p_File = dskOpen(ErrorFilename, "w"))) {
            alright = true;
            dskClose(p_File);
        }

        strcpy(ErrorHandler.Filename, ErrorFilename);
        break;
    default:
        break;
    }

    return alright;
}

void ErrorMsg(ErrorE type, ErrorModuleE moduleId, U32 errorId)
{
    DebugMsg(ERR_DEBUG, moduleId, "Error %d", errorId);

    tcDone();

    switch (type) {
    case Internal_Error:
	printf("Internal Error!\n");
	break;

    case No_Mem:
	printf("You don't have enough memory!\n");
	break;

    case Disk_Defect:
	printf("Can't open file! Please install DER CLOU! again\n");
	break;
    default:
	break;
    }
    exit(-1);
}

static void ErrDebugMsg(DebugE type, const char *moduleName, const char *txt)
{
    FILE *fp;

    if (setup.Debug < type) {
        return;
    }

    if (ErrorHandler.uch_OutputToFile) {	/* Ausgabe */
	if ((fp = fopen(ErrorHandler.Filename, "a"))) {
	    fprintf(fp, "%s\t: %s\n", moduleName, txt);
	    fclose(fp);
	}
    }

    switch (type) {
    case ERR_DEBUG:
	fprintf(stderr, "%s\t: %s\n", moduleName, txt);
        break;

    case ERR_WARNING:
	fprintf(stderr, "Module %s: %s\n", moduleName, txt);
        break;

    case ERR_ERROR:
	fprintf(stderr, "ERROR: Module %s: %s\n", moduleName, txt);

        tcDone();

	exit(ERR_EXIT_ERROR);
        break;
    }
}

void pcErrClose(void)
{
}

void DebugMsg(DebugE type, ErrorModuleE moduleId, const char *format, ...)
{
    va_list arglist;
    char txt[512];

    va_start(arglist, format);
    vsprintf(txt, format, arglist);
    va_end(arglist);

    ErrDebugMsg(type, moduleNames[moduleId], txt);
}

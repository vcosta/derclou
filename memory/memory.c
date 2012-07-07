/*
**	$Filename: memory/memory.c
**	$Release:
**	$Revision:
**	$Date:
**
**	function for memory allocation
**
**	(C) 1993, 1994 ...and avoid panic by
**	    All Rights Reserved
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "memory/memory.h"

#define ERR_MEMORY_NO_MEM       1

struct MemControl {
    ssize_t allocated;
};

static struct MemControl MemControl;


void *TCAllocMem(size_t size, bool clear)
{
    void *p;

    if (clear) {
        p = calloc(1, size);
    } else {
        p = malloc(size);
    }

    if (!p)
	ErrorMsg(No_Mem, ERROR_MODULE_MEMORY, ERR_MEMORY_NO_MEM);
    else
	MemControl.allocated += size;

    return p;
}

ssize_t memGetAllocatedMem(void)
{
    return MemControl.allocated;
}

void TCFreeMem(void *ptr, size_t size)
{
    if (ptr) {
	free(ptr);

	MemControl.allocated -= size;
    }
}

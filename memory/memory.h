/*
**	$Filename: memory/memory.h
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

#ifndef MODULE_MEMORY
#define MODULE_MEMORY

#include "theclou.h"

#include "error/error.h"

void *TCAllocMem(size_t size, bool clear);
void TCFreeMem(void *ptr, size_t size);

ssize_t memGetAllocatedMem(void);

#endif

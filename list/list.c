/*
 * list.c
 * (c) 1994 by Kaweh Kazemi
 * All rights reserved.
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "list/list.h"
#include "memory/memory.h"

LIST *CreateList(void)
{
    LIST *list = NULL;

    if ((list = (LIST *) TCAllocMem(sizeof(*list), true))) {
        NODE_SUCC(INNER_HEAD(list)) = INNER_TAIL(list);
        NODE_PRED(INNER_HEAD(list)) = NULL;
        NODE_NAME(INNER_HEAD(list)) = NULL;

        NODE_SUCC(INNER_TAIL(list)) = NULL;
        NODE_PRED(INNER_TAIL(list)) = INNER_HEAD(list);
        NODE_NAME(INNER_TAIL(list)) = NULL;
    }

    return list;
}

void RemoveList(LIST *list)
{
    RemoveNode(list, NULL);
    FreeList(list);
}

void FreeList(LIST *list)
{
    TCFreeMem(list, sizeof(*list));
}

void *AddNode(LIST *list, void *node, void *predNode)
{
    if (!predNode)
	predNode = INNER_HEAD(list);

    NODE_SUCC(node) = NODE_SUCC(predNode);
    NODE_PRED(node) = predNode;

    NODE_PRED(NODE_SUCC(predNode)) = node;
    NODE_SUCC(predNode) = node;
    return node;
}

void *AddTailNode(LIST *list, void *node)
{
    return AddNode(list, node, LIST_TAIL(list));
}

void *AddHeadNode(LIST *list, void *node)
{
    return AddNode(list, node, INNER_HEAD(list));
}

void *RemNode(void *node)
{
    NODE_SUCC(NODE_PRED(node)) = NODE_SUCC(node);
    NODE_PRED(NODE_SUCC(node)) = NODE_PRED(node);

    /* just to be safe */
    NODE_SUCC(node) = NULL;
    NODE_PRED(node) = NULL;

    return node;
}

void *RemHeadNode(LIST *list)
{
    if (!LIST_EMPTY(list))
	return RemNode(LIST_HEAD(list));
    else
	return NULL;
}

void *RemTailNode(LIST *list)
{
    if (!LIST_EMPTY(list))
	return RemNode(LIST_TAIL(list));
    else
	return NULL;
}

void *CreateNode(LIST *list, size_t size, const char *name)
{
    register NODE *node = NULL;
    register size_t len = 0;

    if (!size)
	size = sizeof(NODE);

    if (name)
	len = strlen(name) + 1;

    if (size >= sizeof(NODE)) {
	if ((node = (NODE *) TCAllocMem(size + len, true))) {
	    NODE_SUCC(node) = NULL;
	    NODE_PRED(node) = NULL;
	    NODE_SIZE(node) = size;

	    if (name)
		NODE_NAME(node) = strcpy((char *) node + size, name);
	    else
		NODE_NAME(node) = NULL;

	    if (list)
		AddTailNode(list, node);
	}
    } else {
	ErrorMsg(Internal_Error, ERROR_MODULE_MEMORY, 666);
    }

    return (void *) node;
}

void RemoveNode(LIST *list, const char *name)
{
    register NODE *node;

    if (name) {
	if ((node = GetNode(list, name))) {
	    RemNode(node);
	    FreeNode(node);
	}
    } else {
	if (!LIST_EMPTY(list)) {
	    while ((node = RemTailNode(list)))
		FreeNode(node);
	}
    }
}

void FreeNode(void *node)
{
    size_t size;

    size = NODE_SIZE(node);

    if (NODE_NAME(node))
	size += strlen(NODE_NAME(node)) + 1;

    TCFreeMem(node, size);
}

void *GetNode(LIST *list, const char *name)
{
    register NODE *node;

    for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node)) {
	if (strcmp(NODE_NAME(node), name) == 0)
	    return node;
    }

    return NULL;
}

void *GetNthNode(LIST *list, U32 nth)
{
    register NODE *node;

    for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node)) {
	if (nth == 0)
	    return node;
	nth--;
    }

    return NULL;
}

U32 GetNrOfNodes(LIST *list)
{
    register NODE *node = NULL;
    register U32 i = 0;

    for (i = 0, node = LIST_HEAD(list); NODE_SUCC(node);
	 i++, node = NODE_SUCC(node));

    return i;
}

U32 GetNodeNrByAddr(LIST *list, void *node)
{
    register NODE *s;
    register U32 i;

    for (s = LIST_HEAD(list), i = 0; NODE_SUCC(s) && (s != node);
	 s = NODE_SUCC(s), i++);

    return i;
}

U32 GetNodeNr(LIST *list, const char *name)
{
    return GetNodeNrByAddr(list, GetNode(list, name));
}

void foreach(LIST *list, void (*processNode) (void *))
{
    register NODE *node;

    for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node))
	processNode(node);
}

void Link(LIST *list, void *node, void *predNode)
{
    if (predNode)
	AddNode(list, node, predNode);
    else
	AddTailNode(list, node);
}

void *UnLinkByAddr(LIST *list, void *node, NODE **predNode)
{
    if (predNode)
	*predNode = NODE_PRED(node);

    return RemNode(node);
}

void *UnLink(LIST *list, const char *name, NODE **predNode)
{
    return UnLinkByAddr(list, GetNode(list, name), predNode);
}

void ReplaceNodeByAddr(LIST *list, void *node, NODE *newNode)
{
    NODE *predNode;

    if ((node = UnLinkByAddr(list, node, &predNode))) {
	Link(list, newNode, predNode);
	FreeNode(node);
    }
}

void ReplaceNode(LIST *list, const char *name, NODE *newNode)
{
    ReplaceNodeByAddr(list, GetNode(list, name), newNode);
}

U32 ReadList(LIST *list, size_t size, char *fileName)
{
    FILE *fh;
    U32 i = UINT32_C(0);
    char buffer[256];

    if ((fh = dskOpen(fileName, "rb"))) {
	while (dskGetLine(buffer, sizeof(buffer), fh)) {
	    if (buffer[0] != ';')	/* skip comments */
	    {
		if (!CreateNode(list, size, buffer)) {
		    RemoveNode(list, NULL);
		    fclose(fh);
		    return 0;
		}

		i++;
	    }
	}

	fclose(fh);
    }
    return i;
}

void WriteList(LIST *list, char *fileName)
{
    register FILE *fh = NULL;
    register NODE *node = NULL;

    if ((fh = dskOpen(fileName, "wb"))) {
	for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node))
	    fprintf(fh, "%s\r\n", NODE_NAME(node));

	fclose(fh);
    }
}

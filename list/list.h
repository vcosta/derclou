/*
**  Der Clou!
**
** $VER: list/list.h 0.2 (10.11.2000)
**
** Reproduced by Oliver Gantert <lucyg@t-online.de>
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_LIST
#define MODULE_LIST

#include <stdlib.h>

#include "theclou.h"

/**************
 * Structures *
 **************/

typedef struct Node {
    struct Node *Succ;
    struct Node *Pred;
    char *Name;
    size_t Size;
} NODE;

typedef struct List {
    NODE Head;
    NODE Tail;
} LIST;

/**********
 * Macros *
 **********/

#define NODE_SUCC(node) (((NODE *)(node))->Succ)
#define NODE_PRED(node) (((NODE *)(node))->Pred)
#define NODE_NAME(node) (((NODE *)(node))->Name)
#define NODE_SIZE(node) (((NODE *)(node))->Size)

#define INNER_HEAD(list) (&(list)->Head)
#define INNER_TAIL(list) (&(list)->Tail)

#define LIST_HEAD(list) (NODE_SUCC(INNER_HEAD((list))))
#define LIST_TAIL(list) (NODE_PRED(INNER_TAIL((list))))

#define LIST_EMPTY(list) (!NODE_SUCC(LIST_HEAD((list))))

/**************
 * Prototypes *
 **************/

LIST *CreateList(void);
void RemoveList(LIST *list);
void FreeList(LIST *list);
void *AddNode(LIST *list, void *node, void *predNode);
void *AddTailNode(LIST *list, void *node);
void *AddHeadNode(LIST *list, void *node);
void *RemNode(void *node);
void *RemHeadNode(LIST *list);
void *RemTailNode(LIST *list);
void *CreateNode(LIST *list, size_t size, const char *name);
void RemoveNode(LIST *list, const char *name);
void FreeNode(void *node);
void *GetNode(LIST *list, const char *name);
void *GetNthNode(LIST *list, U32 nth);
U32 GetNrOfNodes(LIST *list);
U32 GetNodeNrByAddr(LIST *list, void *node);
U32 GetNodeNr(LIST *list, const char *name);
void foreach(LIST *list, void (*processNode) (void *));
void Link(LIST *list, void *node, void *predNode);
void *UnLinkByAddr(LIST *list, void *node, NODE **predNode);
void *UnLink(LIST *list, const char *name, NODE **predNode);
void ReplaceNodeByAddr(LIST *list, void *node, NODE *newNode);
void ReplaceNode(LIST *list, const char *name, NODE *newNode);
U32 ReadList(LIST *list, size_t size, char *fileName);
void WriteList(LIST *list, char *fileName);

#endif				/* MODULE_LIST */

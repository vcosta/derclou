/*
 * Relation.c
 * (c) 1993 by Kaweh Kazemi
 * All rights reserved.
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "data/relation.h"

static RELATION nrOfRelations = 1;
static struct relationDef *relationsDefBase = NULL;

int (*CompareKey) (KEY, KEY) = NULL;
KEY(*EncodeKey) (char *) = NULL;
char *(*DecodeKey) (KEY) = NULL;


static struct relationDef *FindRelation(RELATION id)
{
    register struct relationDef *rd;

    for (rd = relationsDefBase; rd; rd = rd->rd_next) {
	if (rd->rd_id == id)
	    return rd;
    }

    return NULL;
}

RELATION AddRelation(RELATION id)
{
    register struct relationDef *rd;

    if (!FindRelation(id)) {
	if ((rd = (struct relationDef *) TCAllocMem(sizeof(*rd), 0))) {
	    rd->rd_next = relationsDefBase;
	    relationsDefBase = rd;

	    rd->rd_id = id;
	    nrOfRelations++;

	    rd->rd_relationsTable = NULL;

	    return rd->rd_id;
	}
    }

    return 0;
}

RELATION CloneRelation(RELATION id, RELATION cloneId)
{
    register struct relationDef *rd;

    if ((rd = FindRelation(id))) {
	if (AddRelation(cloneId) || FindRelation(cloneId)) {
	    register struct relation *r;

	    for (r = rd->rd_relationsTable; r; r = r->r_next) {
		SetP(r->r_leftKey, cloneId, r->r_rightKey, r->r_parameter);
	    }

	    return cloneId;
	}
    }

    return 0;
}

RELATION RemRelation(RELATION id)
{
    register struct relationDef *rd, **h;

    h = &relationsDefBase;

    for (rd = relationsDefBase; rd; rd = rd->rd_next) {
	if (rd->rd_id == id) {
	    *h = rd->rd_next;

	    while (rd->rd_relationsTable) {
		register struct relation *h = rd->rd_relationsTable->r_next;

		TCFreeMem(rd->rd_relationsTable, sizeof(struct relation));
		rd->rd_relationsTable = h;
	    }

	    TCFreeMem(rd, sizeof(*rd));

	    nrOfRelations--;

	    return id;
	}

	h = &rd->rd_next;
    }

    return 0;
}

RELATION SetP(KEY leftKey, RELATION id, KEY rightKey, PARAMETER parameter)
{
    register struct relationDef *rd = FindRelation(id);

    if (rd && CompareKey) {
	register struct relation *r;

	for (r = rd->rd_relationsTable; r; r = r->r_next) {
	    if (CompareKey(r->r_leftKey, leftKey)
		&& CompareKey(r->r_rightKey, rightKey)) {
		if (parameter != NO_PARAMETER)
		    r->r_parameter = parameter;
		return id;
	    }
	}

	if ((r = (struct relation *) TCAllocMem(sizeof(*r), 0))) {
	    r->r_next = rd->rd_relationsTable;
	    rd->rd_relationsTable = r;

	    r->r_leftKey = leftKey;
	    r->r_rightKey = rightKey;
	    r->r_parameter = parameter;

	    return id;
	}
    }

    return 0;
}

RELATION UnSet(KEY leftKey, RELATION id, KEY rightKey)
{
    register struct relationDef *rd = FindRelation(id);

    if (rd && CompareKey) {
	register struct relation *r;
	register struct relation **h;

	h = &rd->rd_relationsTable;

	for (r = rd->rd_relationsTable; r; r = r->r_next) {
	    if (CompareKey(r->r_leftKey, leftKey)
		&& CompareKey(r->r_rightKey, rightKey)) {
		*h = r->r_next;

		TCFreeMem(r, sizeof(*r));
		return id;
	    }

	    h = &r->r_next;
	}
    }

    return 0;
}

PARAMETER GetP(KEY leftKey, RELATION id, KEY rightKey)
{
    register struct relationDef *rd = FindRelation(id);

    if (rd && CompareKey) {
	register struct relation *r;

	for (r = rd->rd_relationsTable; r; r = r->r_next) {
	    if (CompareKey(r->r_leftKey, leftKey)
		&& CompareKey(r->r_rightKey, rightKey))
		return r->r_parameter;
	}
    }

    return NO_PARAMETER;
}

RELATION AskP(KEY leftKey, RELATION id, KEY rightKey, PARAMETER parameter,
	      COMPARSION comparsion)
{
    register struct relationDef *rd = FindRelation(id);

    if (rd && CompareKey) {
	register struct relation *r;

	for (r = rd->rd_relationsTable; r; r = r->r_next) {
	    if (CompareKey(r->r_leftKey, leftKey)
		&& CompareKey(r->r_rightKey, rightKey)) {
		if (comparsion && (parameter != NO_PARAMETER)) {
		    if (comparsion & CMP_EQUAL) {
			if (comparsion & CMP_HIGHER)
			    return (r->r_parameter >= parameter ? id : 0);
			else if (comparsion & CMP_LOWER)
			    return (r->r_parameter <= parameter ? id : 0);

			return (r->r_parameter == parameter ? id : 0);
		    } else if (comparsion & CMP_HIGHER)
			return (r->r_parameter > parameter ? id : 0);
		    else if (comparsion & CMP_LOWER)
			return (r->r_parameter < parameter ? id : 0);
		    else if (comparsion & CMP_NOT_EQUAL)
			return (r->r_parameter != parameter ? id : 0);

		    return 0;
		}

		return id;
	    }
	}
    }

    return 0;
}

void AskAll(KEY leftKey, RELATION id, void (*UseKey) (void *))
{
    register struct relationDef *rd = FindRelation(id);

    if (rd && CompareKey) {
	register struct relation *r;

	for (r = rd->rd_relationsTable; r; r = r->r_next) {
	    if (CompareKey(r->r_leftKey, leftKey)) {
		if (UseKey)
		    UseKey(r->r_rightKey);
	    }
	}
    }
}

void UnSetAll(KEY key, void (*UseKey) (KEY))
{
    register struct relationDef *rd;

    for (rd = relationsDefBase; rd; rd = rd->rd_next) {
	register struct relation *r;
	register struct relation **h;

	h = &rd->rd_relationsTable;

	for (r = rd->rd_relationsTable; r; r = r->r_next) {
	    if (CompareKey(r->r_leftKey, key) || CompareKey(r->r_rightKey, key)) {
		if (UseKey)
		    UseKey(key);

		*h = r->r_next;

		TCFreeMem(r, sizeof(*r));
	    }

	    h = &r->r_next;
	}
    }
}

int SaveRelations(char *file, U32 offset, U32 size, U16 disk_id)
{
    if (relationsDefBase && DecodeKey) {
	register struct relationDef *rd;
	register struct relation *r;
	register FILE *fh;
	char left[256];
	char right[256];

	if ((fh = dskOpen(file, "wb"))) {
	    fprintf(fh, "%s\r\n", REL_FILE_MARK);

	    for (rd = relationsDefBase; rd; rd = rd->rd_next) {
		if (rd->rd_id > offset) {
		    if (size && (rd->rd_id > offset + size)) {
			continue;
		    }

		    fprintf(fh, "%s\r\n", REL_TABLE_MARK);

		    fprintf(fh, "%" PRIu32 "\r\n", rd->rd_id);

		    for (r = rd->rd_relationsTable; r; r = r->r_next) {
			strcpy(left, DecodeKey(r->r_leftKey));
			strcpy(right, DecodeKey(r->r_rightKey));
			fprintf(fh, "%s\r\n%s\r\n%" PRIu32 "\r\n",
				left, right, r->r_parameter);
		    }
		}
	    }

	    dskClose(fh);

	    return 1;
	}
    }

    return 0;
}

int LoadRelations(char *file, U16 disk_id)
{
    RELATION rd;
    PARAMETER parameter;
    char buffer[256];
    char left[256];
    char right[256];
    U8 goOn;
    FILE *fh = NULL;
    U32 dummy;

    buffer[0] = '\0';
    left[0] = '\0';
    right[0] = '\0';

    if (EncodeKey) {
	if ((fh = dskOpen(file, "rb"))) {
	    dskGetLine(buffer, sizeof(buffer), fh);

	    if (strcmp(buffer, REL_FILE_MARK) == 0) {
		dskGetLine(buffer, sizeof(buffer), fh);

		while (!feof(fh) && strcmp(buffer, REL_TABLE_MARK) == 0) {
		    fscanf(fh, "%" SCNu32 "\r\n", &rd);

		    goOn = 0;
		    if (FindRelation(rd))
			goOn = 1;
		    else {
			if (AddRelation(rd))
			    goOn = 1;
		    }

		    if (goOn) {
			while (dskGetLine(left, sizeof(left), fh)) {
			    if (strcmp(left, REL_TABLE_MARK) == 0) {
				strcpy(buffer, left);
				break;
			    }

			    if (sscanf(left, "%" SCNu32, &dummy) != 1)
				break;

			    dskGetLine(right, sizeof(right), fh);

			    if (sscanf(right, "%" SCNu32, &dummy) != 1)
				break;

			    if (fscanf(fh, "%" SCNu32 "\r\n", &parameter) != 1)
				break;

			    if (!SetP
				(EncodeKey(left), rd, EncodeKey(right),
				 parameter)) {
				dskClose(fh);
				return 0;
			    }
			}
		    } else {
			dskClose(fh);
			return 0;
		    }
		}

		dskClose(fh);
		return 1;
	    }

	    dskClose(fh);
	}
    }

    return 0;
}

void RemRelations(U32 offset, U32 size)
{
    register struct relationDef *rd, *rd_next;

    for (rd = relationsDefBase; rd; rd = rd_next) {
	rd_next = rd->rd_next;

	if (rd->rd_id > offset) {
	    if (size && (rd->rd_id > offset + size)) {
		continue;
	    }

	    RemRelation(rd->rd_id);
	}
    }
}

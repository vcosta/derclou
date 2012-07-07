/****************************************************************************
  Copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/
#include "memory/memory.h"

#include "sound/buffer.h"

struct SndBuffer {
    unsigned char *data;	/* data */
    unsigned size;		/* size of data buffer */

    unsigned insertPos;		/* insert at (insertPos % size) */
    unsigned removePos;		/* remove from (removePos % size) */
};


SND_BUFFER *sndCreateBuffer(unsigned size)
{
    SND_BUFFER *buffer;
    unsigned char *data;
    
    buffer = TCAllocMem(sizeof(*buffer), false);
    data = TCAllocMem(size, true);

    buffer->data = data;
    buffer->size = size;

    sndResetBuffer(buffer);

    return buffer;
}

void sndResetBuffer(SND_BUFFER *buffer)
{
    buffer->insertPos = 0;
    buffer->removePos = 0;
}

void sndFreeBuffer(SND_BUFFER *buffer)
{
    TCFreeMem(buffer->data, buffer->size);
    TCFreeMem(buffer, sizeof(*buffer));
}

unsigned sndLenBuffer(SND_BUFFER *buffer)
{
    return buffer->insertPos - buffer->removePos;
}

unsigned sndInsertBuffer(SND_BUFFER *buffer, const void *src, unsigned srcLen)
{
    const unsigned char *psrc = src;
    unsigned len, pos;

    srcLen = min(srcLen, buffer->size - sndLenBuffer(buffer));

    pos = buffer->insertPos % buffer->size;
    len = min(srcLen, buffer->size - pos);

    /* insert to the end */
    memcpy(buffer->data + pos, psrc, len);
  
    /* insert to the start */
    memcpy(buffer->data, psrc + len, srcLen - len);
  
    buffer->insertPos += srcLen;

    return srcLen;
}

unsigned sndRemoveBuffer(SND_BUFFER *buffer, void *dst, unsigned dstLen)
{
    unsigned char *pdst = dst;
    unsigned len, pos;
    
    dstLen = min(dstLen, sndLenBuffer(buffer));
    
    pos = buffer->removePos % buffer->size;
    len = min(dstLen, buffer->size - pos);

    /* remove from the end */
    memcpy(pdst, buffer->data + pos, len);
    
    /* remove from the start */
    memcpy(pdst + len, buffer->data, dstLen - len);

    buffer->removePos += dstLen;

    return dstLen;
}


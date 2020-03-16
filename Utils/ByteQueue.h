/* 
 * File:   Queue.h
 * Author: Alexandre
 *
 * Created on 14 mars 2020, 17:31
 */

#ifndef BYTE_QUEUE_H_INCLUDED
#define	BYTE_QUEUE_H_INCLUDED

#include "../core.h"

struct ByteQueue_t;
typedef struct ByteQueue_t* ByteQueue;
   
void ByteQueue_Init(ByteQueue queue, byte_t *buffer, uint capacity);
void ByteQueue_Clear(ByteQueue queue);
void ByteQueue_Push(ByteQueue queue, byte_t value);
void ByteQueue_Pop(ByteQueue queue);
byte_t ByteQueue_Get(ByteQueue queue);
uint ByteQueue_Size(ByteQueue queue);
uint ByteQueue_Capacity(ByteQueue queue);

#endif	// !BYTE_QUEUE_H_INCLUDED

#if !defined(QUEUE_DEFINITION_INCLUDED) && defined(INCLUDE_QUEUE_DEFINITION)
#define QUEUE_DEFINITION_INCLUDED

struct ByteQueue_t{
    byte_t *pBuffer;
    uint backCursor;
    uint frontCursor;
    uint capacity;
    uint size;
};


#endif // (!QUEUE_DEFINITION_INCLUDED) && INCLUDE_QUEUE_DEFINITION
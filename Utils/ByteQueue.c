#include <xc.h>

#define INCLUDE_QUEUE_DEFINITION
#include "ByteQueue.h"

#define GET_CURSOR_INCREMENT(cursor, maxValue) (((cursor)+1)%(maxValue))

void ByteQueue_Init(ByteQueue queue, byte_t *buffer, uint capacity){
    __conditional_software_breakpoint(queue != NULL);
    __conditional_software_breakpoint(buffer != NULL);

    queue->pBuffer = buffer;
    queue->frontCursor = 0;
    queue->backCursor = 0;
    queue->size = 0;
    queue->capacity = capacity;
}

void ByteQueue_Clear(ByteQueue queue){
    __conditional_software_breakpoint(queue != NULL);

    queue->frontCursor = 0;
    queue->backCursor = 0;
    queue->size = 0;
}

void ByteQueue_Push(ByteQueue queue, byte_t value){
    __conditional_software_breakpoint(queue != NULL);

    // Si il y a de la place, on ajoute la donnée à la fin de la queue
    if(queue->size != queue->capacity)
    {
        queue->pBuffer[queue->backCursor] = value;
        queue->backCursor = GET_CURSOR_INCREMENT(queue->backCursor, queue->capacity);
        queue->size++;
    }
}

void ByteQueue_Pop(ByteQueue queue){
    __conditional_software_breakpoint(queue != NULL);

    if(queue->size != 0){
        queue->frontCursor = GET_CURSOR_INCREMENT(queue->frontCursor, queue->capacity);
        queue->size--;
    }
    
}

byte_t ByteQueue_Get(ByteQueue queue){
   __conditional_software_breakpoint(queue != NULL);
   __conditional_software_breakpoint(queue->size != 0);
   
   return queue->pBuffer[queue->frontCursor];
}

uint ByteQueue_Size(ByteQueue queue){
   __conditional_software_breakpoint(queue != NULL);

   return queue->size;
}
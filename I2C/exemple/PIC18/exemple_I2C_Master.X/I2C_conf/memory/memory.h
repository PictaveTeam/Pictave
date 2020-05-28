/****************************************************************************/
/**
*
* @file memory.h
* 
*
* This file contains functions to replace a malloc on an 8-bit microcontroller.
* This file contains a function that uses a lot of space. It is recommended
* to reduce this size according to your needs.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date	    Changes
* ----- ------ -------- ----------------------------------------------
* 1.00	epeu   07/05/20 First Release
* 
*****************************************************************************/

#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

/***************************** Include Files ********************************/

#include <string.h>

/************************** Constant Definitions ****************************/

#define MALLOC_SIZE               64 

/* MALLOC_SIZE use 2 memory arrea of 64 bytes. 
 * reduce this size according to your needs */

#define MEMORY_POINT_TAB_SIZE     10
#define MEMORY_A                   0
#define MEMORY_B                   1

#define PIC_SUCCESS                0
#define PIC_FAILED               (-1)

/**************************** Type Definitions ******************************/

typedef unsigned char u8;
typedef unsigned int u16;
typedef u8 **t_mem;
typedef struct
{
    u8 *Pointer;
    u8 PointerSize;
    u8 State;
    u8 ID;
    u8 indice;
}t_memoryPoint;

typedef struct
{
    u8 pile[MALLOC_SIZE];
    u8 use;
}t_memory;

typedef struct
{
    t_memoryPoint Tab[MEMORY_POINT_TAB_SIZE];
    u8 indice;
}t_tabPoint;

/************************** Function Prototypes *****************************/

void memory_init(void);
u8* myMalloc(u8 sizeData, u8 memoryNumber);
t_mem getMemoryPointer(u8 sizeData, u8 memoryNumber);
void getFree(t_mem Pointer, u8 memoryNumber);
int getMemorySize(t_mem Pointer, u8 memoryNumber);
int getMemoryID(t_mem Pointer, u8 memoryNumber);
void getFreeFull(u8 memoryNumber);
t_mem getMemoryFromID(u8 ID, u8 memoryNumber);



#endif // MEMORY_H_INCLUDED

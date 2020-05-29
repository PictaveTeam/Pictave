/****************************************************************************/
/**
*
* @file memory.c
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

/***************************** Include Files ********************************/

#include "memory.h"

/************************** Variable Definitions ****************************/

t_tabPoint array[2];
t_memory memory[2];
u8 *nulPointer;
u8 myMemID[2];

/****************************************************************************/

/****************************************************************************/
/**
*
* This function is used to initialize the arrays used by the memory.
*
* @param  None.
*
* @return None.
*
* @note   None.
*
*
*****************************************************************************/

void memory_init(void)
{
    nulPointer = NULL;
    getFreeFull(MEMORY_A);
    getFreeFull(MEMORY_B);
}

/****************************************************************************/
/**
*
* This function makes it possible to give an address available
* in memory for a pointer.
*
* @param  - Parameter IN:
*                   - sizeData : Number of bytes to be allocated.
*                   - memoryNumber : which memory.
*
* @return   address of the zone from which one can write in the array 
*           on a sizeData size.
*
* @note   Do not use. Use getMemoryPointer function.
*
*
*****************************************************************************/

u8* myMalloc(u8 sizeData,u8 memoryNumber)
{
    if((memory[memoryNumber].use + sizeData) > (MALLOC_SIZE-1))
            return NULL;
    memory[memoryNumber].use += sizeData;
    return &memory[memoryNumber].pile[memory[memoryNumber].use - sizeData];
}

/****************************************************************************/
/**
*
* This function provides a pointer to a memory area of sizeData size.
*
* @param  - Parameter IN:
*                   - sizeData : Number of bytes to be allocated.
*                   - memoryNumber : which memory.
*
* @return   t_mem pointer.Points to a NULL value if no space is available.
*
* @note   for write, use a u8 pointer or memcpy function.
*
*
*****************************************************************************/

t_mem getMemoryPointer(u8 sizeData, u8 memoryNumber)
{
    t_mem dP;
    u8 i;
    u8 indice;
    if(array[memoryNumber].indice >= MEMORY_POINT_TAB_SIZE)
    {
        dP = &nulPointer;
        return dP;
    }

    u8 *Pointer;
    Pointer = myMalloc(sizeData, memoryNumber);
    if(Pointer == NULL)
    {
        dP = &nulPointer;
        return dP;
    }
    myMemID[memoryNumber]++;
    if(myMemID[memoryNumber] == 0)
        myMemID[memoryNumber]++;
    for(i = 0; i < MEMORY_POINT_TAB_SIZE; i++)
    {
        if(array[memoryNumber].Tab[i].State == 0)
        {
            array[memoryNumber].Tab[i].Pointer = Pointer;
            array[memoryNumber].Tab[i].PointerSize = sizeData;
            array[memoryNumber].Tab[i].State = 1;
            array[memoryNumber].Tab[i].ID = myMemID[memoryNumber];
            array[memoryNumber].Tab[i].indice = (memory[memoryNumber].use - sizeData);
            indice = i;
            break;
        }
    }
    array[memoryNumber].indice++;
    dP = &array[memoryNumber].Tab[indice].Pointer;
    return dP;
}

/****************************************************************************/
/**
*
* allows to free the space used by the pointer of type t_mem
*
* @param  - Parameter IN:
*                   - Pointer : Point to the area to be released.
*                   - memoryNumber : which memory.
*
* @return   None.
*
* @note   If the pointer does not point to an area allocated with the 
*         getMemoryPointer function, then no action will be performed.
*
*
*****************************************************************************/

void getFree(t_mem Pointer, u8 memoryNumber)
{
    if(*Pointer != NULL)
    {
        u8 i;
        u8 *tabPoint;
        u8 tab[MALLOC_SIZE];
        memcpy(tab , memory[memoryNumber].pile, MALLOC_SIZE);
        u8 indice = 0;
        for(i = 0; i < MEMORY_POINT_TAB_SIZE ; i++)
        {
            if(Pointer == &array[memoryNumber].Tab[i].Pointer)
            {
                indice = i;
                break;
            }
        }
        for(i = 0; i < MEMORY_POINT_TAB_SIZE ; i++)
        {
            if((array[memoryNumber].Tab[i].indice > array[memoryNumber].Tab[indice].indice) && (array[memoryNumber].Tab[i].State == 1))
            {
                tabPoint = &tab[array[memoryNumber].Tab[indice].indice];
                memcpy(tabPoint , array[memoryNumber].Tab[i].Pointer, MALLOC_SIZE - array[memoryNumber].Tab[i].indice);
                memcpy(memory[memoryNumber].pile , tab, MALLOC_SIZE);
                break;
            }
        }
        for(i = 0; i < MEMORY_POINT_TAB_SIZE ; i++)
        {
            if((array[memoryNumber].Tab[i].indice > array[memoryNumber].Tab[indice].indice ) && (array[memoryNumber].Tab[i].State == 1))
            {
                array[memoryNumber].Tab[i].indice -= array[memoryNumber].Tab[indice].PointerSize;
                array[memoryNumber].Tab[i].Pointer = &memory[memoryNumber].pile[array[memoryNumber].Tab[i].indice];
            }
        }
        array[memoryNumber].Tab[indice].State = 0;
        array[memoryNumber].indice--;
        memory[memoryNumber].use -= array[memoryNumber].Tab[indice].PointerSize;
    }
}

/****************************************************************************/
/**
*
* Function to obtain the size allocated to a pointer of type t_mem
*
* @param  - Parameter IN:
*                   - Pointer.
*                   - memoryNumber : which memory.
*
* @return   returns PIC_FAILED if the pointer of type t_mem does
*           not point to a field allocated with the getMemoryPointer function.
*           Otherwise returns the allocated size.
*
* @note   None.
*
*
*****************************************************************************/

int GetMemorySize(t_mem Pointer, u8 memoryNumber)
{
    if(*Pointer == NULL)
        return PIC_FAILED;
    u8 i;
    u8 indice = MEMORY_POINT_TAB_SIZE + 1;
    for(i = 0; i < MEMORY_POINT_TAB_SIZE ; i++)
    {
        if(Pointer == &array[memoryNumber].Tab[i].Pointer)
        {
            indice = i;
            break;
        }
    }
    if(indice == (MEMORY_POINT_TAB_SIZE + 1))
        return PIC_FAILED;
    if(array[memoryNumber].Tab[indice].State == 0)
        return PIC_FAILED;
    else
        return array[memoryNumber].Tab[indice].PointerSize;
}

/****************************************************************************/
/**
*
* Function to obtain the ID of a pointer of type t_mem
*
* @param  - Parameter IN:
*                   - Pointer.
*                   - memoryNumber : which memory.
*
* @return   returns PIC_FAILED if the pointer of type t_mem does
*           not point to a field allocated with the getMemoryPointer function.
*           Otherwise returns the ID.
*
* @note   None.
*
*
*****************************************************************************/

int getMemoryID(t_mem Pointer, u8 memoryNumber)
{
    if(*Pointer == NULL)
        return PIC_FAILED;
    u8 i;
    u8 indice = MEMORY_POINT_TAB_SIZE + 1;
    for(i = 0; i < MEMORY_POINT_TAB_SIZE ; i++)
    {
        if(Pointer == &array[memoryNumber].Tab[i].Pointer)
        {
            indice = i;
            break;
        }
    }
    if(indice == (MEMORY_POINT_TAB_SIZE + 1))
        return PIC_FAILED;
    if(array[memoryNumber].Tab[indice].State == 0)
        return PIC_FAILED;
    else
        return array[memoryNumber].Tab[indice].ID;
}

/****************************************************************************/
/**
*
* allows you to completely free up a memory.
*
* @param  - Parameter IN:
*                   - memoryNumber : which memory.
*
* @return   None.
*
* @note   None.
*
*
*****************************************************************************/

void getFreeFull(u8 memoryNumber)
{
    u8 i;
    memory[memoryNumber].use = 0;
    array[memoryNumber].indice = 0;
    for(i = 0; i < MEMORY_POINT_TAB_SIZE ; i++)
        array[memoryNumber].Tab[i].State = 0;
}

/****************************************************************************/
/**
*
* This function makes it possible to obtain a pointer of type t_mem 
* on an already allocated zone.
*
* @param  - Parameter IN:
*                   - ID.
*                   - memoryNumber : which memory.
*
* @return   If the ID has not been set or the field is no longer allocated
*           to that ID, then the function points to a NULL value. Otherwise
*           it returns a pointer of type t_mem.
*
* @note   None.
*
*
*****************************************************************************/

t_mem getMemoryFromID(u8 ID, u8 memoryNumber)
{
    u8 i;
    t_mem dP;
    u8 indice = MEMORY_POINT_TAB_SIZE + 1;
    for(i = 0; i < MEMORY_POINT_TAB_SIZE ; i++)
    {
        if(ID == array[memoryNumber].Tab[i].ID)
        {
            indice = i;
            break;
        }
    }
    if(indice == (MEMORY_POINT_TAB_SIZE + 1))
    {
        dP = &nulPointer;
        return dP;
    }
    if(array[memoryNumber].Tab[indice].State == 0)
    {
        dP = &nulPointer;
        return dP;
    }
    else
    {
        dP = &array[memoryNumber].Tab[indice].Pointer;
        return dP;
    }
}

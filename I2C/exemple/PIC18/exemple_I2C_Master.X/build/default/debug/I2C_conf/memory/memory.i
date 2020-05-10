# 1 "I2C_conf/memory/memory.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "I2C_conf/memory/memory.c" 2
# 22 "I2C_conf/memory/memory.c"
# 1 "I2C_conf/memory/memory.h" 1
# 25 "I2C_conf/memory/memory.h"
# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\string.h" 1 3



# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\musl_xc8.h" 1 3
# 4 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\string.h" 2 3






# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\features.h" 1 3
# 10 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\string.h" 2 3
# 25 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\string.h" 3
# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\bits/alltypes.h" 1 3
# 122 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\bits/alltypes.h" 3
typedef unsigned size_t;
# 168 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\bits/alltypes.h" 3
typedef __int24 int24_t;
# 204 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\bits/alltypes.h" 3
typedef __uint24 uint24_t;
# 411 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\bits/alltypes.h" 3
typedef struct __locale_struct * locale_t;
# 25 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\string.h" 2 3


void *memcpy (void *restrict, const void *restrict, size_t);
void *memmove (void *, const void *, size_t);
void *memset (void *, int, size_t);
int memcmp (const void *, const void *, size_t);
void *memchr (const void *, int, size_t);

char *strcpy (char *restrict, const char *restrict);
char *strncpy (char *restrict, const char *restrict, size_t);

char *strcat (char *restrict, const char *restrict);
char *strncat (char *restrict, const char *restrict, size_t);

int strcmp (const char *, const char *);
int strncmp (const char *, const char *, size_t);

int strcoll (const char *, const char *);
size_t strxfrm (char *restrict, const char *restrict, size_t);

char *strchr (const char *, int);
char *strrchr (const char *, int);

size_t strcspn (const char *, const char *);
size_t strspn (const char *, const char *);
char *strpbrk (const char *, const char *);
char *strstr (const char *, const char *);
char *strtok (char *restrict, const char *restrict);

size_t strlen (const char *);

char *strerror (int);
# 65 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\c99\\string.h" 3
char *strtok_r (char *restrict, const char *restrict, char **restrict);
int strerror_r (int, char *, size_t);
char *stpcpy(char *restrict, const char *restrict);
char *stpncpy(char *restrict, const char *restrict, size_t);
size_t strnlen (const char *, size_t);
char *strdup (const char *);
char *strndup (const char *, size_t);
char *strsignal(int);
char *strerror_l (int, locale_t);
int strcoll_l (const char *, const char *, locale_t);
size_t strxfrm_l (char *restrict, const char *restrict, size_t, locale_t);




void *memccpy (void *restrict, const void *restrict, int, size_t);
# 25 "I2C_conf/memory/memory.h" 2
# 43 "I2C_conf/memory/memory.h"
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
    u8 pile[64];
    u8 use;
}t_memory;

typedef struct
{
    t_memoryPoint Tab[10];
    u8 indice;
}t_tabPoint;



void memory_init(void);
u8* myMalloc(u8 sizeData, u8 memoryNumber);
t_mem getMemoryPointer(u8 sizeData, u8 memoryNumber);
void getFree(t_mem Pointer, u8 memoryNumber);
int getMemorySize(t_mem Pointer, u8 memoryNumber);
int getMemoryID(t_mem Pointer, u8 memoryNumber);
void getFreeFull(u8 memoryNumber);
t_mem getMemoryFromID(u8 ID, u8 memoryNumber);
# 22 "I2C_conf/memory/memory.c" 2




t_tabPoint array[2];
t_memory memory[2];
u8 *nulPointer;
u8 myMemID[2];
# 47 "I2C_conf/memory/memory.c"
void memory_init(void)
{
    nulPointer = ((void*)0);
    getFreeFull(0);
    getFreeFull(1);
}
# 72 "I2C_conf/memory/memory.c"
u8* myMalloc(u8 sizeData,u8 memoryNumber)
{
    if((memory[memoryNumber].use + sizeData) > (64 -1))
            return ((void*)0);
    memory[memoryNumber].use += sizeData;
    return &memory[memoryNumber].pile[memory[memoryNumber].use - sizeData];
}
# 96 "I2C_conf/memory/memory.c"
t_mem getMemoryPointer(u8 sizeData, u8 memoryNumber)
{
    t_mem dP;
    u8 i;
    u8 indice;
    if(array[memoryNumber].indice >= 10)
    {
        dP = &nulPointer;
        return dP;
    }

    u8 *Pointer;
    Pointer = myMalloc(sizeData, memoryNumber);
    if(Pointer == ((void*)0))
    {
        dP = &nulPointer;
        return dP;
    }
    myMemID[memoryNumber]++;
    if(myMemID[memoryNumber] == 0)
        myMemID[memoryNumber]++;
    for(i = 0; i < 10; i++)
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
# 152 "I2C_conf/memory/memory.c"
void getFree(t_mem Pointer, u8 memoryNumber)
{
    if(*Pointer != ((void*)0))
    {
        u8 i;
        u8 *tabPoint;
        u8 tab[64];
        memcpy(tab , memory[memoryNumber].pile, 64);
        u8 indice = 0;
        for(i = 0; i < 10 ; i++)
        {
            if(Pointer == &array[memoryNumber].Tab[i].Pointer)
            {
                indice = i;
                break;
            }
        }
        for(i = 0; i < 10 ; i++)
        {
            if((array[memoryNumber].Tab[i].indice > array[memoryNumber].Tab[indice].indice) && (array[memoryNumber].Tab[i].State == 1))
            {
                tabPoint = &tab[array[memoryNumber].Tab[indice].indice];
                memcpy(tabPoint , array[memoryNumber].Tab[i].Pointer, 64 - array[memoryNumber].Tab[i].indice);
                memcpy(memory[memoryNumber].pile , tab, 64);
                break;
            }
        }
        for(i = 0; i < 10 ; i++)
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
# 211 "I2C_conf/memory/memory.c"
int GetMemorySize(t_mem Pointer, u8 memoryNumber)
{
    if(*Pointer == ((void*)0))
        return (-1);
    u8 i;
    u8 indice = 10 + 1;
    for(i = 0; i < 10 ; i++)
    {
        if(Pointer == &array[memoryNumber].Tab[i].Pointer)
        {
            indice = i;
            break;
        }
    }
    if(indice == (10 + 1))
        return (-1);
    if(array[memoryNumber].Tab[indice].State == 0)
        return (-1);
    else
        return array[memoryNumber].Tab[indice].PointerSize;
}
# 251 "I2C_conf/memory/memory.c"
int getMemoryID(t_mem Pointer, u8 memoryNumber)
{
    if(*Pointer == ((void*)0))
        return (-1);
    u8 i;
    u8 indice = 10 + 1;
    for(i = 0; i < 10 ; i++)
    {
        if(Pointer == &array[memoryNumber].Tab[i].Pointer)
        {
            indice = i;
            break;
        }
    }
    if(indice == (10 + 1))
        return (-1);
    if(array[memoryNumber].Tab[indice].State == 0)
        return (-1);
    else
        return array[memoryNumber].Tab[indice].ID;
}
# 288 "I2C_conf/memory/memory.c"
void getFreeFull(u8 memoryNumber)
{
    u8 i;
    memory[memoryNumber].use = 0;
    array[memoryNumber].indice = 0;
    for(i = 0; i < 10 ; i++)
        array[memoryNumber].Tab[i].State = 0;
}
# 316 "I2C_conf/memory/memory.c"
t_mem getMemoryFromID(u8 ID, u8 memoryNumber)
{
    u8 i;
    t_mem dP;
    u8 indice = 10 + 1;
    for(i = 0; i < 10 ; i++)
    {
        if(ID == array[memoryNumber].Tab[i].ID)
        {
            indice = i;
            break;
        }
    }
    if(indice == (10 + 1))
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

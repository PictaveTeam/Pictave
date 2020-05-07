# 1 "I2C_conf/memory/memory.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:\\Program Files (x86)\\Microchip\\xc8\\v2.10\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "I2C_conf/memory/memory.c" 2
# 1 "I2C_conf/memory/memory.h" 1



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
# 4 "I2C_conf/memory/memory.h" 2








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
    u8 pile[128];
    u8 use;
}t_memory;

typedef struct
{
    t_memoryPoint Tab[10];
    u8 indice;
}t_tabPoint;



void memory_init(void);
u8* myMalloc(u8 sizeData);
t_mem getMemoryPointer(u8 sizeData);
void getFree(t_mem Pointer);
int getMemorySize(t_mem Pointer);
int getMemoryID(t_mem Pointer);
void getFreeFull(void);
t_mem getMemoryFromID(u8 ID);
# 1 "I2C_conf/memory/memory.c" 2


t_tabPoint array;
t_memory memory;
u8 *nulPointer;
u8 myMemID;

void memory_init(void)
{
    nulPointer = ((void*)0);
    getFreeFull();
}

u8* myMalloc(u8 sizeData)
{
    if((memory.use + sizeData) > (128 -1))
            return ((void*)0);
    memory.use += sizeData;
    return &memory.pile[memory.use - sizeData];
}

t_mem getMemoryPointer(u8 sizeData)
{
    t_mem dP;
    u8 i;
    u8 indice;
    if(array.indice >= 10)
    {
        dP = &nulPointer;
        return dP;
    }

    u8 *Pointer;
    Pointer = myMalloc(sizeData);
    if(Pointer == ((void*)0))
    {
        dP = &nulPointer;
        return dP;
    }
    myMemID++;
    if(myMemID == 0)
        myMemID++;
    for(i = 0; i < 10; i++)
    {
        if(array.Tab[i].State == 0)
        {
            array.Tab[i].Pointer = Pointer;
            array.Tab[i].PointerSize = sizeData;
            array.Tab[i].State = 1;
            array.Tab[i].ID = myMemID;
            array.Tab[i].indice = (memory.use - sizeData);
            indice = i;
            break;
        }
    }
    array.indice++;
    dP = &array.Tab[indice].Pointer;
    return dP;
}

void getFree(t_mem Pointer)
{
    if(*Pointer != ((void*)0))
    {
        u8 i;
        u8 *tabPoint;
        u8 tab[128];
        memcpy(tab , memory.pile, 128);
        u8 indice = 0;
        for(i = 0; i < 10 ; i++)
        {
            if(Pointer == &array.Tab[i].Pointer)
            {
                indice = i;
                break;
            }
        }
        for(i = 0; i < 10 ; i++)
        {
            if((array.Tab[i].indice > array.Tab[indice].indice) && (array.Tab[i].State == 1))
            {
                tabPoint = &tab[array.Tab[indice].indice];
                memcpy(tabPoint , array.Tab[i].Pointer, 128 - array.Tab[i].indice);
                memcpy(memory.pile , tab, 128);
                break;
            }
        }
        for(i = 0; i < 10 ; i++)
        {
            if((array.Tab[i].indice > array.Tab[indice].indice ) && (array.Tab[i].State == 1))
            {
                array.Tab[i].indice -= array.Tab[indice].PointerSize;
                array.Tab[i].Pointer = &memory.pile[array.Tab[i].indice];
            }
        }
        array.Tab[indice].State = 0;
        array.indice--;
        memory.use -= array.Tab[indice].PointerSize;
    }
}

int GetMemorySize(t_mem Pointer)
{
    if(*Pointer == ((void*)0))
        return (-1);
    u8 i;
    u8 indice = 10 + 1;
    for(i = 0; i < 10 ; i++)
    {
        if(Pointer == &array.Tab[i].Pointer)
        {
            indice = i;
            break;
        }
    }
    if(indice == (10 + 1))
        return (-1);
    if(array.Tab[indice].State == 0)
        return (-1);
    else
        return array.Tab[indice].PointerSize;
}

int getMemoryID(t_mem Pointer)
{
    if(*Pointer == ((void*)0))
        return (-1);
    u8 i;
    u8 indice = 10 + 1;
    for(i = 0; i < 10 ; i++)
    {
        if(Pointer == &array.Tab[i].Pointer)
        {
            indice = i;
            break;
        }
    }
    if(indice == (10 + 1))
        return (-1);
    if(array.Tab[indice].State == 0)
        return (-1);
    else
        return array.Tab[indice].ID;
}

void getFreeFull(void)
{
    u8 i;
    memory.use = 0;
    array.indice = 0;
    for(i = 0; i < 10 ; i++)
        array.Tab[i].State = 0;
}

t_mem getMemoryFromID(u8 ID)
{
    u8 i;
    t_mem dP;
    u8 indice = 10 + 1;
    for(i = 0; i < 10 ; i++)
    {
        if(ID == array.Tab[i].ID)
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
    if(array.Tab[indice].State == 0)
    {
        dP = &nulPointer;
        return dP;
    }
    else
    {
        dP = &array.Tab[indice].Pointer;
        return dP;
    }
}

/* 
 * File:   Keypad_4x4.h
 * Author: Flugs
 *
 * Created on 21 février 2019, 17:21
 */

#ifndef KEYPAD_4X4_H
#define	KEYPAD_4X4_H

#include "LibKit_UserPref.h"
#include <xc.h>

#ifdef USE_KEYPAD4x4

#ifdef XC16 // 16 bit MCU
	typedef volatile unsigned int* SFRptr;
    #define _16BIT_MCU
#elif defined __XC8 
	typedef volatile unsigned char* SFRptr;
#endif

#define KP_CDRIVE_RLOW
#if !(defined KP_RDRIVE_CLOW || defined KP_RDRIVE_CHIGH || defined KP_CDRIVE_RLOW || defined KP_CDRIVE_RHIGH)
#error No Keypad layout detected, please define a Keypad layout :
#error KP_RDRIVE_CLOW : Keypad rows drived by PIC I/O, collumns pulled LOW 
#error KP_RDRIVE_CHIGH : Keypad rows drived by PIC I/O, collumns pulled HIGH 
#error KP_CDRIVE_RLOW : Keypad collumns drived by PIC I/O, rows pulled LOW 
#error KP_CDRIVE_RHIGH : Keypad collumns drived by PIC I/O, rows pulled HIGH
#endif
    
#define KP_ROW0 0
#define KP_ROW1 1
#define KP_ROW2 2
#define KP_ROW3 3
#define KP_COL0  4
#define KP_COL1  5
#define KP_COL2  6
#define KP_COL3  7
    

typedef enum
{
    BIT0 = 0x01,
    BIT1 = 0x02,
    BIT2 = 0x04,
    BIT3 = 0x08,
    BIT4 = 0x10,
    BIT5 = 0x20,
    BIT6 = 0x40,
    BIT7 = 0x80
    
    #ifdef _16BIT_MCU
    ,BIT8 =  0x0100,
    BIT9 =  0x0200,
    BIT10 = 0x0400,
    BIT11 = 0x0800,
    BIT12 = 0x1000,
    BIT13 = 0x2000,
    BIT14 = 0x4000,
    BIT15 = 0x8000
    #endif
    
}RegisterBit;

typedef struct{
    SFRptr pinTris;
    SFRptr pinPort;
    RegisterBit pinBit;
}PinInfo;

typedef struct {
    PinInfo pins[8];
    char keysName[4][4]; // [Ligne][Colonne]
    unsigned int keysState; 
}Keypad4x4;

typedef unsigned char uchar;
typedef unsigned int uint;

void Keypad4x4_Init(Keypad4x4* keypad);
unsigned int Keypad4x4_Read(Keypad4x4* keypad);
char Keypad4x4_GetKeyState(Keypad4x4* keypad, const char c);
char Keypad4x4_GetKeyState_ByPos(Keypad4x4* keypad, uchar col, uchar row);
char Keypad4x4_GetKeys(Keypad4x4* const keypad, char* keyArray, uchar maxKeys);

#endif // defined USE_KEYPAD4x4
#endif	/* KEYPAD_4X4_H */


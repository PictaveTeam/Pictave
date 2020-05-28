/* 
 * File:   main.c
 * Author: Erwan PEUREUX
 * IUT de POITIERS
 * Created on 25 septembre 2018, 16:00
 */
#include "xc.h"
#include "config.h"
#include "I2C.h"

// CONFIG1

#pragma config POSCMOD = XT             // Primary Oscillator Select (XT Oscillator mode selected)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL           // Oscillator Select (Primary Oscillator with PLL module (HSPLL, ECPLL))
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)

#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Windowed Watchdog Timer enabled; FWDTEN must be 1)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator/debugger uses EMUC2/EMUD2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF              // JTAG Port Not Enable (JTAG port is not enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#define S0   LATDbits.LATD0
#define S1   LATDbits.LATD1
#define S2   LATDbits.LATD2
#define S3   LATDbits.LATD3
#define S4   LATEbits.LATE0
#define S5   LATEbits.LATE1
#define S6   LATEbits.LATE2
#define S7   LATEbits.LATE3


#define C0 PORTEbits.RE5
#define C1 PORTEbits.RE6
#define C2 PORTEbits.RE7
#define C3 PORTBbits.RB11
#define C4 PORTBbits.RB12
#define C5 PORTBbits.RB13
#define C6 PORTCbits.RC13
#define C7 PORTCbits.RC14

#define LATCH_DAC   LATFbits.LATF6

#define EEPROM 0xAE

int main(int argc, char** argv) {
    
    int Status;
    u8 *Buffer;
    u16 BufferSize;
    u8 adresse;
    u8 data[2]={0x10, 0x21};
    initPictave();
    Status=I2C_Init(MASTER | INTERRUPT);
    if((Status == PIC_FAILED) || (Status == I2C_SEND_OVERFLOW))
        S0 = 1;
    
    Status = I2C_Write(EEPROM, data, sizeof(data), 10);
    if(Status == PIC_FAILED)
        S1 = 1;
    data[0] = 0x11;
    Status = I2C_Write(EEPROM, data, sizeof(data), 10);
    if((Status == PIC_FAILED) || (Status == I2C_SEND_OVERFLOW))
        S2 = 1;
    data[0] = 0x10;
    Status = I2C_Read_Register_Request(EEPROM, data, 1, 1, 10);
    if((Status == PIC_FAILED) || (Status == I2C_SEND_OVERFLOW))
        S3 = 1;
    
    
    while(1)
    {
        Buffer = I2C_Read_Buffer(&adresse, &BufferSize);
        if(Buffer!=NULL)
        {
            if(Buffer[0]==0x21)
                S4 = 1;
            I2C_Free();
        }
    }
    
}


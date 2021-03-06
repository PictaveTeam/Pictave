/*
 * File:   main.c
 * Author: epeureux
 *
 * Created on 6 avril 2020, 21:29
 */
// PIC18F45K22 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config FOSC = HSHP      // Oscillator Selection bits (HS oscillator (high power > 16 MHz))
#pragma config PLLCFG = ON      // 4X PLL Enable (Oscillator multiplied by 4)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock is always enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power-up Timer Enable bit (Power up timer disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<5:0> pins are configured as digital I/O on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = OFF     // HFINTOSC Fast Start-up (HFINTOSC output and ready status are delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTD2   // ECCP2 B output mux bit (P2B is on RD2)
#pragma config MCLRE = EXTMCLR  // MCLR Pin Enable bit (MCLR pin enabled, RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled if MCLRE is also 1)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection Block 0 (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection Block 1 (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection Block 2 (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection Block 3 (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

#define LED0 LATDbits.LD0

#define EEPROM_A              (0xA0)
#define EEPROM_B              (0xAE)

#include <xc.h>
#include "interrupt.h"
#include "I2C.h"

void main(void) {
    OSCCONbits.IRCF = 0b111; // 16 Mhz Clock
    TRISA  = 0xFF;    // PortA en entr�e
    TRISC  = 0b10111111;
	TRISD  = 0x00;    // PortD en sortie
	TRISE  = 0x00;    // ..................................................................
	TRISB  = 0x00;    // PortB en sortie 
    ANSELA = 0x01;
    ANSELC = 0;
    ANSELE = 0;
    ANSELB = 0;
    
    
    WPUB=0;
    INTCON2bits.NOT_RBPU=1;
    INTCONbits.GIE=1;
   
    
    LATD=0;
    LATE=0;
    LATB=0;
    
    int Status;
    
    ISR_Init();
    Status = I2C_Init(MASTER | INTERRUPT);
    if (Status == PIC_FAILED)
    {
        LATD = 0xFF;
        return;
    }
    ISR_ConnectFunction(&I2C_ISR);
    
    u8 bufferSize;
    u8 adresse;
    t_mem buffer;
    u8 data[3]={0x12, 0x13, 0x21};
    u8* point;
    
    Status = I2C_Write(EEPROM_A, data, 3, 10);
    if (Status == PIC_FAILED)
    {
        LATD = 0xFF;
        return;
    }
    
    data[1] = 0x14;
    data[2] = 0x32;
    Status = I2C_Write(EEPROM_B, data, 3, 10);
    if (Status == PIC_FAILED)
    {
        LATD = 0xFF;
        return;
    }
    data[1] = 0x13;
    Status = I2C_Read_Register_Request(EEPROM_A, data, 2, 1, 10);
    if (Status == PIC_FAILED)
    {
        LATD = 0xFF;
        return;
    }
    data[1] = 0x14;
    Status = I2C_Read_Register_Request(EEPROM_B, data, 2, 1, 10);
    if (Status == PIC_FAILED)
    {
        LATD = 0xFF;
        return;
    }
    
    while(1)
    {
        buffer = I2C_Read_Buffer(&adresse, &bufferSize);
        if(*buffer != NULL)
        {
            point = *buffer;
            if(adresse == EEPROM_A)
            {
                if(point[0]==0x21)
                    LATD = LATD | 2;
            }
            if(adresse == EEPROM_B)
            {
                if(point[0]==0x32)
                    LATD = LATD | 4;
            }
            I2C_Free();
        }
    }
    
    return;
}

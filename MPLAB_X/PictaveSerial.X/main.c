// PIC24FJ64GA006 Configuration Bit Settings

// 'C' source line config statements

// CONFIG2
#pragma config POSCMOD = HS             // Primary Oscillator Select (HS Oscillator mode selected)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRI              // Oscillator Select (Primary Oscillator (XT, HS, EC))
#pragma config IESO = ON                // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) enabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx1               // Comm Channel Select (Emulator/debugger uses EMUC1/EMUD1)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = ON              // JTAG Port Enable (JTAG port is enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include <xc.h>
#include "../../Uart/Uart.h"

void XBee_WriteCmd(Uart xbee, Uart log, const char* cmd){
    Uart_WriteString(xbee, cmd);
    Uart_WriteString(log, cmd);
    Uart_WriteByte(log, '[');

    byte_t val = 0;
    while(val != '\r'){
        if(Uart_ReadByte(xbee, &val)){
            if(val == '\r'){
               Uart_WriteByte(log, ']');
            }
            Uart_WriteByte(log, val);
        }
    }
}

void XBee_WaitForConnect(Uart xbee, Uart log){
    byte_t val[10];
    val[0] = 0;
    
    Uart_WriteString(xbee, "ATAI\r");
    Uart_WriteString(log, "ATAI\r");
    Uart_WriteByte(log, '[');

    int cpt = 0;

    while(val[0] != '0'){
        
        if(Uart_ReadByte(xbee, &val[cpt])){
            if(val[cpt] == '\r'){
               Uart_WriteByte(log, ']');
            }
            Uart_WriteByte(log, val[cpt]);

            if(val[cpt] == '\r'){
                
                for(int i=0; i < 0xFFFF; i++);
                
                Uart_WriteString(xbee, "ATAI\r");
                Uart_WriteString(log, "ATAI\r");
                Uart_WriteByte(log, '[');
                cpt = 0;
            }else{
                cpt++;
            }
        }
    }
}

int main(void){
    
    Uart m_XBee = Uart2;
    Uart m_UB232 = Uart1;

    TRISD = 0;
    PORTD = 0;
    TRISE = 0;
    PORTE = 0;
    
    Uart_Start(m_UB232, 9600);
    Uart_Start(m_XBee, 9600);


        
    PORTE = 1;

    XBee_WriteCmd(m_XBee, m_UB232, "+++");
    //XBee_WriteCmd(m_XBee, m_UB232, "ATD00\r");

   // XBee_WriteCmd(m_XBee, m_UB232, "ATIDPluplume-Box\r");
    XBee_WriteCmd(m_XBee, m_UB232, "ATEE2\r"); // WPA2
    XBee_WriteCmd(m_XBee, m_UB232, "ATPKmotdepasse_pluplume\r");
    XBee_WriteCmd(m_XBee, m_UB232, "ATIDPluplume-Box\r");
    XBee_WriteCmd(m_XBee, m_UB232, "ATAC\r");

    //XBee_WriteCmd(m_XBee, m_UB232, "ATID\r");
    XBee_WaitForConnect(m_XBee, m_UB232);
    
    XBee_WriteCmd(m_XBee, m_UB232, "ATIP1\r"); // Mode TCP
    XBee_WriteCmd(m_XBee, m_UB232, "ATDL192.168.1.28\r"); // Définit l'IP
    
    XBee_WriteCmd(m_XBee, m_UB232, "ATAC\r");

    //XBee_WriteCmd(m_XBee, m_UB232, "ATID\r");
    XBee_WaitForConnect(m_XBee, m_UB232);
    
    XBee_WriteCmd(m_XBee, m_UB232, "ATMY\n"); // On quitte le mode commande  
    XBee_WriteCmd(m_XBee, m_UB232, "ATMY\n"); // On quitte le mode commande    
    XBee_WriteCmd(m_XBee, m_UB232, "ATCN\n"); // On quitte le mode commande
    
    byte_t val = 0;
    PORTE = 0x2;

    while(true){
        if(Uart_ReadByte(m_XBee, &val)){
          PORTE = 0x0;
        }
       // XBee_WriteCmd
    
        //Uart_Read(m_XBee, valArray, 3);
        
      //  nbDispo = Uart_Available(m_Uart);
       // Uart_Read(m_Uart, valArray, 10);
      //  Uart_Write(m_Uart, valArray, 10);
       /*if(Uart_ReadByte(m_Uart, &val)){
           // Uart_WriteByte(m_Uart, val);
           U1TXREG = val;
       }*/
    }
    
    return 0;
}
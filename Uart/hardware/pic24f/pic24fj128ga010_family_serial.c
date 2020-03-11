/*!
\file pic18f45k22_serial.dev 
\author Alexandre Brunet
\date 20/02/2019
*
* THIS FILE MUST NOT BE ADDED TO COMPILATION 
* THIS FILE WILL BE INCLUDED BY Uart.c 
*/

#include "../../Uart_t.h"
#include "../../Uart.h"

#if UART1_ENABLE == 1

static float uart1_start(uint baudrate);
static void uart1_stop(void);

static byte_t uart1_rxBuffer[UART1_RX_BUFFER_SIZE];
static byte_t uart1_txBuffer[UART1_TX_BUFFER_SIZE];

static struct Uart_t uart1 = {
    .rxBuffer = uart1_rxBuffer,
    .txBuffer = uart1_txBuffer,
    .RX_BUFFER_SIZE = UART1_TX_BUFFER_SIZE,
    .TX_BUFFER_SIZE = UART1_RX_BUFFER_SIZE,
    .start_fnc = uart1_start,
    .stop_fnc = uart1_stop,
};;

Uart Uart1 = &uart1;

// Initialisation des registres de l'uart1
static float uart1_start(uint baudrate)
{ 
   /*-------------------------------------*
    * Configuration du mode de RX et TX   * 
    *-------------------------------------*/   
    TRISFbits.TRISF2 = PINMODE_INPUT; // rx1
    TRISFbits.TRISF3 = PINMODE_OUTPUT; // tx1
        
   /*-------------------------------------*
    * Calcul du baudrate, et de l'erreur  * 
    *-------------------------------------*/

    // U1BRG = (fcy)/(16*baudrate)-1; <- erreur !! (16*baudrate) overflow, ne loge pas dans 16 bits
    float fbaud = baudrate;
    U1BRG = (FCY)/(fbaud*16.0f)-1;
    float realBaud = ((float)FCY)/((float)(16*(U1BRG+1)));
    float thBaud = (float) FCY;
    float error = (realBaud-thBaud)/thBaud;    
    
   /*---------------------------------------*
    * Config de l'uart et des interruptions * 
    *---------------------------------------*/
    U1MODEbits.STSEL = 0;       // un bit de stop 
    U1MODEbits.PDSEL = 0b00;    // 8 bits, pas de parité
    U1MODEbits.BRGH = 0;        // mode basse vitesse 
    
    // Netoyage des flags d'interruption
    IFS0bits.U1RXIF = 0; 
    IFS0bits.U1TXIF = 0;
    // Activation des interruptions
    IEC0bits.U1TXIE = 1;
    IEC0bits.U1RXIE = 1;
    
    // interruption quand toute la pile d'envoi vient d'être vidée
    U1STAbits.UTXISEL1 = 1;
    U1STAbits.UTXISEL0 = 0;
    
    /*-------------------------------------*
     * Activation de l'uart                * 
     *-------------------------------------*/
    U1MODEbits.UARTEN = 1; // Active l'UART 1
    U1TXREG = 0;
    U1STAbits.UTXEN = 1; // provoque un envoi, donc une interruption non voulue

    return error;
}

static void uart1_stop(void)
{
   /*-------------------------------------*
    * Désactivation des interruptions     * 
    *-------------------------------------*/
    IEC0bits.U1TXIE = 0; // Désactive interruption TX 
    IEC0bits.U1RXIE = 0; // Désactive interruption TX 
    
   /*-------------------------------------*
    * Désactivation de l'UART             * 
    *-------------------------------------*/
    U1STAbits.UTXEN = 0; // Désactive l'envoi des données
    U1MODEbits.UARTEN = 0; // Désactive l'UART
}




void __attribute__((interrupt, auto_psv)) _U1RXInterrupt ( void ){
    IFS0bits.U1RXIF = 0;
    Uart1->onRxInterrupt(Uart1, (byte_t) U1RXREG);
   // byte_t x = U1RXREG;
   // PORTE = x;
	/*while(U1STAbits.URXDA){ // Clear the RX Fifo (FIFO is 4 bytes deep)
        // on place la nouvelle valeur dans le buffer
        // et on incrémente le curseur
        sUart->rxBuffer[sUart->receiveCursor] = (byte_t) U1RXREG;
        INCREMENT_CURSOR(sUart->receiveCursor, UART_RX_BUFFER_SIZE);
	}*/
}
	
void __attribute__((interrupt, auto_psv)) _U1TXInterrupt ( void ){
	// At least one character is free in TX fifo
	IFS0bits.U1TXIF = 0;	
    
    TRISD = 0x0;
    PORTD++;
    // On remplit le FIFO TX si possible	
	/*while((sUart->txPendingBytes > 0) && !U1STAbits.UTXBF){ // Tant qu'il y a des donn?es en buffer, et que la FIFO TX n'est pas pleine on met les donn?es dans la pile
		U1TXREG = sUart->txBuffer[sUart->sendCursor]; // On écrit le caractère sur le port série
        INCREMENT_CURSOR(sUart->sendCursor, UART_TX_BUFFER_SIZE); // On incrémente la tête d'écriture
        sUart->txPendingBytes -= 1;
	}*/
}

/* On ne fait rien si il y a une erreur */	
void __attribute__((interrupt, auto_psv)) _U1ErrInterrupt(void){
    IFS4bits.U1ERIF = 0;
}

#endif //UART1_ENABLE == 1

#if USE_UART1 != 0

	#if defined __24FJ64GA006_H
		#warning [Serial Library] Building Serial for PIC24FJ64GA006
	#elif defined __24FJ96GA006_H 
		#warning [Serial Library] Building Serial for PIC24FJ96GA006
	#elif  defined __24FJ128GA006_H 
		#warning [Serial Library] Building Serial for PIC24FJ128GA006
    #elif defined __24FJ64GA008_H 
		#warning [Serial Library] Building Serial for PIC24FJ64GA008
	#elif  defined __24FJ96GA008_H 
		#warning [Serial Library] Building Serial for PIC24FJ96GA008
	#elif  defined __24FJ128GA008_H
		#warning [Serial Library] Building Serial for PIC24FJ128GA008
    #elif defined __24FJ64GA010_H 
		#warning [Serial Library] Building Serial for PIC24FJ64GA010
	#elif  defined __24FJ96GA010_H 
		#warning [Serial Library] Building Serial for PIC24FJ96GA010
	#elif  defined __24FJ128GA010_H
		#warning [Serial Library] Building Serial for PIC24FJ128GA010
	#endif
		
	static float Serial_DeviceInit(float baudrate){
		//----------------------[CALCUL DU BAUDRATE]----------------------------
		float baudL, baudH, errorL, errorH;
		uint bgrL, bgrH; 
		bgrL =  (uint) FloatToInt(((float) FCY / (16.0f * baudrate)) - 1.0f);
		bgrH =  (uint) FloatToInt(((float) FCY / (4.0f  * baudrate)) - 1.0f);
		baudL = (float) FCY / (16.0f * (bgrL+1));
		baudH = (float) FCY / (4.0f * (bgrH+1));
		errorL = (baudL - baudrate) / baudrate;
		errorH = (baudH - baudrate) / baudrate;
		if(errorL < 0) { errorL = -errorL; }
		if(errorH < 0) { errorH = -errorH; }

		TRISFbits.TRISF2 = 1; // RX2 as OUTPUT
		TRISFbits.TRISF3 = 1; // TX2 as OUTPUT

		//-------------------------[UART2MODE]------------------------------    
		U1MODE = 0b1000000000000000; // UartON, 8bits no parity, 1 stop and others as default
		if(errorL <= errorH){ // Low baudrate mode
			U1BRG = bgrL;
			U1MODEbits.BRGH = 0; // Low speed baud rate mode
		}else{  // High baudrate mode
			U1BRG = bgrH;
			U1MODEbits.BRGH = 1; // Low speed baud rate mode
			baudL = baudH;
		}
		// ErrorL is now the final error
		//---------------------[RCSTA1 + Interrrupts]---------------------------
		U1STAbits.UTXISEL0 = 1; // Interrupt when character shifted out of TransmitShifRegister
		U1STAbits.UTXISEL1 = 0; // Interrupt when character shifted out of TransmitShifRegister
		U1STAbits.UTXINV = 0; // Idls state = 1
		U1STAbits.UTXBRK = 0;
		U1STAbits.UTXEN = 0;
		U1STAbits.URXISEL = 0b00;
		U1STAbits.ADDEN = 0;
		
		IEC0bits.U1RXIE = 1;           //  Interrupt on RX2
		IEC0bits.U1TXIE = 1;         
		IFS0bits.U1TXIF = 0;
		IPC2bits.U1RXIP = 0b101;      //  Priorit� 5 sur la reception (prioritaires sur l'UART1)
		IPC3bits.U1TXIP = 0b101;
		return baudL;
	}
	static void Serial_DeviceEnableTXInterrupt(void){
		if(U1STAbits.UTXEN  == 0){ // Pas d'envoi en cours, on active l'envoi
			U1STAbits.UTXEN = 1; // Enable TX2 , it will generate TX interrupt
		}
	}
	
	void __attribute__((interrupt, auto_psv)) _U1RXInterrupt ( void ){
		IFS0bits.U1RXIF = 0;
		while(U1STAbits.URXDA){ // Clear the RX Fifo (FIFO is 4 bytes deep)
			incrementCursor(&rxCursor1);
			rxBuffer1[rxCursor1] = U1RXREG;
		}
	}
	
	void __attribute__((interrupt, auto_psv)) _U1TXInterrupt ( void ){
		// At least one character is free in TX fifo
		IFS0bits.U1TXIF = 0;
		
		if(!Serial_WriteAvailable()){ // Tous les caract�res ont �t� envoy�s
			U1STAbits.UTXEN = 0; // Disable TX2
			return;
		}
		
		while(Serial_WriteAvailable() && !U1STAbits.UTXBF){ // Tant qu'il y a des donn�es en buffer, et que la FIFO TX n'est pas pleine on met les donn�es dans la pile
			incrementCursor(&txCursor1); // On incr�mente la t�te d'�criture
			U1TXREG = txBuffer1[txCursor1]; // On �crit le caract�re sur le port s�rie
		}
	}
	
	void __attribute__((interrupt, auto_psv)) _U1ErrInterrupt(void){
		
	}
#endif

#if USE_UART2 != 0
	#if defined __24FJ64GA006_H
		#warning [Serial Library] Building Serial2 for PIC24FJ64GA006
	#elif defined __24FJ96GA006_H 
		#warning [Serial Library] Building Serial2 for PIC24FJ96GA006
	#elif  defined __24FJ128GA006_H 
		#warning [Serial Library] Building Serial2 for PIC24FJ128GA006
    #elif defined __24FJ64GA008_H 
		#warning [Serial Library] Building Serial2 for PIC24FJ64GA008
	#elif  defined __24FJ96GA008_H 
		#warning [Serial Library] Building Serial2 for PIC24FJ96GA008
	#elif  defined __24FJ128GA008_H
		#warning [Serial Library] Building Serial2 for PIC24FJ128GA008
    #elif defined __24FJ64GA010_H 
		#warning [Serial Library] Building Serial2 for PIC24FJ64GA010
	#elif  defined __24FJ96GA010_H 
		#warning [Serial Library] Building Serial2 for PIC24FJ96GA010
	#elif  defined __24FJ128GA010_H
		#warning [Serial Library] Building Serial2 for PIC24FJ128GA010
	#endif

	static float Serial2_DeviceInit(float baudrate)
	{
		//----------------------[CALCUL DU BAUDRATE]----------------------------
		float baudL, baudH, errorL, errorH;
		uint bgrL, bgrH; 
		bgrL =  (uint) FloatToInt(((float) FCY / (16.0f * baudrate)) - 1.0f);
		bgrH =  (uint) FloatToInt(((float) FCY / (4.0f  * baudrate)) - 1.0f);
		baudL = (float) FCY / (16.0f * (bgrL+1));
		baudH = (float) FCY / (4.0f * (bgrH+1));
		errorL = (baudL - baudrate) / baudrate;
		errorH = (baudH - baudrate) / baudrate;
		if(errorL < 0) { errorL = -errorL; }
		if(errorH < 0) { errorH = -errorH; }

		TRISFbits.TRISF4 = 1; // RX2 as OUTPUT
		TRISFbits.TRISF5 = 1; // TX2 as OUTPUT

		//-------------------------[UART2MODE]------------------------------    
		U2MODE = 0b1000000000000000; // UartON, 8bits no parity, 1 stop and others as default
		if(errorL <= errorH){ // Low baudrate mode
			U2BRG = bgrL;
			U2MODEbits.BRGH = 0; // Low speed baud rate mode
		}else{  // High baudrate mode
			U2BRG = bgrH;
			U2MODEbits.BRGH = 1; // Low speed baud rate mode
			baudL = baudH;
			
		}
		// ErrorL is now the return value of the final error
		
		//---------------------[RCSTA1 + Interrrupts]---------------------------
		U2STAbits.UTXISEL0 = 1; // Interrupt when character shifted out of TransmitShifRegister
		U2STAbits.UTXISEL1 = 0; // Interrupt when character shifted out of TransmitShifRegister
		U2STAbits.UTXINV = 0; // Idls state = 1
		U2STAbits.UTXBRK = 0;
		U2STAbits.UTXEN = 0;
		U2STAbits.URXISEL = 0b00;
		U2STAbits.ADDEN = 0;
		
		IEC1bits.U2RXIE = 1;           //  Interrupt on RX2
		IEC1bits.U2TXIE = 1;         
		IFS1bits.U2TXIF = 0;
		IPC7bits.U2RXIP = 0b101;      //  Priorit� 5 sur la reception (prioritaires sur l'UART1)
		IPC7bits.U2TXIP = 0b101;
		
		return baudL;
	}
	
	static void Serial2_DeviceEnableTXInterrupt(void)
	{
		if(U2STAbits.UTXEN  == 0){ // Pas d'envoi en cours, on active l'envoi
			U2STAbits.UTXEN = 1; // Enable TX2 , it will generate TX interrupt
		}
	}
	
	void __attribute__((interrupt, auto_psv)) _U2RXInterrupt ( void )
	{
		IFS1bits.U2RXIF = 0;
		while(U2STAbits.URXDA){ // Clear the RX Fifo (FIFO is 4 bytes deep)
			incrementCursor2(&rxCursor2);
			rxBuffer2[rxCursor2] = U2RXREG;
		}
	}
	
	void __attribute__((interrupt, auto_psv)) _U2TXInterrupt ( void )
	{
		// At least one character is free in TX fifo
		IFS1bits.U2TXIF = 0;
		
		if(!Serial2_WriteAvailable()){ // Tous les caract�res ont �t� envoy�s
			U2STAbits.UTXEN = 0; // Disable TX2
			return;
		}
		
		while(Serial2_WriteAvailable() && !U2STAbits.UTXBF){ // Tant qu'il y a des donn�es en buffer, et que la FIFO TX n'est pas pleine on met les donn�es dans la pile
			incrementCursor2(&txCursor2); // On incr�mente la t�te d'�criture
			U2TXREG = txBuffer2[txCursor2]; // On �crit le caract�re sur le port s�rie
		}
	}
#endif // UART2
/*!
\file pic18f45k22_serial.dev 
\author Alexandre Brunet
\date 20/02/2019
*
*This file cannot be used alone.
*This file must be included in Serial.c
*/


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
		IPC2bits.U1RXIP = 0b101;      //  Priorité 5 sur la reception (prioritaires sur l'UART1)
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
		
		if(!Serial_WriteAvailable()){ // Tous les caractères ont été envoyés
			U1STAbits.UTXEN = 0; // Disable TX2
			return;
		}
		
		while(Serial_WriteAvailable() && !U1STAbits.UTXBF){ // Tant qu'il y a des données en buffer, et que la FIFO TX n'est pas pleine on met les données dans la pile
			incrementCursor(&txCursor1); // On incrémente la tête d'écriture
			U1TXREG = txBuffer1[txCursor1]; // On écrit le caractère sur le port série
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
		IPC7bits.U2RXIP = 0b101;      //  Priorité 5 sur la reception (prioritaires sur l'UART1)
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
		
		if(!Serial2_WriteAvailable()){ // Tous les caractères ont été envoyés
			U2STAbits.UTXEN = 0; // Disable TX2
			return;
		}
		
		while(Serial2_WriteAvailable() && !U2STAbits.UTXBF){ // Tant qu'il y a des données en buffer, et que la FIFO TX n'est pas pleine on met les données dans la pile
			incrementCursor2(&txCursor2); // On incrémente la tête d'écriture
			U2TXREG = txBuffer2[txCursor2]; // On écrit le caractère sur le port série
		}
	}
#endif // UART2
/*!
\file pic18f45k22_serial.dev 
\author Alexandre Brunet
\date 20/02/2019
*
*This file cannot be used alone.
*This file must be included in Serial.c
*/

#if USE_UART1 != 0
	#ifdef _PIC18LF23K22_H_
		#info [Serial Library] Building Serial for PIC18LF23K22
	#elif defined _PIC18LF24K22_H_ 
		#info [Serial Library] Building Serial for PIC18LF24K22
	#elif defined _PIC18LF25K22_H_
		#info [Serial Library] Building Serial for PIC18LF25K22
	#elif defined _PIC18LF26K22_H_
		#info [Serial Library] Building Serial for PIC18LF26K22
    #elif defined _PIC18LF43K22_H_ 
		#info [Serial Library] Building Serial for PIC18LF43K22
	#elif defined _PIC18LF44K22_H_ 
		#info [Serial Library] Building Serial for PIC18LF44K22
	#elif defined _PIC18LF45K22_H_ 
		#info [Serial Library] Building Serial for PIC18LF45K22
	#elif defined _PIC18LF46K22_H_ 
		#info [Serial Library] Building Serial for PIC18LF46K22
    #elif defined _PIC18F23K22_H_  
		#info [Serial Library] Building Serial for PIC18LF23K22
	#elif defined _PIC18F24K22_H_  
		#info [Serial Library] Building Serial for PIC18LF24K22
	#elif defined _PIC18F25K22_H_  
		#info [Serial Library] Building Serial for PIC18LF25K22
	#elif defined _PIC18F26K22_H_ 
		#info [Serial Library] Building Serial for PIC18LF26K22
    #elif defined _PIC18F43K22_H_
		#info [Serial Library] Building Serial for PIC18LF43K22
	#elif defined _PIC18F44K22_H_  
		#info [Serial Library] Building Serial for PIC18LF24K22
	#elif defined _PIC18F45K22_H_  
		#info [Serial Library] Building Serial for PIC18LF45K22
	#elif defined _PIC18F46K22_H_ 
		#info [Serial Library] Building Serial for PIC18LF46K22
	#endif
	
	static float Serial_DeviceInit(float baudrate)
	{
		//----------------------[CALCUL DU BAUDRATE]----------------------------
		float baudL, baudH, errorL, errorH;
		uint bgrL, bgrH; 
		bgrL =  (uint) FloatToInt(((float)FOSC / baudrate / 64.0f) - 1.0f);
		bgrH =  (uint) FloatToInt(((float)FOSC / baudrate / 16.0f) - 1.0f);
		baudL = (float) FOSC / (64.0f * (bgrL+1));
		baudH = (float) FOSC / (16.0f * (bgrH+1));
		errorL = (baudL - baudrate) / baudrate;
		errorH = (baudH - baudrate) / baudrate;
		if(errorL < 0) { errorL = -errorL; }
		if(errorH < 0) { errorH = -errorH; }


		ANSELCbits.ANSC6 = 0;
		ANSELCbits.ANSC7 = 0;
		TRISCbits.RC6 = 1; // TX1 as input 
		TRISCbits.RC7 = 1; // RX1 as input 

		//------------------------[TXSTA1 + SPBRG1]-----------------------------           
		TXSTA1bits.TX9 = 0; // 8 bit mode
		TXSTA1bits.TXEN = 1; // Enable TX
		TXSTA1bits.SYNC = 0; // Asynchronous mode
		if(errorL <= errorH){ // Low baudrate mode
			SPBRG1 = bgrL;
			TXSTA1bits.BRGH = 0; // Low speed baud rate mode
		}else{  // High baudrate mode
			SPBRG1 = bgrH;
			TXSTA1bits.BRGH = 1; // Low speed baud rate mode
			baudL = baudH;
		}
		
		//---------------------[RCSTA1 + Interrrupts]---------------------------
		RCSTA1bits.RX9 = 0; // 8 bit reception mode
		RCSTA1bits.CREN = 1; // Enable RX
		PIE1bits.RC1IE = 1; // Enable RX interrupt
		PIE1bits.TX1IE = 0; // Disable TX interrupt, it will be enabled when writing data to TX
		RCSTA1bits.SPEN = 1; // Enable UART1 
		
		return baudL;
	}
	
	static void Serial_DeviceEnableTXInterrupt(void)
	{
		if(PIE1bits.TX1IE == 0){ // Pas d'envoi en cours, on active l'interruption
			PIE1bits.TX1IE = 1; // Enable TX interrupt
		}
	}
	
	void Serial_ISR(void)
	{
		if(PIR1bits.RC1IF){ // Réception d'un caractère
			incrementCursor(&rxCursor1);
			rxBuffer1[rxCursor1] = RCREG1;
		}

		if(PIR1bits.TX1IF && PIE1bits.TX1IE == 1)
		{ // UART Libre pour l'envoi
			if(Serial_WriteAvailable()){ // Des caractères sont en attente
				incrementCursor(&txCursor1); // On incrémente la tête d'écriture
				TXREG1 = txBuffer1[txCursor1]; // On écrit le caractère sur le port série
			}else{ // Tous les caractères ont été envoyés
				PIE1bits.TX1IE = 0; // Disable TX interrupt
			}
		}
	}
#endif // UART1

#if USE_UART2 != 0

	#ifdef _PIC18LF23K22_H_
		#info [Serial Library] Building Serial2 for PIC18LF23K22
	#elif defined _PIC18LF24K22_H_ 
		#info [Serial Library] Building Serial2 for PIC18LF24K22
	#elif defined _PIC18LF25K22_H_
		#info [Serial Library] Building Serial2 for PIC18LF25K22
	#elif defined _PIC18LF26K22_H_
		#info [Serial Library] Building Serial2 for PIC18LF26K22
    #elif defined _PIC18LF43K22_H_ 
		#info [Serial Library] Building Serial2 for PIC18LF43K22
	#elif defined _PIC18LF44K22_H_ 
		#info [Serial Library] Building Serial2 for PIC18LF44K22
	#elif defined _PIC18LF45K22_H_ 
		#info [Serial Library] Building Serial2 for PIC18LF45K22
	#elif defined _PIC18LF46K22_H_ 
		#info [Serial Library] Building Serial2 for PIC18LF46K22
    #elif defined _PIC18F23K22_H_  
		#info [Serial Library] Building Serial2 for PIC18F23K22
	#elif defined _PIC18F24K22_H_  
		#info [Serial Library] Building Serial2 for PIC18F24K22
	#elif defined _PIC18F25K22_H_  
		#info [Serial Library] Building Serial2 for PIC18F25K22
	#elif defined _PIC18F26K22_H_ 
		#info [Serial Library] Building Serial2 for PIC18F26K22
    #elif defined _PIC18F43K22_H_
		#info [Serial Library] Building Serial2 for PIC18F43K22
	#elif defined _PIC18F44K22_H_  
		#info [Serial Library] Building Serial2 for PIC18F24K22
	#elif defined _PIC18F45K22_H_  
		#info [Serial Library] Building Serial2 for PIC18F45K22
	#elif defined _PIC18F46K22_H_ 
		#info [Serial Library] Building Serial2 for PIC18F46K22
	#endif

	static float Serial2_DeviceInit(float baudrate)
	{
		//----------------------[CALCUL DU BAUDRATE]----------------------------
		float baudL, baudH, errorL, errorH;
		uint bgrL, bgrH; 
		bgrL =  (uint) FloatToInt(((float)FOSC / baudrate / 64.0f) - 1.0f);
		bgrH =  (uint) FloatToInt(((float)FOSC / baudrate / 16.0f) - 1.0f);
		baudL = (float) FOSC / (64.0f * (bgrL+1));
		baudH = (float) FOSC / (16.0f * (bgrH+1));
		errorL = (baudL - baudrate) / baudrate;
		errorH = (baudH - baudrate) / baudrate;
		if(errorL < 0) { errorL = -errorL; }
		if(errorH < 0) { errorH = -errorH; }


		ANSELDbits.ANSD6 = 0;
		ANSELDbits.ANSD7 = 0;
		TRISDbits.RD6 = 1; // TX2 as input 
		TRISDbits.RD7 = 1; // RX2 as input 

		//------------------------[TXSTA1 + SPBRG1]-----------------------------           
		TXSTA2bits.TX9 = 0; // 8 bit mode
		TXSTA2bits.TXEN = 1; // Enable TX
		TXSTA2bits.SYNC = 0; // Asynchronous mode
		if(errorL <= errorH){ // Low baudrate mode
			SPBRG2 = bgrL;
			TXSTA2bits.BRGH = 0; // Low speed baud rate mode
		}else{  // High baudrate mode
			SPBRG2 = bgrH;
			TXSTA2bits.BRGH = 1; // Low speed baud rate mode
			baudL = baudH;
		}

		//---------------------[RCSTA1 + Interrrupts]---------------------------
		RCSTA2bits.RX9 = 0; // 8 bit reception mode
		RCSTA2bits.CREN = 1; // Enable RX
		PIE3bits.RC2IE = 1; // Enable RX interrupt
		PIE3bits.TX2IE = 0; // Disable TX interrupt, it will be enabled when writing data to TX
		RCSTA2bits.SPEN = 1; // Enable UART1 
		
		return baudL;
	}
	
	static void Serial2_DeviceEnableTXInterrupt(void)
	{
		if(PIE3bits.TX2IE == 0){ // Pas d'envoi en cours, on active l'interruption
			PIE3bits.TX2IE = 1; // Enable TX interrupt
		}
	}
	
	void Serial2_ISR(void)
	{
		if(PIR3bits.RC2IF){ // Réception d'un caractère
			incrementCursor2(&rxCursor2);
			rxBuffer2[rxCursor2] = RCREG2;
		}

		if(PIR3bits.TX2IF && PIE3bits.TX2IE == 1){ // UART Libre pour l'envoi
			if(Serial2_WriteAvailable()){ // Des caractères sont en attente
				incrementCursor2(&txCursor2); // On incrémente la tête d'écriture
				TXREG2 = txBuffer2[txCursor2]; // On écrit le caractère sur le port série
			}else{ // Tous les caractères ont été envoyés
				PIE3bits.TX2IE = 0; // Disable TX interrupt
			}
		}
	}
#endif // UART2


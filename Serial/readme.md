# Serial Library for PICs (UART)

Cette librairie permet l'utilisation simple des ports série UART sur différents PICs.
Pour le moment, le seul format supporté est 8 bits de données, pas de parité et 1 bit de stop.

### Pour les PICs ayant un vecteur d'interruption partagé, il faudra inclure la fonction `Serial_ISR()` pour l'UART1 ou `SerialX_ISR()` pour l'UARTX (avec X > 1)


**Microprocesseurs compatibles :**
- [x]  -> Testé sur le composant
- [ ] -> Librairie compatible, mais pas testé sur le composant

- PIC18F :
	- [ ] PIC18(L)F23K22
	- [ ] PIC18(L)F24K22
	- [ ] PIC18(L)F25K22
	- [ ] PIC18(L)F26K22
	- [ ] PIC18(L)F43K22
	- [ ] PIC18(L)F44K22
	- [x] PIC18(L)F45K22
	- [ ] PIC18(L)F46K22
- PIC24F :
	- [x] PIC24FJ64GA006 
	- [ ] PIC24FJ96GA006
	- [ ] PIC24FJ128GA006 
	- [ ] PIC24FJ64GA008
	- [ ] PIC24FJ96GA008 
	- [ ] PIC24FJ128GA008 
	- [ ] PIC24FJ64GA010 
	- [ ] PIC24FJ96GA010 
	- [ ] PIC24FJ128GA010 

## Exemple
```
#include <xc.h>
#include "Serial.h"

// NE PAS OUBLIER DE CONFIGURER LE FICHIER "Serial_UserPref.h"
// CE FICHER DOIT CONTENIR LES DEFINITIONS DE FCY ET/OU FOSC SELON VOTRE COMPOSANT
// FCY -> Fréquence d'instruction
// FOSC -> Fréqence de l'oscillateur (PLL inclue)

void main()
{
//------------------------------------------------------------
//----------------[Utilisation de l'UART1]--------------------
//------------------------------------------------------------

	// Initialise l'UART1 à 9600 bauds
	float userBaudrate = 9600.0; // Baudrate souhaité
	float baud;                  // Baudrate réel utilisé par le port série
	baud = Serial_Init(9600);    // Initialisation de l'UART1

	Serial_WriteByte('T');       // Envoie le caractère T sur le port Série
	Serial_WriteSring("UART1");  // Envoie la chaine "UART1" sur le port Série

	char data[] = {'T', 'E', 'X', 'T', 'E' };
	char dataSize = 5;
	Serial_Write(data, dataSize); // Envoi des 5 premières cases du tableau data sur le port Serie



//------------------------------------------------------------
//----------------[Utilisation de l'UART2]--------------------
//------------------------------------------------------------

	// Initialise l'UART2 à 19200 bauds
	Serial2_Init(19200);         // Initialisation de l'UART1
	Serial2_WriteByte('T');      // Envoie le caractère T sur le port Série
	Serial2_WriteSring("UART1"); // Envoie la chaine "UART1" sur le port Série

	char data[] = {'T', 'E', 'X', 'T', 'E' };
	char dataSize = 5;
	Serial2_Write(data, dataSize); // Envoi des 5 premières cases du tableau data sur le port Serie

	while(1)
	{
		if(Serial_Available()){         // Si l'UART1 à reçu des données
			char c = Serial_Read(); // On lit la première donnée
			Serial2_WriteByte(c);   // On envoie la donnée sur l'UART2
		}
		
		if(Serial2_Available()){         // Si l'UART1 à reçu des données
			char c = Serial2_Read(); // On lit la première donnée
			Serial_WriteByte(c);     // On envoie la donnée sur l'UART2
		}
	}
}

// Uniquement si le PIC à besoin d'une ISR commune 
// (Qui ne possède pas une interruption par périphérique)
#ifdef SINGLE_ISR
void interrupt ISR_routine( void ){
    Serial_ISR();
    Serial2_ISR();
}
#endif
```
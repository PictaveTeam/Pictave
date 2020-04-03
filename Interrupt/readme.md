# Interrupt Library for PICs (Timer)

Cette librairie permet la gestion des interruptions pour les pics.
Elle n'est pas compatible avec la carte Pictave.


## Sommaire :
- [Microcontrôleurs compatibles](#microcontrôleurs-compatibles-) 
- [Exemples](#exemple)
- [Documentation](#Documentation)

## Microcontrôleurs compatibles :
- [x] -> Librairie compatible, testé sur le composant
- [ ] -> Librairie compatible, mais pas testé sur le composant


- PIC18F :
	- [x] PIC18F45K22
- PIC16F :
	- [x] PIC16F690
	- [ ] PIC16F876
	- [ ] PIC16F877

## Exemple (invalide TBD)
```
#include <xc.h>
#include "timer.h"
#include "interrupt.h"



void main()
{
	int Status;
//------------------------------------------------------------
//----------------[Utilisation du timer]--------------------
//------------------------------------------------------------

	// Initialise le timer sur une periode de 1ms
	Status = timerInit(0.001);
	if (Status == PIC_FAILED)
		while(1);

	while(1)
	{
		S0 ^= 1; // on inverse l'état de la sortie 0 (doit-être configurer avant)
		__delay_ms(10); // on attend 10 ms.
	}
}

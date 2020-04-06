# Timer Library for PICs (Timer)

Cette librairie permet l'utilisation de timers pouvant être utilisé en non bloquants ou
bloquants (while).


## Sommaire :
- [Microcontrôleurs compatibles](#microcontrôleurs-compatibles-) 
- [Exemples](#exemple)
- [Documentation](#Documentation)

## Microcontrôleurs compatibles :
- [x] -> Librairie compatible, testé sur le composant
- [ ] -> Librairie compatible, mais pas testé sur le composant


- PIC24F :
	- [x] PIC24FJ64GA006
- PIC18F :
	- [x] PIC18F45K22

## Exemple
```
#include <xc.h>
#include "timer.h"

// NE PAS OUBLIER DE CONFIGURER LE FICHIER "timer.h"
// CE FICHER DOIT CONTENIR LES DEFINITIONS DE FCY
// FCY -> Fréquence d'instruction
// FOSC -> Fréqence de l'oscillateur (PLL inclue)

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
		delay(10); // on attend 10 ms.
	}
}

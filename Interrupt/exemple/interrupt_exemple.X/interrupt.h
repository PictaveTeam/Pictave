/****************************************************************************/
/**
*
* @file interrupt.h
* 
*
* This file is used to manage interruptions. It allows to connect the 
* interrupt routine of the PIC to functions of other drivers.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date	    Changes
* ----- ------ -------- ----------------------------------------------
* 1.00	epeu   03/03/20 First Release
* 
*****************************************************************************/

#ifndef INTERRUPT_H
#define	INTERRUPT_H

/***************************** Include Files ********************************/

#include <xc.h>   
#include <string.h>

/************************** Constant Definitions ****************************/

#define PIC_SUCCESS 0
#define PIC_FAILED (-1)

/**************************** Type Definitions ******************************/

typedef void (*ISR_Function[10])(void);
typedef void (*ISR_FunctionPtr)(void);
typedef unsigned char u8;
typedef unsigned int u16;

typedef struct
{
    u8 ISRnbr;
    ISR_Function FunctTab;
}t_ISRConfig;

/************************** Function Prototypes *****************************/

int ISR_Init(void);
int ISR_ConnectFunction(ISR_FunctionPtr Function);
void ISR_Tasks(void);


#endif	/* INTERRUPT_H */


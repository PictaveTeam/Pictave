/****************************************************************************/
/**
*
* @file timer.c
* 
*
* This file contains the implementation of the interface functions for timer 1.
* This library has only been experimented with Pictave for the moment.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date	    Changes
* ----- ------ -------- ----------------------------------------------
* 1.00	epeu   21/03/20 First Release
* 1.01  epeu   22/03/20 Delete an include because already include in xc.h
* 
*****************************************************************************/

#ifndef TIMER_H
#define TIMER_H

/************************** Constant Definitions ****************************/

#define FCY ((double)16000000)
#define PIC_SUCCESS       0
#define PIC_FAILED      (-1)
#define TIMER_READY       0
#define TIMER_NOT_READY   1
#define REGISTER_SIZE_16  65536
#define PRESC_256         256U
#define PRESC_64          64U
#define PRESC_8           8U
#define PRESC_1           1U

/***************************** Include Files ********************************/

#include "xc.h"
#include <string.h>
#ifdef __PIC24FJ64GA006__

#define PICTAVE

#endif /* __PIC24FJ64GA006__ */

/**************************** Type Definitions ******************************/

typedef unsigned char t_timer[10];
typedef unsigned int u16;
typedef unsigned char u8;
typedef struct
{
    u8 State;
    t_timer timer;
}t_timerInit;

/************************** Function Prototypes *****************************/

int timerInit(float period); /* Init timer */
u16 getTimer(u8 timer); /* get timer value */
int setTimer(u8 timer, u16 value); /* launch timer */
int timerEnable(void);
int timerDisable(void);
void __delay_ms(u16 t); /* use of timer 0 of setTimer() */

#endif /* TIMER_H */


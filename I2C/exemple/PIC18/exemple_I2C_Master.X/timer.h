/****************************************************************************/
/**
*
* @file timer.h
* 
*
* This file contains the implementation of the interface functions for timer 1
* or timer 0 (PIC18F).
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date	    Changes
* ----- ------ -------- ----------------------------------------------
* 1.00	epeu   21/03/20 First Release
* 1.01  epeu   22/03/20 Delete an include because already include in xc.h
* 2.00  epeu   06/04/20 Add support of PIC18F45K22
* 
*****************************************************************************/

#ifndef TIMER_H
#define TIMER_H

/***************************** Include Files ********************************/

#include "xc.h"
#include <string.h>
#ifdef __PIC24FJ64GA006__

#define PICTAVE

#endif /* __PIC24FJ64GA006__ */
#ifdef _PIC18F45K22_H_

#define ENEMEA

#endif /* _PIC18F45K22_H_ */

/************************** Constant Definitions ****************************/

#ifdef PICTAVE
#define FCY  ((double)16000000)
#elif defined( ENEMEA )
#define FOSC ((double)80000000)
#define FTIMER (FOSC/4)
#endif /* PICTAVE | ENEMEA */

#define PIC_SUCCESS       0
#define PIC_FAILED      (-1)
#define TIMER_READY       0
#define TIMER_NOT_READY   1
#define REGISTER_SIZE_16  65536
#define PRESC_256         256U
#define PRESC_128         128U
#define PRESC_64          64U
#define PRESC_32          32U
#define PRESC_16          16U
#define PRESC_8           8U
#define PRESC_4           4U
#define PRESC_2           2U
#define PRESC_1           1U

/**************************** Type Definitions ******************************/

typedef unsigned char t_timer[10];
typedef unsigned int u16;
typedef unsigned char u8;
typedef struct
{
    u8 State;
    t_timer timer;
    u16 Pr;
}t_timerInit;

/************************** Function Prototypes *****************************/

int timerInit(float period); /* Init timer */
void timer_ISR(void); /* interrupt routine */
u16 getTimer(u8 timer); /* get timer value */
int setTimer(u8 timer, u16 value); /* launch timer */
int timerEnable(void);
int timerDisable(void);
void delay(u16 t); /* use of timer 0 */
void prescaler(float period, u16 *pr, u16 presc, u16 *prescUsed, float *min);


#endif /* TIMER_H */


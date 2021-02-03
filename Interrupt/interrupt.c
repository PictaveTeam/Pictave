/****************************************************************************/
/**
*
* @file interrupt.c
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
* 1.01	epeu   03/02/21 Bug fix : Overflow
* 
*****************************************************************************/

/***************************** Include Files ********************************/

#include "interrupt.h"

/************************** Variable Definitions ****************************/

t_ISRConfig ISRConfig;

/****************************************************************************/
/**
*
* Interrupt routine
*
* @param  None
*
* @return None.
*
* @note
*
*
*****************************************************************************/
void __interrupt() ISR_Routine()
{
    ISR_Tasks();
}

/****************************************************************************/
/**
*
* Initializes the timer Interrupt driver by setting the number of functions 
* to 0.
*
* @param	None
*
* @return
*
*		- PIC_SUCCESS if initialization was successful
*
* @note    None
*
*****************************************************************************/
int ISR_Init(void)
{
    ISRConfig.ISRnbr = 0;
    return PIC_SUCCESS;
}

/****************************************************************************/
/**
*
* This function connects the function of another driver to the interrupt 
* routine. 
*
* @param	pointer to a Function
*
* @return
*
*		- PIC_SUCCESS
*		- PIC_FAILED
*
* @note    None
*
*****************************************************************************/
int ISR_ConnectFunction(ISR_FunctionPtr Function)
{
	if(ISRConfig.ISRnbr > 9)
		return PIC_FAILED;
    ISRConfig.FunctTab[ISRConfig.ISRnbr] = Function;
    ISRConfig.ISRnbr++;
    return PIC_SUCCESS;
}

/****************************************************************************/
/**
*
* Main function of this driver. 
*
* @param	pointer to a Function
*
* @return   None
*
*
* @note   This function performs all functions that have been added 
* to the interrupt routine.
*
*****************************************************************************/
void ISR_Tasks(void)
{
    u8 i;
    for(i=0;i<ISRConfig.ISRnbr;i++)
        ISRConfig.FunctTab[i]();
}

/****************************************************************************/
/**
 * @end file
 * 
 * **************************************************************************/

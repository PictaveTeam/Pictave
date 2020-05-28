/****************************************************************************/
/**
*
* @file config.h
* 
*
* This file contains the declaration of hardware function for Pictave Card.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date	    Changes
* ----- ------ -------- ----------------------------------------------
* 1.00	epeu   22/03/20 First Release
* 
*****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

/***************************** Include Files ********************************/

#include "xc.h"
#include "p24FJ64GA006.h"

/************************** Constant Definitions ****************************/

#define PIC_SUCCESS       0
#define PIC_FAILED      (-1)

#ifdef __PIC24FJ64GA006__

#define PICTAVE

#endif /* __PIC24FJ64GA006__ */

/************************** Function Prototypes *****************************/

int initPictave(void);

#endif /* CONFIG_H */
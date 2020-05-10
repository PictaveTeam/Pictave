/****************************************************************************/
/**
*
* @file I2C.h
* 
*
* This file contains the I2C driver
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date	    Changes
* ----- ------ -------- ----------------------------------------------
* 1.00	epeu   26/02/20 First Release
* 2.00	epeu   02/05/20 Add support of PIC18FXXK22 and non-blocking functions
* 3.00  epeu   07/05/20 Changing the handling of incoming and outgoing
*                       messages for PIC18. Correction of a PIC18 bug.
* 3.01  epeu   10/05/20 Add Bus Speed calcul for PIC18 : 300kbs
* 
*****************************************************************************/

#ifndef I2C_H
#define I2C_H

/***************************** Include Files ********************************/

#include "I2C_conf/I2C_PIC_Support.h"

/************************** Function Prototypes *****************************/

void I2C_ISR(void);
int I2C_Scan(t_I2CScan *Adresses, u8 NbrAdress);
int I2C_Get_Status(u8 MsgID);
int I2C_Init(u8 Options);
int I2C_Write(u8 adresse, u8 *data, u8 sizeData, u8 delay);
int I2C_Read_Request(u8 adresse, u8 sizeData, u8 delay);
int I2C_Read_Register_Request(u8 adresse, u8 *sendData, u8 sendsizeData, u8 rcvsizeData, u8 delay);
#ifdef PICTAVE
u8* I2C_Read_Buffer(u8 *adresse, u8 *sizeData);
#elif defined(ENEMEA)
t_mem I2C_Read_Buffer(u8 *adresse, u8 *sizeData);
#endif /* PICTAVE | ENEMEA */


void I2C_TabRefresh(void);
int I2C_Free(void);


#endif	/* I2C_H */


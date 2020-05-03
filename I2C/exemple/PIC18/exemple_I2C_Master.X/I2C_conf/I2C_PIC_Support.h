/****************************************************************************/
/**
*
* @file I2C_PIC_Support.h
* 
*
* This file contains the I2C hardware support and blocking function for master
* mode. Constants and types for I2C driver included.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date	    Changes
* ----- ------ -------- ----------------------------------------------
* 1.00	epeu   02/05/20 First Release
* 
*****************************************************************************/

#ifndef I2C_PIC_SUPPORT_H
#define	I2C_PIC_SUPPORT_H

/***************************** Include Files ********************************/

#include <xc.h> // include processor files - each processor file is guarded.  
#include <string.h> /* include memset function */
#include <stdlib.h> /* include malloc function */

/************************** Constant Definitions ****************************/

#define I2C_PIC18_SUPPORT (defined(_PIC18F44K22_H_) || defined(_PIC18F45K22_H_)|| defined(_PIC18F46K22_H_))

#ifdef __PIC24FJ64GA006__

#define PICTAVE
#define _ISR_ROUTINE __attribute__((interrupt, auto_psv))

#endif /* __PIC24FJ64GA006__ */
#if I2C_PIC18_SUPPORT

#define ENEMEA

#endif /* _PIC18F45K22_H_ */

#define I2C_READY                 1
#define I2C_NOT_READY             0
#define I2C_END                   1
#define I2C_RECEIVED              2
#define I2C_PROCESSING            0
#define PIC_SUCCESS               0
#define PIC_FAILED              (-1)
#define I2C_SEND_OVERFLOW       (-2)
#define SLAVE                 (0x01)
#define MASTER                (0x02)
#define INTERRUPT             (0x04)

#ifdef PICTAVE
#define FCY ((double)16000000)
#define FTIMER (FCY)
#elif defined( ENEMEA )
#define FOSC ((double)80000000)
#define FTIMER (FOSC/4)
#endif /* PICTAVE | ENEMEA */

#define REGISTER_SIZE_16  65536
#define REGISTER_SIZE_8   256
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

typedef unsigned int u16;
typedef unsigned char u8;

typedef enum
{
    MASTER_START,
    MASTER_ADRESSE,
    MASTER_ACK_AD,
    MASTER_WRITE,
    MASTER_ACK_DA,
    MASTER_READ,
    MASTER_READ_NOACK,
    MASTER_RESTART,
    MASTER_STOP,
    I2C_OVERFLOW,
    I2C_COLISION,
    BUS_READY,
    DELAY_TIMEOUT,
    DELAY_INTER,
    DELAY_DA,
    SCAN
}t_I2CState;

typedef enum
{
    I2C_MASTER_WRITE,
    I2C_MASTER_READ,
    I2C_MASTER_WRITE_WITH_RESTART,
    I2C_MASTER_SCAN
}t_I2CMsgType;

typedef struct
{
    t_I2CMsgType type;
    u8 adress;
    u8 *data;
    u8 sizeData;
    u8 delay;
}t_I2CMsg;

typedef struct
{
    t_I2CState State;
    t_I2CMsg Buffer[10];
    t_I2CMsg RcvBuffer[10];
    u8 MsgID[11];
    u8 RcvMsgID[11];
    u8 BufferNbr;
    u8 RcvBufferNbr;
    u8 Status;
    u8 Options;
}t_I2CConfig;

typedef struct
{
    u8 Adress[10];
    u8 NbrAdress;
    u8 State;
}t_I2CScan;

typedef struct
{
    u8 *data;
    u8 sizeData;
    u8 MsgID;
}t_I2CMyMsg;

#ifdef PICTAVE
typedef u8 *t_I2CPointerTab[10];
typedef struct
{
    t_I2CPointerTab PointTab;
    u8 sizePoint;
}t_I2CMemory;
#endif /* PICTAVE */

/************************** Function Prototypes *****************************/

char I2C_Master_Read(unsigned char a);
int I2C_Master_Stop(void);
int I2C_Master_Write(int data);
int I2C_Master_Start(int adresse);
int I2C_Master_RepeatStart(int adresse);
int I2C_EEPROM_Write(char adresse,int EEadresse, char data);
int I2C_EEPROM_Read(char adresse, int EEadresse);
void I2C_timer_prescaler(float period, u16 *prUsed, u16 presc, u16 *prescUsed, float *min);

#endif	/* I2C_PIC_SUPPORT_H */


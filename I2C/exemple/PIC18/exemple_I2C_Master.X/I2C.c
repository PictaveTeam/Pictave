/****************************************************************************/
/**
*
* @file I2C.c
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
* 
*****************************************************************************/

/***************************** Include Files ********************************/

#include "I2C.h"

/************************** Variable Definitions ****************************/

t_I2CConfig I2CConfig;
t_I2CScan *I2Cadress;
#ifdef PICTAVE
t_I2CMemory I2CMemory;
u16 PointerRefresh;
#endif /* PICTAVE */
u16 timer;
u8 myMsgID;

/****************************************************************************/

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
*****************************************************************************/

void I2C_ISR(void)
{
    static u8 i = 0;
    static u8 ack = 0;
    u8 type;
#ifdef PICTAVE
    if((IFS1bits.MI2C1IF == 1) || ((IFS0bits.T2IF == 1) && ((I2CConfig.State == DELAY_DA) || (I2CConfig.State == DELAY_INTER))))
    {
#elif defined(ENEMEA)
    if((SSP1IF == 1) || ((TMR2IF == 1) && ((I2CConfig.State == DELAY_DA) || (I2CConfig.State == DELAY_INTER))))
    {
#endif /* PICTAVE | ENEMEA */
        delay_end:
        switch(I2CConfig.State)
        {
            case MASTER_START:
                
                i = 0;
                ack = 0;
                if(I2CConfig.Buffer[0].delay > 0)
                {
                    timer = I2CConfig.Buffer[0].delay;
                    I2CConfig.State = DELAY_DA;
                }
                else
                {
                    timer = 10;
                    I2CConfig.State = DELAY_TIMEOUT;
                }
#ifdef PICTAVE
                I2C1TRN = I2CConfig.Buffer[0].adress;
#elif defined(ENEMEA)
                SSP1BUF = I2CConfig.Buffer[0].adress;
#endif /* PICTAVE | ENEMEA */
                break;
            case MASTER_WRITE:
#ifdef PICTAVE
                if(I2C1STATbits.ACKSTAT==0)
                {
#elif defined(ENEMEA)
                if(SSP1CON2bits.ACKSTAT==0)
                {
#endif /* PICTAVE | ENEMEA */
                    if(I2CConfig.Buffer[0].sizeData > 1)
                    {
#ifdef PICTAVE
                        I2C1TRN = I2CConfig.Buffer[0].data[i];
#elif defined(ENEMEA)
                        SSP1BUF = I2CConfig.Buffer[0].data[i];
#endif /* PICTAVE | ENEMEA */
                        i++;
                        I2CConfig.Buffer[0].sizeData--;
                        if(I2CConfig.Buffer[0].delay > 0)
                        {
                            ack = 0;
                            I2CConfig.State = DELAY_DA;
                            timer = I2CConfig.Buffer[0].delay;
                        }
                    }
                    else
                    {
#ifdef PICTAVE
                        I2C1TRN = I2CConfig.Buffer[0].data[i];
#elif defined(ENEMEA)
                        SSP1BUF = I2CConfig.Buffer[0].data[i];
#endif /* PICTAVE | ENEMEA */
                        i++;
                        I2CConfig.Buffer[0].sizeData--;
                        if(I2CConfig.Buffer[0].type == I2C_MASTER_WRITE_WITH_RESTART)
                        {
                            I2CConfig.State = MASTER_STOP;
                        }
                        else
                            I2CConfig.State = MASTER_ACK_DA;
                        
                    }
                        
                }
                else
                {
                    ack = 0;
#ifdef PICTAVE
                    I2C1CONbits.SEN = 1;
#elif defined(ENEMEA)
                    SSP1CON2bits.SEN = 1;
#endif /* PICTAVE | ENEMEA */
                    if(I2CConfig.Buffer[0].delay > 0)
                    {
                        I2CConfig.State = DELAY_INTER;
                        timer = I2CConfig.Buffer[0].delay;
                    }
                    else
                    {
                        I2CConfig.State = MASTER_STOP;
                    }
                }
                break;
            case MASTER_READ:
#ifdef PICTAVE
                I2C1CONbits.RCEN = 1;
#elif defined(ENEMEA)
                SSP1CON2bits.RCEN = 1;
#endif /* PICTAVE | ENEMEA */
                I2CConfig.State = MASTER_READ_NOACK;
                break;
            case MASTER_READ_NOACK:
                if(i < (I2CConfig.Buffer[0].sizeData - 1))
                {
#ifdef PICTAVE
                    I2C1CONbits.ACKDT = 0;
                    I2CConfig.Buffer[0].data[i] = I2C1RCV;
#elif defined(ENEMEA)
                    SSP1CON2bits.ACKDT = 0;
                    I2CConfig.Buffer[0].data[i] = SSP1BUF;
#endif /* PICTAVE | ENEMEA */
                    i++;
                    if(I2CConfig.Buffer[0].delay > 0)
                    {
                        ack = 0;
                        I2CConfig.State = DELAY_DA;
                        timer = I2CConfig.Buffer[0].delay;
                    }
                    else
                        I2CConfig.State = MASTER_READ;
                }
                else
                {
#ifdef PICTAVE
                    I2C1CONbits.ACKDT = 1;
                    I2CConfig.Buffer[0].data[i] = I2C1RCV;
#elif defined(ENEMEA)
                    SSP1CON2bits.ACKDT = 1;
                    I2CConfig.Buffer[0].data[i] = SSP1BUF;
#endif /* PICTAVE | ENEMEA */
                    i++;
                    I2CConfig.State = MASTER_ACK_DA;
                }
#ifdef PICTAVE
                I2C1CONbits.ACKEN = 1;
#elif defined(ENEMEA)
                SSP1CON2bits.ACKEN=1;
#endif /* PICTAVE | ENEMEA */
                break;
            case MASTER_ACK_DA:
                ack = 0;
#ifdef PICTAVE
                I2C1CONbits.PEN = 1;
#elif defined(ENEMEA)
                SSP1CON2bits.PEN = 1;
#endif /* PICTAVE | ENEMEA */
                if(I2CConfig.Buffer[0].delay > 0)
                {
                    I2CConfig.State = DELAY_INTER;
                    timer = I2CConfig.Buffer[0].delay;
                }
                else
                {
                    I2CConfig.State = MASTER_STOP;
                }
                break;
            case MASTER_STOP:
                type = I2CConfig.Buffer[0].type;
                if(I2CConfig.Buffer[0].type == I2C_MASTER_READ)
                {
                    if (I2CConfig.RcvBufferNbr > 9)
                    {
#ifdef PICTAVE
                        free(I2CConfig.RcvBuffer[0].data);
#endif /* PICTAVE */
                        for(i = 0; i<(I2CConfig.RcvBufferNbr - 1) ; i++)
                        {
                            I2CConfig.RcvBuffer[i] = I2CConfig.RcvBuffer[i+1];
                            I2CConfig.RcvMsgID[i] = I2CConfig.RcvMsgID[i+1];
                        }
                        I2CConfig.RcvBufferNbr--;
                    }
                    I2CConfig.RcvBuffer[I2CConfig.RcvBufferNbr].adress = I2CConfig.Buffer[0].adress;
                    I2CConfig.RcvBuffer[I2CConfig.RcvBufferNbr].data = I2CConfig.Buffer[0].data;
                    I2CConfig.RcvBuffer[I2CConfig.RcvBufferNbr].sizeData = i;
                    I2CConfig.RcvMsgID[I2CConfig.RcvBufferNbr] = I2CConfig.MsgID[0];
                    I2CConfig.RcvBufferNbr++;
                }
#ifdef PICTAVE
                else
                    free(I2CConfig.Buffer[0].data);
#endif /* PICTAVE */
                for(i = 0; i<I2CConfig.BufferNbr ; i++)
                {
                    I2CConfig.Buffer[i] = I2CConfig.Buffer[i+1];
                    I2CConfig.MsgID[i] = I2CConfig.MsgID[i+1];
                }
                I2CConfig.BufferNbr--;
                if(I2CConfig.BufferNbr > 0)
                {
                    I2CConfig.State = MASTER_START;
                    if(type == I2C_MASTER_WRITE_WITH_RESTART)
                    {
#ifdef PICTAVE
                        I2C1CONbits.RSEN = 1;
#elif defined(ENEMEA)
                        SSP1CON2bits.RSEN = 1;
#endif /* PICTAVE | ENEMEA */
                    }
                    else
                    {
#ifdef PICTAVE
                        I2C1CONbits.SEN = 1;
#elif defined(ENEMEA)
                        SSP1CON2bits.SEN = 1;
#endif /* PICTAVE | ENEMEA */
                    }
                }
                else
                    I2CConfig.State = BUS_READY;
                break;
            case DELAY_DA:
#ifdef PICTAVE
                if(IFS1bits.MI2C1IF)
                    ack = 1;
#elif defined(ENEMEA)
                if(SSP1IF)
                    ack = 1;
#endif /* PICTAVE | ENEMEA */
                if((timer == 0) && ack)
                {
                    if((I2CConfig.Buffer[0].type == I2C_MASTER_WRITE)||(I2CConfig.Buffer[0].type == I2C_MASTER_WRITE_WITH_RESTART))
                    {
                        I2CConfig.State = MASTER_WRITE;
                        goto delay_end;
                    }
                    if(I2CConfig.Buffer[0].type == I2C_MASTER_READ)
                    {
                        I2CConfig.State = MASTER_READ;
                        goto delay_end;
                    }
                }
                if((timer == 0) && (ack == 0))
                {
#ifdef PICTAVE
                    I2C1CONbits.SEN = 0;
                    I2C1CONbits.SEN = 1;
#elif defined(ENEMEA)
                    SSP1CON2bits.SEN = 0;
                    SSP1CON2bits.SEN = 1;
#endif /* PICTAVE | ENEMEA */
                    I2CConfig.State = MASTER_START;
                }
                break;
            case DELAY_TIMEOUT:
#ifdef PICTAVE
                if(IFS1bits.MI2C1IF)
                {
#elif defined(ENEMEA)
                if(SSP1IF)
                {
#endif /* PICTAVE | ENEMEA */
                    
                    if((I2CConfig.Buffer[0].type == I2C_MASTER_WRITE)||(I2CConfig.Buffer[0].type == I2C_MASTER_WRITE_WITH_RESTART))
                    {
                        I2CConfig.State = MASTER_WRITE;
                        goto delay_end;
                    }
                    if(I2CConfig.Buffer[0].type == I2C_MASTER_READ)
                    {
                        I2CConfig.State = MASTER_READ;
                        goto delay_end;
                    }
                }
                else
                {
                    if(timer == 0)
                    {
#ifdef PICTAVE
                    I2C1CONbits.SEN = 0;
                    I2C1CONbits.SEN = 1;
#elif defined(ENEMEA)
                    SSP1CON2bits.SEN = 0;
                    SSP1CON2bits.SEN = 1;
#endif /* PICTAVE | ENEMEA */
                        I2CConfig.State = MASTER_START;
                    }
                }
                break;
            case DELAY_INTER:
#ifdef PICTAVE
                if(IFS1bits.MI2C1IF)
                    ack = 1;
#elif defined(ENEMEA)
                if(SSP1IF)
                    ack = 1;
#endif /* PICTAVE | ENEMEA */
                if((timer == 0) && ack)
                {
                    I2CConfig.State = MASTER_STOP;
                    goto delay_end;
                }
                if((timer == 0) && (ack == 0))
                {
#ifdef PICTAVE
                    I2C1CONbits.SEN = 0;
                    I2C1CONbits.SEN = 1;
#elif defined(ENEMEA)
                    SSP1CON2bits.SEN = 0;
                    SSP1CON2bits.SEN = 1;
#endif /* PICTAVE | ENEMEA */
                    I2CConfig.State = MASTER_START;
                }
                break;
            
            default:
                break;
        }
#ifdef PICTAVE
        IFS1bits.MI2C1IF = 0;
#elif defined(ENEMEA)
        SSP1IF = 0;
#endif /* PICTAVE | ENEMEA */
    }
#ifdef PICTAVE
    if(IFS0bits.T2IF)
    {
#elif defined(ENEMEA)
    if(TMR2IF)
    {
#endif /* PICTAVE | ENEMEA */
        if(timer > 0)
            timer--;
#ifdef PICTAVE
        if(PointerRefresh > 0)
            PointerRefresh--;
        else
        {
            PointerRefresh = 10000;
        }
        IFS0bits.T2IF = 0;
#elif defined(ENEMEA)
        TMR2IF = 0;
#endif /* PICTAVE | ENEMEA */
    }
}
                
#ifdef PICTAVE

/****************************************************************************/
/**
*
* Timer 2 Interrupt routine
*
* @param  None.
*
* @return None.
*
* @note
*
* Valid only on PIC24FJ64GA006
*
*****************************************************************************/
void _ISR_ROUTINE _T2Interrupt ( void )
{
    I2C_ISR();
}

/****************************************************************************/
/**
*
* I2C Interrupt routine
*
* @param  None.
*
* @return None.
*
* @note
*
* Valid only on PIC24FJ64GA006
*
*****************************************************************************/
void _ISR_ROUTINE _MI2C1Interrupt ( void )
{
    I2C_ISR();
}

#endif

#ifdef BETA

/****************************************************************************/
/**
*
* I2C address scan function. This function returns an array with 
* the addresses of the connected I2C slaves.
*
* @param  - OUT Parameter : Adresses is a structure that contains an array
*                           where the addresses will be stored, an unsigned
*                           char with the number of addresses detected,
*                           and an unsigned char with the current scan status.
* 
*         - IN  Parameter : NbrMaxAdress is an unsigned char which contains
*                           the number of addresses to be searched for.
*
* @return
* 
*		- PIC_SUCCESS if Scan request was successful
*		- PIC_FAILED if I2C is not init or PIC is not master or option
*         not equal to INTERRUPT 
*
* @note
*
* 10 adresses max. The function is not done.
*
*****************************************************************************/

int I2C_Scan(t_I2CScan *Adresses, u8 NbrMaxAdress)
{
    if((I2CConfig.Options & INTERRUPT) == 0)
        return PIC_FAILED;
    if(I2CConfig.Status != I2C_READY)
        return PIC_FAILED;
    
    myMsgID++;
    if(myMsgID == 0)
        myMsgID++;
    
    I2Cadress = Adresses;
    I2Cadress->NbrAdress = 0;
    I2Cadress->State = I2C_NOT_READY;
    if(I2CConfig.BufferNbr >= 9)
        return I2C_SEND_OVERFLOW;
    I2CConfig.Buffer[I2CConfig.BufferNbr].type = I2C_MASTER_SCAN;
    I2CConfig.MsgID[I2CConfig.BufferNbr] = myMsgID;
    I2CConfig.BufferNbr++;
    if(I2CConfig.State == BUS_READY)
    {
        I2CConfig.State = MASTER_START;
#ifdef PICTAVE
        I2C1CONbits.SEN = 1;
#elif defined(ENEMEA)
        SSP1CON2bits.SEN = 1;
#endif /* PICTAVE | ENEMEA */
    }
    return PIC_SUCCESS;
}
#endif /* BETA */

/****************************************************************************/
/**
*
* This function allows you to obtain the status of a message you have sent
* or if you have received the messages. When a message is sent through the
* interrupt routine, it is added to an I2C request list.
*
* @param  - IN  Parameter : MsgID is a unique unsigned char number that
*                           is returned during an I2C (write, read) request. 
*                           It allows to know the status of the request.
*
* @return
* 
*		- I2C_PROCESSING If the request is in court
*		- I2C_RECEIVED if a reading is received
*       - I2C_END if the message is sent
*		- PIC_FAILED if I2C is not init or PIC is not master or option
*         not equal to INTERRUPT 
*
* @note
*
* None.
*
*****************************************************************************/
int I2C_Get_Status(u8 MsgID)
{
    u8 i;
    if(I2CConfig.Status != I2C_READY)
        return PIC_FAILED;
    for(i = 0; i<I2CConfig.BufferNbr ; i++)
    {
        if(I2CConfig.MsgID[i] == MsgID)
            return I2C_PROCESSING;
    }
    for(i = 0; i<I2CConfig.RcvBufferNbr ; i++)
    {
        if(I2CConfig.RcvMsgID[i] == MsgID)
            return I2C_RECEIVED;
    }
    return I2C_END;
}

/****************************************************************************/
/**
*
* This function initializes the I2C bus, as Master or Slave (not implemented).
* It allows to define a blocking mode, and a non blocking mode thanks to
* the interrupt routine.
*
* @param  - IN  Parameter : 
*                   Options is an unsigned char that takes the values : 
*                           - MASTER
*                           - SLAVE
*                           - INTERRUPT
*
* @return
* 
*		- PIC_SUCCESS if I2C bus is ready.
*		- PIC_FAILED Actually with SLAVE option and if timer 2 fails 
*         to initialize.
*
* @note
*
* The I2C driver uses timer 2 for some functionalities. Slave configuration
* is not yet implemented.
*
*****************************************************************************/

int I2C_Init(u8 Options){
#ifdef PICTAVE
    float min = 20;
    u16 prescUsed = 0;
    u16 prUsed = 0;
    
    I2C_timer_prescaler(0.001, &prUsed, PRESC_256, &prescUsed, &min);
    I2C_timer_prescaler(0.001, &prUsed, PRESC_64, &prescUsed, &min);
    I2C_timer_prescaler(0.001, &prUsed, PRESC_8, &prescUsed, &min);
    I2C_timer_prescaler(0.001, &prUsed, PRESC_1, &prescUsed, &min);
    
    if(min==20)
        return PIC_FAILED;
    I2CConfig.BufferNbr = 0;
    I2CConfig.RcvBufferNbr = 0;
    memset(&I2CConfig.MsgID, 0, 11);
    memset(&I2CConfig.RcvMsgID, 0, 11);
    myMsgID = 1;
    I2CConfig.State = BUS_READY;
    
    switch(prescUsed)
    {
        case PRESC_256:
            T2CONbits.TCKPS=0b11;
            break;
        case PRESC_64:
            T2CONbits.TCKPS=0b10;
            break;
        case PRESC_8:
            T2CONbits.TCKPS=0b01;
            break;
        case PRESC_1:
            T2CONbits.TCKPS=0b00;
            break;
        default:
            return PIC_FAILED;
            break;
    }
    PR2=(u16)prUsed;
    timer = 0;
    T2CONbits.TON=0;            /* timer 2 disable */
    T2CONbits.TSIDL=0;          /* state in idle mode */
    T2CONbits.TGATE=0;          /* disabled accumulation */
    T2CONbits.TCS=0;            /* internal clock */
    T2CONbits.T32=0;            /* 16 bit timer */
    IFS0bits.T2IF=0;            /* overflow cleared */
    IEC0bits.T2IE=1;            /* timer 1 interrupt enable */ 
    IPC1bits.T2IP=1;            /* priority 1(low) */
    T2CONbits.TON=1;            /* timer 2 enable */
    
    
    if((Options & MASTER) != 0)
    {
        if((Options & SLAVE) != 0)
            return PIC_FAILED;
        if((Options & INTERRUPT) != 0)
            IEC1bits.MI2C1IE = 1;
        else
            IEC1bits.MI2C1IE = 0;
        I2C1CON = 0x9000; 
        I2C1STAT = 0;
        I2C1ADD = 0;
        I2C1BRG = 300; 
        I2CConfig.Status = I2C_READY;
        I2CConfig.Options = Options;
        I2CMemory.sizePoint = 0;
        return PIC_SUCCESS;
    }
    return PIC_FAILED;
    
#endif /* PICTAVE */
#ifdef ENEMEA
    PEIE = 1;
    GIE = 1;
    
    float min = 20;
    u16 prescUsed = 0;
    u16 prUsed = 0;
    I2C_timer_prescaler(0.0001, &prUsed, PRESC_16, &prescUsed, &min);
    I2C_timer_prescaler(0.0001, &prUsed, PRESC_4, &prescUsed, &min);
    I2C_timer_prescaler(0.0001, &prUsed, PRESC_1, &prescUsed, &min);
    if(min==20)
        return PIC_FAILED;
    I2CConfig.BufferNbr = 0;
    I2CConfig.RcvBufferNbr = 0;
    memset(&I2CConfig.MsgID, 0, sizeof(I2CConfig.MsgID));
    myMsgID = 1;
    I2CConfig.State = BUS_READY;
    T2CONbits.T2OUTPS = 0b1001;
    PR2 = (u8) prUsed;
    TMR2 = 0;
    TMR2IE = 1;
    TMR2IF = 0;
    switch(prescUsed)
    {
        case PRESC_16:
            T2CONbits.T2CKPS = 0b10;
            break;
        case PRESC_4:
            T2CONbits.T2CKPS = 0b01;
            break;
        case PRESC_1:
            T2CONbits.T2CKPS = 0b00;
            break;
        default:
            return PIC_FAILED;
            break;
    }
    timer = 0;
    T2CONbits.TMR2ON = 1;
    if((Options & INTERRUPT) != 0)
        SSP1IE = 1;
    else
        SSP1IE = 0;
    if((Options & MASTER) != 0)
    {
        if((Options & SLAVE) != 0)
            return PIC_FAILED;
        SSP1CON1bits.SSPEN = 0; 
        SSP1ADD = 65;  // 300 kbps (clk 80 MHz) 
        SSP1STATbits.SMP = 0; 
        SSP1CON1bits.SSPM = 0b1000;  // Master mode 
        SSP1CON1bits.SSPEN = 1; 
        I2CConfig.Status = I2C_READY;
        I2CConfig.Options = Options;
        return PIC_SUCCESS;
    }
    return PIC_FAILED;
#endif /* ENEMEA */
}

/****************************************************************************/
/**
*
* This function is used to send a message on the I2C bus (Master mode only).
*
* @param  - IN  Parameter : 
*                       -adresse : unsigned int which contains 
*                                  the slave's address.
*                       -data : unsigned char array containing the message
*                               to be sent.
*                       -sizeData : size of the data to be sent
*                       -delay : waiting time between sending 2 bytes.
*                                Set to zero if not required. Some slaves 
*                                such as EEPROMs need a timeout.  
*
* @return
* 
*       - PIC_SUCCESS if the message has been sent (only for blocking functions) 
*		- A unique identifier MsgID that allows you to get information about
*         the status of the shipment using the get status function.
*		- PIC_FAILED if I2C is not init or PIC is not master
*
* @note
*
* Master mode only.
* For interrupt mode : 
*   Warning, if this I2C driver is used on PIC18, be careful not to touch the
*   array pointed by data as long as the message has not been sent.
*   On pic24 there is no problem because the driver uses the 
*   dynamic memory allocation and just makes a copy of the array to send.
* For blocking mode :
*   If the slave is not on the bus, the function is in an infinite loop. 
*
*****************************************************************************/

int I2C_Write(u8 adresse, u8 *data, u8 sizeData, u8 delay)
{
    u8 *myData = NULL;
    u8 i;
    
    if(I2CConfig.Status != I2C_READY)
        return PIC_FAILED;
    if((I2CConfig.Options & SLAVE) != 0)
        return PIC_FAILED;
    
    if((I2CConfig.Options & INTERRUPT) == 0)
    {
        I2C_Master_Start(adresse);
        if(delay > 0)
        {
            timer = delay;
            while(timer != 0);
        }
        for(i = 0; i < sizeData; i++)
        {
            I2C_Master_Write(data[i]);
            if(delay > 0)
        {
            timer = delay;
            while(timer != 0);
        }
        }
        I2C_Master_Stop();
        if(delay > 0)
        {
            timer = delay;
            while(timer != 0);
        }
        return PIC_SUCCESS;
    }
    else
    {
        if(I2CConfig.BufferNbr >= 9)
            return I2C_SEND_OVERFLOW;

        myMsgID++;
        if(myMsgID == 0)
            myMsgID++;

#ifdef PICTAVE
        myData = malloc(sizeData * sizeof(u8));
        if(myData == NULL)
        {
            return PIC_FAILED;
        }
        memcpy(myData, data, sizeData * sizeof(u8));
#endif /* PICTAVE */

        I2CConfig.Buffer[I2CConfig.BufferNbr].type = I2C_MASTER_WRITE;
        I2CConfig.Buffer[I2CConfig.BufferNbr].adress = adresse;
#ifdef PICTAVE
        I2CConfig.Buffer[I2CConfig.BufferNbr].data = myData;
#elif defined(ENEMEA)
        I2CConfig.Buffer[I2CConfig.BufferNbr].data = data;
#else
        return PIC_FAILED;
#endif /* PICTAVE | ENEMEA*/
        I2CConfig.Buffer[I2CConfig.BufferNbr].sizeData = sizeData;
        I2CConfig.Buffer[I2CConfig.BufferNbr].delay = delay;
        I2CConfig.MsgID[I2CConfig.BufferNbr] = myMsgID;
        I2CConfig.BufferNbr++;
        if(I2CConfig.State == BUS_READY)
        {
            I2CConfig.State = MASTER_START;
#ifdef PICTAVE
            I2C1CONbits.SEN = 1;
#elif defined(ENEMEA)
            SSP1CON2bits.SEN = 1;
#endif /* PICTAVE | ENEMEA */
        }
        return myMsgID;
    }
}

/****************************************************************************/
/**
*
* This function is used to read a message from the I2C bus (Master mode only).
*
* @param  - IN  Parameter : 
*                       -adresse : unsigned int which contains 
*                                  the slave's address.
*                       -data : unsigned char array in which the message 
*                               will be stored (PIC18 only)
*                       -sizeData : size of the data to be received
*                       -delay : waiting time between sending 2 bytes.
*                                Set to zero if not required. Some slaves 
*                                such as EEPROMs need a timeout.  
*
* @return
* 
*		- A unique identifier MsgID that allows you to obtain information 
*         on the reception status by using the "Get Status" function.
*		- PIC_FAILED if I2C is not init or PIC is not master or option
*         not equal to INTERRUPT 
*
* @note
*
* Master mode and Interrupt mode only.
* Warning, if this I2C driver is used on PIC18, be careful not to touch the
* array pointed by data as long as the message has not been received.
* On pic24 there is no problem because the driver uses the 
* dynamic memory allocation and just makes a copy of the array to send.
*
*****************************************************************************/

#ifdef PICTAVE
int I2C_Read_Request(u8 adresse, u8 sizeData, u8 delay)
{
    u8 *myData = NULL;
#elif defined(ENEMEA)
int I2C_Read_Request(u8 adresse, u8 *data, u8 sizeData, u8 delay)
{
#endif /* PICTAVE | ENEMEA */
    if(I2CConfig.Status != I2C_READY)
        return PIC_FAILED;
    if((I2CConfig.Options & SLAVE) != 0)
        return PIC_FAILED;
    if(I2CConfig.BufferNbr >= 9)
        return I2C_SEND_OVERFLOW;
    
    myMsgID++;
    if(myMsgID == 0)
        myMsgID++;
    
#ifdef PICTAVE
    myData = malloc(sizeData * sizeof(u8));
    if(myData == NULL)
        return PIC_FAILED;
#endif /* PICTAVE */
    
    I2CConfig.Buffer[I2CConfig.BufferNbr].type = I2C_MASTER_READ;
    I2CConfig.Buffer[I2CConfig.BufferNbr].adress = adresse;
    I2CConfig.Buffer[I2CConfig.BufferNbr].data = myData;
    I2CConfig.Buffer[I2CConfig.BufferNbr].sizeData = sizeData;
    I2CConfig.Buffer[I2CConfig.BufferNbr].delay = delay;
    I2CConfig.MsgID[I2CConfig.BufferNbr] = myMsgID;
    I2CConfig.BufferNbr++;
    if(I2CConfig.State == BUS_READY)
    {
        I2CConfig.State = MASTER_START;
#ifdef PICTAVE
        I2C1CONbits.SEN = 1;
#elif defined(ENEMEA)
        SSP1CON2bits.SEN = 1;
#endif /* PICTAVE | ENEMEA */
    }
    return myMsgID;
}

/****************************************************************************/
/**
*
* This function allows to send to send a data followed by a restart
* to read a data. It should be used for example to read the contents
* of a register on a slave. (Master mode only).
*
* @param  - IN  Parameter : 
*                       -adresse : unsigned int which contains 
*                                  the slave's address.
*                       -sendSata : unsigned char array containing the message
*                               to be sent.
*                       -sendsizeData : size of the data to be sent
*                       -rcvData : unsigned char array in which the message 
*                               will be stored (PIC18 only)
*                       -rcvsizeData : size of the data to be received
*                       -delay : waiting time between sending 2 bytes.
*                                Set to zero if not required. Some slaves 
*                                such as EEPROMs need a timeout.  
*
* @return
* 
*		- A unique identifier MsgID that allows you to obtain information 
*         on the reception status by using the "Get Status" function.
*		- PIC_FAILED if I2C is not init or PIC is not master or option
*         not equal to INTERRUPT 
*
* @note
*
* Master mode and Interrupt mode only.
* Warning, if this I2C driver is used on PIC18, be careful not to touch the
* array pointed by data as long as the message has not been received.
* On pic24 there is no problem because the driver uses the 
* dynamic memory allocation and just makes a copy of the array to send.
*
*****************************************************************************/

#ifdef PICTAVE
int I2C_Read_Register_Request(u8 adresse, u8 *sendData, u8 sendsizeData, u8 rcvsizeData, u8 delay)
{
#elif defined(ENEMEA)
int I2C_Read_Register_Request(u8 adresse, u8 *sendData, u8 sendsizeData, u8 *rcvData, u8 rcvsizeData, u8 delay)
{
#endif /* PICTAVE | ENEMEA */

#ifdef PICTAVE
    u8 *myData = NULL;
    u8 *rcvData = NULL;
#endif /* PICTAVE */
    
    if(I2CConfig.Status != I2C_READY)
        return PIC_FAILED;
    if((I2CConfig.Options & SLAVE) != 0)
        return PIC_FAILED;
    if(I2CConfig.BufferNbr >= 9)
        return I2C_SEND_OVERFLOW;
    
    myMsgID++;
    if(myMsgID == 0)
        myMsgID++;
    
#ifdef PICTAVE
    myData = malloc(sendsizeData * sizeof(u8));
    if(myData == NULL)
        return PIC_FAILED;
    memcpy(myData, sendData, sendsizeData * sizeof(u8));
    
    rcvData = malloc(sendsizeData * sizeof(u8));
    if(rcvData == NULL)
        return PIC_FAILED;
#endif /* PICTAVE */
    
    I2CConfig.Buffer[I2CConfig.BufferNbr].type = I2C_MASTER_WRITE_WITH_RESTART;
    I2CConfig.Buffer[I2CConfig.BufferNbr].adress = adresse;
#ifdef PICTAVE
    I2CConfig.Buffer[I2CConfig.BufferNbr].data = myData;
#elif defined(ENEMEA)
    I2CConfig.Buffer[I2CConfig.BufferNbr].data = sendData;
#else
    return PIC_FAILED;
#endif /* PICTAVE | ENEMEA*/
    I2CConfig.Buffer[I2CConfig.BufferNbr].sizeData = sendsizeData;
    I2CConfig.Buffer[I2CConfig.BufferNbr].delay = delay;
    I2CConfig.MsgID[I2CConfig.BufferNbr] = myMsgID;
    
    myMsgID++;
    if(myMsgID == 0)
        myMsgID++;
    
    I2CConfig.BufferNbr++;
    I2CConfig.Buffer[I2CConfig.BufferNbr].type = I2C_MASTER_READ;
    I2CConfig.Buffer[I2CConfig.BufferNbr].adress = adresse | 1;
    I2CConfig.Buffer[I2CConfig.BufferNbr].data = rcvData;
    I2CConfig.Buffer[I2CConfig.BufferNbr].sizeData = rcvsizeData;
    I2CConfig.Buffer[I2CConfig.BufferNbr].delay = delay;
    I2CConfig.MsgID[I2CConfig.BufferNbr] = myMsgID;
    I2CConfig.BufferNbr++;
    if(I2CConfig.State == BUS_READY)
    {
        I2CConfig.State = MASTER_START;
#ifdef PICTAVE
        I2C1CONbits.SEN = 1;
#elif defined(ENEMEA)
        SSP1CON2bits.SEN = 1;
#endif /* PICTAVE | ENEMEA */
    }
    return myMsgID;
}

/****************************************************************************/
/**
*
* This function returns messages received after using the I2C_Read_Request
* and I2C_Read_Register_Request functions. If there are no messages to return,
* then it returns NULL (Master mode only).
*
* @param  - OUT  Parameter : 
*                       -adresse : unsigned int which contains 
*                                  the slave's address.
*                       -sizeData : size of the data received. 
*
* @return
* 
*		- a pointer to the received data.
*		- NULL if I2C is not init or PIC is not master or option
*         not equal to INTERRUPT or no data was received. 
*
* @note
*
* Master mode and Interrupt mode only.
* Only on PIC24: It is recommended to use the I2C_Free function to free
* the dynamic memory allocation after using the received data. Otherwise,
* you may not be able to send or receive messages anymore 
* (no memory available). Also note that the oldest messages are deleted
* every 10 seconds after they are obtained by this function.
* 
*
*****************************************************************************/
u8* I2C_Read_Buffer(u8 *adresse, int *sizeData)
{
    u8 i;
    u8 *data;
    if((I2CConfig.Options & INTERRUPT) == 0)
        return NULL;
    if(I2CConfig.RcvBufferNbr > 0)
    {
        data = I2CConfig.RcvBuffer[0].data;
        *sizeData = I2CConfig.RcvBuffer[0].sizeData;
        *adresse = I2CConfig.RcvBuffer[0].adress;
        for(i = 0; i<I2CConfig.RcvBufferNbr ; i++)
        {
            I2CConfig.RcvBuffer[i] = I2CConfig.RcvBuffer[i+1];
            I2CConfig.RcvMsgID[i] = I2CConfig.RcvMsgID[i+1];
        }
        I2CConfig.RcvBufferNbr--;
#ifdef PICTAVE
        if (I2CMemory.sizePoint > 9)
            I2C_TabRefresh();
        
        I2CMemory.PointTab[I2CMemory.sizePoint] = data;
        I2CMemory.sizePoint++;
        PointerRefresh = 10000;  
#endif /* PICTAVE */
        return data;
    }
    else
        return NULL;
}



#ifdef PICTAVE

void I2C_TabRefresh(void)
{
    u8 i;
    if(I2CMemory.sizePoint > 0)
    {
        free(I2CMemory.PointTab[0]);
        for (i = 0; i < I2CMemory.sizePoint; i++)
            I2CMemory.PointTab[i] = I2CMemory.PointTab[i+1];
        I2CMemory.sizePoint--;
    }
}

int I2C_Free(void)
{
    u8 i;
    for (i = 0; i < I2CMemory.sizePoint; i++)
        free(I2CMemory.PointTab[i]);
    I2CMemory.sizePoint = 0;
    return PIC_SUCCESS;
}

#endif /* PICTAVE */
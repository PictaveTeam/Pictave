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
* 2.01	epeu   06/05/20 bug fix
* 3.00  epeu   07/05/20 Changing the handling of incoming and outgoing
*                       messages for PIC18. Correction of a PIC18 bug.
* 3.01  epeu   10/05/20 Add Bus Speed calcul for PIC18 : 300kbs
* 3.02  epeu   23/05/20 Added error handling. Added an Address Scan function.
* 
*****************************************************************************/

/***************************** Include Files ********************************/

#include "I2C.h"

/************************** Variable Definitions ****************************/

t_I2CConfig I2CConfig;
t_I2CScan I2Cadress;
t_I2CMemory I2CMemory;
u16 PointerRefresh;
u16 timer;
u8 myMsgID;
t_I2CError I2CError[10];
u8 sizeError;
#ifdef ENEMEA
u8 *nulPointer;
#endif /* ENEMEA */

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
    static u16 adressScan = 0x02;
    u8 type;
    
#ifdef PICTAVE
    if((IFS1bits.MI2C1IF == 1) || ((IFS0bits.T2IF == 1) && ((I2CConfig.State == DELAY_DA) || (I2CConfig.State == DELAY_INTER))))
    {
#elif defined(ENEMEA)
    if((SSP1IF == 1) || ((TMR2IF == 1) && ((I2CConfig.State == DELAY_DA) || (I2CConfig.State == DELAY_INTER) || (I2CConfig.State == DELAY_TIMEOUT))))
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
            case I2C_SCAN:
                if(SSP1CON2bits.ACKSTAT==0)
                {
                    I2Cadress.Adress[I2Cadress.NbrAdress] = adressScan;
                    I2Cadress.NbrAdress++;
                }
                adressScan+=2;
                if((adressScan > 0xFE)||(I2Cadress.NbrAdress >= I2Cadress.Max))
                {
#ifdef PICTAVE
                    I2C1CONbits.PEN = 1;
#elif defined(ENEMEA)
                    SSP1CON2bits.PEN = 1;
#endif /* PICTAVE | ENEMEA */
                    I2CConfig.State = MASTER_STOP;
                    adressScan = 0x02;
                }
                else
                {
#ifdef PICTAVE
                    I2C1CONbits.SEN = 1;
#elif defined(ENEMEA)
                    SSP1CON2bits.SEN = 1;
#endif /* PICTAVE | ENEMEA */
                    I2CConfig.State = MASTER_START;
                    I2CConfig.Buffer[0].adress = adressScan;
                }
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
                        t_mem Pointeur = I2CConfig.Buffer[0].data;
                        u8 *point;
                        point = *Pointeur;
                        SSP1BUF = point[i];
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
                        t_mem Pointeur = I2CConfig.Buffer[0].data;
                        u8 *point;
                        point = *Pointeur;
                        SSP1BUF = point[i];
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
                    I2C1CONbits.PEN = 1;
#elif defined(ENEMEA)
                    SSP1CON2bits.PEN = 1;
#endif /* PICTAVE | ENEMEA */
                    I2CConfig.State = MASTER_STOP;
                    I2CError[sizeError].MsgID = I2CConfig.MsgID[0];
                    I2CError[sizeError].error = I2C_NO_SLAVE;
                    sizeError++;
                    if(sizeError > 9)
                    {
                        for(i=0;i<=9;i++)
                        {
                            I2CError[i] = I2CError[i+1];
                        }
                        sizeError--;
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
                    t_mem Pointeur = I2CConfig.Buffer[0].data;
                    u8 *point;
                    point = *Pointeur;
                    point[i] = SSP1BUF;
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
                    t_mem Pointeur = I2CConfig.Buffer[0].data;
                    u8 *point;
                    point = *Pointeur;
                    point[i] = SSP1BUF;
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
#elif defined(ENEMEA)
                        getFree(I2CConfig.RcvBuffer[0].data, MEMORY_A);
#endif /* PICTAVE | ENEMEA */
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
#ifdef ENEMEA
                    I2CConfig.RcvBuffer[I2CConfig.RcvBufferNbr].memID = I2CConfig.Buffer[0].memID;
#endif /* ENEMEA */
                    I2CConfig.RcvMsgID[I2CConfig.RcvBufferNbr] = I2CConfig.MsgID[0];
                    I2CConfig.RcvBufferNbr++;
                }

                else
#ifdef PICTAVE
                {
                    free(I2CConfig.Buffer[0].data);
                }
#elif defined(ENEMEA)
                {
                    getFree(I2CConfig.Buffer[0].data, MEMORY_A);
                }
#endif /* PICTAVE | ENEMEA */
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
                    I2C1CONbits.PEN = 1;
#elif defined(ENEMEA)
                    SSP1CON2bits.SEN = 0;
                    SSP1CON2bits.PEN = 1;
#endif /* PICTAVE | ENEMEA */
                    I2CConfig.State = MASTER_STOP;
                    I2CError[sizeError].MsgID = I2CConfig.MsgID[0];
                    I2CError[sizeError].error = I2C_TIMEOUT;
                    sizeError++;
                    if(sizeError > 9)
                    {
                        for(i=0;i<=9;i++)
                        {
                            I2CError[i] = I2CError[i+1];
                        }
                        sizeError--;
                    }
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
                    if(I2CConfig.Buffer[0].type == I2C_MASTER_SCAN)
                    {
                        I2CConfig.State = I2C_SCAN;
                        goto delay_end;
                    }
                }
                else
                {
                    if(timer == 0)
                    {
#ifdef PICTAVE
                    I2C1CONbits.SEN = 0;
                    I2C1CONbits.PEN = 1;
#elif defined(ENEMEA)
                    SSP1CON2bits.SEN = 0;
                    SSP1CON2bits.PEN = 1;
#endif /* PICTAVE | ENEMEA */
                        I2CConfig.State = MASTER_STOP;
                        I2CError[sizeError].MsgID = I2CConfig.MsgID[0];
                        I2CError[sizeError].error = I2C_TIMEOUT;
                        sizeError++;
                        if(sizeError > 9)
                        {
                            for(i=0;i<=9;i++)
                            {
                                I2CError[i] = I2CError[i+1];
                            }
                            sizeError--;
                        }
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
                    I2C1CONbits.PEN = 1;
#elif defined(ENEMEA)
                    SSP1CON2bits.SEN = 0;
                    SSP1CON2bits.PEN = 1;
#endif /* PICTAVE | ENEMEA */
                    I2CConfig.State = MASTER_STOP;
                    I2CError[sizeError].MsgID = I2CConfig.MsgID[0];
                    I2CError[sizeError].error = I2C_ERROR;
                    sizeError++;
                    if(sizeError > 9)
                    {
                        for(i=0;i<=9;i++)
                        {
                            I2CError[i] = I2CError[i+1];
                        }
                        sizeError--;
                    }
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

        if(PointerRefresh > 0)
            PointerRefresh--;
        else
        {
            PointerRefresh = 10000;
        }
#ifdef PICTAVE
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

/****************************************************************************/
/**
*
* I2C address scan function. This function returns an array with 
* the addresses of the connected I2C slaves.
*
* @param  - IN/OUT Parameter : Adresses is an array
*                           where the addresses will be stored.
* 
*         - IN  Parameter : NbrMaxAdress is an unsigned char which contains
*                           the number of addresses to be searched for.
*
* @return
* 
*		- Number of adresses founded if Scan request was successful
*		- PIC_FAILED if I2C is not init or PIC is not master or option
*         not equal to INTERRUPT. PIC_FAILED if bus TIMEOUT
*
* @note None.
*
*
*****************************************************************************/

int I2C_Scan(u8 *Adresses, u8 NbrMaxAdress)
{
    int Status = I2C_PROCESSING;
    if((I2CConfig.Options & INTERRUPT) == 0)
        return PIC_FAILED;
    if(I2CConfig.Status != I2C_READY)
        return PIC_FAILED;
    
    myMsgID++;
    if(myMsgID == 0)
        myMsgID++;
    
    I2Cadress.Adress = Adresses;
    I2Cadress.Max = NbrMaxAdress;
    I2Cadress.NbrAdress = 0;
    I2Cadress.State = I2C_PROCESSING;
    if(I2CConfig.BufferNbr >= 9)
        return I2C_SEND_OVERFLOW;
    I2CConfig.Buffer[I2CConfig.BufferNbr].type = I2C_MASTER_SCAN;
    I2CConfig.Buffer[I2CConfig.BufferNbr].adress = 0x02;
    I2CConfig.Buffer[I2CConfig.BufferNbr].delay = 0;
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
    while(Status==I2C_PROCESSING)
        Status = I2C_Get_Status(myMsgID);
    if(Status == I2C_END)
        return I2Cadress.NbrAdress;
    else
        return PIC_FAILED;
}

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
*       - I2C_TIMEOUT if timeout error on bus.
*       - I2C_ERROR if connexion error.
*       - I2C_NO_SLAVE if slave not connected.   
*
* @note
*
* If an error is detected, the message is deleted. It is not taken into
* account. Valid for: I2C_ERROR, I2C_TIMEOUT, I2C_NO_SLAVE.
*
*****************************************************************************/
int I2C_Get_Status(u8 MsgID)
{
    u8 i;
    if(I2CConfig.Status != I2C_READY)
        return PIC_FAILED;
    for(i = 0; i<sizeError ; i++)
    {
        if(I2CError[i].MsgID == MsgID)
            return I2CError[i].error;
    }
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
    sizeError = 0;
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
    nulPointer = NULL;
    memory_init();
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
        int clock = (((FOSC/ (300000*4)) - 1) >= 0)?((FOSC / (300000*4)) - 1):PIC_FAILED; // 300kbs 
        if(clock==PIC_FAILED)
            return clock;
        SSP1ADD = (u8)clock; 
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
*   On pic24 the driver uses the dynamic memory allocation and just makes
*   a copy of the array to send.
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
        I2CConfig.Buffer[I2CConfig.BufferNbr].data = myData;
#elif defined(ENEMEA)
        t_mem Pointeur = getMemoryPointer(sizeData * sizeof(u8), MEMORY_A);
        I2CConfig.Buffer[I2CConfig.BufferNbr].memID = getMemoryID(Pointeur, MEMORY_A);
        if(*Pointeur == NULL)
        {
            return PIC_FAILED;
        }
        memcpy(*Pointeur, data, sizeData * sizeof(u8));
        I2CConfig.Buffer[I2CConfig.BufferNbr].data = Pointeur;
#else
        return PIC_FAILED;
#endif /* PICTAVE | ENEMEA*/
        

        I2CConfig.Buffer[I2CConfig.BufferNbr].type = I2C_MASTER_WRITE;
        I2CConfig.Buffer[I2CConfig.BufferNbr].adress = adresse;
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
*
*****************************************************************************/


int I2C_Read_Request(u8 adresse, u8 sizeData, u8 delay)
{
    u8 *myData = NULL;
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
    {
        return PIC_FAILED;
    }
    I2CConfig.Buffer[I2CConfig.BufferNbr].data = myData;
#elif defined(ENEMEA)
    t_mem Pointeur = getMemoryPointer(sizeData * sizeof(u8), MEMORY_A);
    I2CConfig.Buffer[I2CConfig.BufferNbr].memID = getMemoryID(Pointeur, MEMORY_A);
    if(*Pointeur == NULL)
    {
        return PIC_FAILED;
    }
    I2CConfig.Buffer[I2CConfig.BufferNbr].data = Pointeur;
#else
        return PIC_FAILED;
#endif /* PICTAVE | ENEMEA*/
    
    I2CConfig.Buffer[I2CConfig.BufferNbr].type = I2C_MASTER_READ;
    I2CConfig.Buffer[I2CConfig.BufferNbr].adress = adresse;
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
*
*****************************************************************************/


int I2C_Read_Register_Request(u8 adresse, u8 *sendData, u8 sendsizeData, u8 rcvsizeData, u8 delay)
{
    u8 *myData = NULL;
    u8 *rcvData = NULL;
    
    if(I2CConfig.Status != I2C_READY)
        return PIC_FAILED;
    if((I2CConfig.Options & SLAVE) != 0)
        return PIC_FAILED;
    if(I2CConfig.BufferNbr >= 8)
        return I2C_SEND_OVERFLOW;
    
    myMsgID++;
    if(myMsgID == 0)
        myMsgID++;
    
#ifdef PICTAVE
    myData = malloc(sendsizeData * sizeof(u8));
    if(myData == NULL)
    {
        return PIC_FAILED;
    }
    memcpy(myData, sendData, sendsizeData*sizeof(u8));
    I2CConfig.Buffer[I2CConfig.BufferNbr].data = myData;
#elif defined(ENEMEA)
    t_mem Pointeur = getMemoryPointer(sendsizeData * sizeof(u8), MEMORY_A);
    I2CConfig.Buffer[I2CConfig.BufferNbr].memID = getMemoryID(Pointeur, MEMORY_A);
    if(*Pointeur == NULL)
    {
        return PIC_FAILED;
    }
    memcpy(*Pointeur, sendData, sendsizeData*sizeof(u8));
    I2CConfig.Buffer[I2CConfig.BufferNbr].data = Pointeur;
#else
        return PIC_FAILED;
#endif /* PICTAVE | ENEMEA*/
    
#ifdef PICTAVE
    rcvData = malloc(rcvsizeData * sizeof(u8));
    if(rcvData == NULL)
        return PIC_FAILED;
    
    I2CConfig.Buffer[I2CConfig.BufferNbr+1].data = rcvData;
#elif defined(ENEMEA)
    Pointeur = getMemoryPointer(rcvsizeData * sizeof(u8), MEMORY_A);
    if(*Pointeur == NULL)
    {
        return PIC_FAILED;
    }
    I2CConfig.Buffer[I2CConfig.BufferNbr+1].data = Pointeur;
    I2CConfig.Buffer[I2CConfig.BufferNbr+1].memID = getMemoryID(Pointeur, MEMORY_A);
#else
    return PIC_FAILED;
#endif /* PICTAVE | ENEMEA*/
    
    
    I2CConfig.Buffer[I2CConfig.BufferNbr].type = I2C_MASTER_WRITE_WITH_RESTART;
    I2CConfig.Buffer[I2CConfig.BufferNbr].adress = adresse;
    I2CConfig.Buffer[I2CConfig.BufferNbr].sizeData = sendsizeData;
    I2CConfig.Buffer[I2CConfig.BufferNbr].delay = delay;
    I2CConfig.MsgID[I2CConfig.BufferNbr] = myMsgID;
    
    myMsgID++;
    if(myMsgID == 0)
        myMsgID++;
    
    I2CConfig.BufferNbr++;
    I2CConfig.Buffer[I2CConfig.BufferNbr].type = I2C_MASTER_READ;
    I2CConfig.Buffer[I2CConfig.BufferNbr].adress = adresse | 1;
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
#ifdef PICTAVE
u8* I2C_Read_Buffer(u8 *adresse, u8 *sizeData)
#elif defined(ENEMEA)
t_mem I2C_Read_Buffer(u8 *adresse, u8 *sizeData)
#endif /* PICTAVE | ENEMEA */
{
    u8 i;
    u8 *data;
    u8 ID;
#ifdef PICTAVE
    if((I2CConfig.Options & INTERRUPT) == 0)
        return NULL;
#elif defined(ENEMEA)
    if((I2CConfig.Options & INTERRUPT) == 0)
        return &nulPointer;
#endif /* PICTAVE | ENEMEA */
    if(I2CConfig.RcvBufferNbr > 0)
    {
#ifdef PICTAVE
        data = I2CConfig.RcvBuffer[0].data;
#elif defined(ENEMEA)
        t_mem Pointeur = getMemoryPointer(I2CConfig.RcvBuffer[0].sizeData, MEMORY_B);
        ID = getMemoryID(Pointeur, MEMORY_B);
        memcpy(*Pointeur, *I2CConfig.RcvBuffer[0].data, I2CConfig.RcvBuffer[0].sizeData);
#endif /* PICTAVE | ENEMEA */
        *sizeData = I2CConfig.RcvBuffer[0].sizeData;
        *adresse = I2CConfig.RcvBuffer[0].adress & 0xFE;
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
        return data;
        }
    else
        return NULL;
#elif defined(ENEMEA)
        if (I2CMemory.sizePoint > 9)
            I2C_TabRefresh();
        
        I2CMemory.PointTab[I2CMemory.sizePoint] = ID;
        I2CMemory.sizePoint++;
        PointerRefresh = 10000;
        return Pointeur;
        }
    else
        return &nulPointer;
#endif /* PICTAVE | ENEMEA */

}

/****************************************************************************/
/**
*
* This function frees up the memory used by old messages so that new ones
* can be written to them.
*
* @param  None. 
*
* @return None. 
*
* @note
*
* This function is used every ten seconds by an interrupt in order to delete
* old messages if the I2C_Free() function is not used.
* 
*
*****************************************************************************/

void I2C_TabRefresh(void)
{
    u8 i;
    if(I2CMemory.sizePoint > 0)
    {
#ifdef PICTAVE
        free(I2CMemory.PointTab[0]);
#elif defined(ENEMEA)
        t_mem Pointer = getMemoryFromID(I2CMemory.PointTab[0], MEMORY_B);
        getFree(Pointer, MEMORY_B);
#endif /* PICTAVE | ENEMEA */
        for (i = 0; i < I2CMemory.sizePoint; i++)
            I2CMemory.PointTab[i] = I2CMemory.PointTab[i+1];
        I2CMemory.sizePoint--;
    }
}

/****************************************************************************/
/**
*
* This function frees up all the memory used by old messages so that new
* ones can be written to them.
*
* @param  None. 
*
* @return None. 
*
* @note
*
* It is advisable to use this function frequently.
* 
*
*****************************************************************************/

int I2C_Free(void)
{
    u8 i;
#ifdef PICTAVE
    for (i = 0; i < I2CMemory.sizePoint; i++)
        free(I2CMemory.PointTab[i]);
#elif defined(ENEMEA)
    for (i = 0; i < I2CMemory.sizePoint; i++)
    {
        t_mem Pointer = getMemoryFromID(I2CMemory.PointTab[i], MEMORY_B);
        getFree(Pointer, MEMORY_B);
    }
#endif /* PICTAVE | ENEMEA */
    I2CMemory.sizePoint = 0;
    return PIC_SUCCESS;
}

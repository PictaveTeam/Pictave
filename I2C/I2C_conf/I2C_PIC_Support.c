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
* 1.01  epeu   07/05/20 Add of memory library for 8 bits microcontroller.
* 1.02  epeu   23/05/20 Add new struct for I2C library.  
* 
*****************************************************************************/

/***************************** Include Files ********************************/

#include "I2C_PIC_Support.h"

/****************************************************************************/


/****************************************************************************/
/**
*
* Use this function to start sending data.
*
* @param	adresse is the slave's address.
*
* @return
*
*		- PIC_SUCCESS returns success if the address has been sent.
*
* @note     If the slave is not on the bus,
*           the function is in an infinite loop. 
*
*
*****************************************************************************/

int I2C_Master_Start(int adresse){
#ifdef PICTAVE
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN==1);
    Nop();
    I2C1TRN = adresse;
    while(I2C1STATbits.TRSTAT==1);
    Nop();
#elif defined(ENEMEA)
    SSP1CON2bits.SEN = 1;
    while(SSP1CON2bits.SEN==1);
    Nop();
    SSP1IF=0;
    SSP1BUF = adresse;
    while(SSP1STATbits.BF==1);
    Nop();
#endif /* PICTAVE | ENEMEA */
    return PIC_SUCCESS;
}

/****************************************************************************/
/**
*
* This function makes it possible to make a repeat start
*
* @param	adresse is the slave's address.
*
* @return
*
*		- PIC_SUCCESS returns success if the address has been sent.
*
* @note     If the slave is not on the bus,
*           the function is in an infinite loop. 
*
*
*****************************************************************************/

int I2C_Master_RepeatStart(int adresse){
#ifdef PICTAVE
    I2C1CONbits.RSEN = 1;
    while(I2C1CONbits.RSEN==1);
    Nop();
    I2C1TRN = adresse;
    while(I2C1STATbits.TRSTAT==1);
    Nop();
#elif defined(ENEMEA)
    SSP1CON2bits.RSEN = 1;
    while(SSP1CON2bits.RSEN==1);
    Nop();
    SSP1IF=0;
    SSP1BUF = adresse;
    while(SSP1STATbits.BF==1);
    Nop();
#endif /* PICTAVE | ENEMEA */
    return PIC_SUCCESS;
}

/****************************************************************************/
/**
*
* This function closes the transmission on the I2C bus.
*
* @param	None.
*
* @return
*
*		- PIC_SUCCESS.
*
* @note     None.
*
*
*****************************************************************************/

int I2C_Master_Stop(void){
#ifdef PICTAVE
    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN == 1);
    Nop();
#elif defined(ENEMEA)
    SSP1CON2bits.PEN = 1;
    while(SSP1CON2bits.PEN == 1);
    Nop();
#endif /* PICTAVE | ENEMEA */
    return PIC_SUCCESS;
}

/****************************************************************************/
/**
*
* This function sends data to the slave.
*
* @param	data : 8 bits max.
*
* @return
*
*		- PIC_SUCCESS returns success if the data has been sent.
*
* @note     None. 
*
*
*****************************************************************************/

int I2C_Master_Write(int data){
#ifdef PICTAVE
    if(I2C1STATbits.ACKSTAT==0){
        I2C1TRN = data;
        while(I2C1STATbits.TRSTAT==1);
        Nop();
    }else{
        I2C1CONbits.PEN = 1;
        while(I2C1CONbits.PEN == 1);
        Nop();
    }
#elif defined(ENEMEA)
    if(SSP1CON2bits.ACKSTAT==0){
        SSP1BUF = data;
        while(SSP1STATbits.BF==1);
        Nop();
    }else{
        SSP1CON2bits.PEN = 1;
        while(SSP1CON2bits.PEN == 1);
        Nop();
    }
#endif /* PICTAVE | ENEMEA */
    return PIC_SUCCESS;
}

/****************************************************************************/
/**
*
* This function is used to read data from an I2C slave.
*
* @param	1 or 0. Send 1 for the last byte.
*
* @return
*
*		- Returns the received data
*
* @note     None. 
*
*
*****************************************************************************/

char I2C_Master_Read(unsigned char a){
#ifdef PICTAVE
    I2C1CONbits.RCEN =1;         //on lance la reception
    while(I2C1CONbits.RCEN);    //on attend la fin de la reception
    Nop();
    I2C1CONbits.ACKDT = (a)?0:1;         //on envoi 
    I2C1CONbits.ACKEN=1;         //un NOACK ou un ACK
    while(I2C1CONbits.ACKEN);    //on attend la fin du NOACK
    Nop();
    return I2C1RCV;   		 //donnée recue dans le buffer
#elif defined(ENEMEA)
    SSP1CON2bits.RCEN =1;         //on lance la reception
    while(SSP1CON2bits.RCEN);    //on attend la fin de la reception
    Nop();
    SSP1CON2bits.ACKDT = (a)?0:1;         //on envoi 
    SSP1CON2bits.ACKEN=1;         //un NOACK ou un ACK
    while(SSP1CON2bits.ACKEN);    //on attend la fin du NOACK
    Nop();
    return SSP1BUF;   		 //donnée recue dans le buffer
#endif /* PICTAVE | ENEMEA */
}

/****************************************************************************/
/**
*
* This function allows the calculation of the prescaler 
* for the timer used by the I2C.
*
* @param	Parameter IN :
*                   - period : period in second. 
*                   - presc  : the prescaler. 
*
* @param	Parameter IN/OUT :  
*                   - prUsed : the value of the comparator register to be used.
*                   - prescUsed  : the prescaler used. 
*                   - min  : the minimum approaching the desired value.
*
* @return   None.
*
* @note     None. 
*
*
*****************************************************************************/

void I2C_timer_prescaler(float period, u16 *prUsed, u16 presc, u16 *prescUsed, float *min)
{
    float minB;
    double pr_f;
    u16 pr;
    long double x;
    x = FTIMER / ((double)presc);
#ifdef PICTAVE
    if((x*period)<=(double)REGISTER_SIZE_16)
    {
#elif defined(ENEMEA)
    if((x*period)<=(double)REGISTER_SIZE_8)
    {
#endif /* PICTAVE | ENEMEA */
        pr_f = x * period;
        pr = (u16)pr_f;
        minB = (pr>pr_f)? (pr-pr_f):(pr_f-pr);
        if(minB < *min)
        {
            *prUsed = pr;
            *prescUsed = presc;
            *min = minB;
        }
        pr++;
        minB = (pr>pr_f)? (pr-pr_f):(pr_f-pr);
        if(minB < *min)
        {
            *prUsed = pr;
            *prescUsed = presc;
            *min = minB;
        }
    }
}

#ifdef BETA
int I2C_EEPROM_Write(char adresse,int EEadresse, char data)
{
    if (EEadresse <= MAX_EEPROM_ADRESSE)
    {
        I2C_Master_Start(adresse);
        __delay_ms(5);
#ifdef EEPROM_24LC512
        I2C_Master_Write((char)(EEadresse>>8));
        __delay_ms(5);
#endif /* EEPROM_24LC512 */
        I2C_Master_Write((char)(EEadresse));
        __delay_ms(5);
        I2C_Master_Write(data);
        __delay_ms(5);
        I2C_Master_Stop();
        __delay_ms(10);
        return PIC_SUCCESS;
    }
    return PIC_FAILED;
}
int I2C_EEPROM_Read(char adresse, int EEadresse)
{
    u8 stac;
    if (EEadresse <= MAX_EEPROM_ADRESSE)
    {
        I2C_Master_Start(adresse);
        __delay_ms(5);
#ifdef EEPROM_24LC512
        I2C_Master_Write((char)(EEadresse>>8));
        __delay_ms(5);
#endif /* EEPROM_24LC512 */
        I2C_Master_Write((char)(EEadresse));
        __delay_ms(5);
        I2C_Master_RepeatStart((char)(adresse+1));
        __delay_ms(5);
        stac = I2C_Master_Read(0);
        __delay_ms(5);
        I2C_Master_Stop();
        __delay_ms(10);
        return stac;
    }
    return PIC_FAILED;
}
#endif /* ENEMEA */

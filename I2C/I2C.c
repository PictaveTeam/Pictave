#include "I2C.h"

void I2C_Master_Init(void){
    I2C1CON = 0x9000; 
    I2C1STAT = 0;
    I2C1ADD = 0;
    I2C1BRG = 300;
}
char I2C_Master_Read(unsigned char a){
    I2C1CONbits.RCEN =1;         //on lance la reception
    while(I2C1CONbits.RCEN);    //on attend la fin de la reception
    Nop();
    I2C1CONbits.ACKDT = (a)?0:1;         //on envoi 
    I2C1CONbits.ACKEN=1;         //un NOACK ou un ACK
    while(I2C1CONbits.ACKEN);    //on attend la fin du NOACK
    Nop();
    return I2C1RCV;   		 //donnée recue dans le buffer
}
void I2C_Master_Stop(void){
    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN == 1);
    Nop();
}
void I2C_Master_Write(int data){
    if(I2C1STATbits.ACKSTAT==0){
        I2C1TRN = data;
        while(I2C1STATbits.TRSTAT==1);
        Nop();
    }else{
        I2C1CONbits.PEN = 1;
        while(I2C1CONbits.PEN == 1);
        Nop();
    }
}
void I2C_Master_Start(int adresse){
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN==1);
    Nop();
    I2C1TRN = adresse;
    while(I2C1STATbits.TRSTAT==1);
    Nop();
}
void I2C_Master_RepeatStart(int adresse){
    I2C1CONbits.RSEN = 1;
    while(I2C1CONbits.RSEN==1);
    Nop();
    I2C1TRN = adresse;
    while(I2C1STATbits.TRSTAT==1);
    Nop();
}
t_STATUS I2C_EEPROM_Write(char adresse,int EEadresse, char data)
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
        return OK;
    }
    return ERROR;
}
t_STATUS I2C_EEPROM_Read(char adresse, int EEadresse)
{
    char stac;
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
    return ERROR;
}

/* Library contain "while", it cannot used for speed processing.
 maximum duration of a function: 32 ms(write or read an EEPROM */

#ifndef I2C_H
#define	I2C_H

#include <xc.h>
#include "p24FJ64GA006.h"


#define ERROR                   (-1)
#define OK                        0
#define MAX_EEPROM_ADRESSE    (0xFF) /* For 24LC08b can be changed if others EEPROM used*/
#define EEPROM                (0xA0) /* /!\ Warning: The EEPROM connected in Pictave take all adresses A0h-AFh /!\ */

typedef signed int t_STATUS;

void I2C_Master_Init(void);
char I2C_Master_Read(unsigned char a);
void I2C_Master_Stop(void);
void I2C_Master_Write(int data);
void I2C_Master_Start(int adresse);
void I2C_Master_RepeatStart(int adresse);
t_STATUS I2C_EEPROM_Write(char adresse,int EEadresse, char data);/* adresse: slave adress, EEadresse: 8bit Register adress, data: writing data */
t_STATUS I2C_EEPROM_Read(char adresse, int EEadresse);/* adresse: slave adress, EEadresse: 8bit Register adress */

#endif	/* I2C_H */

/*-------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef DELAY_LIBRARY

#include "timer.h"
/* __delay_ms() from timer.h is used. It must be replaced by library libpic30.h that contain the same function */
/* you can create your own function */

#else

#include <libpic30.h>
#define    FCY    16000000UL /* to change with your configuration in Hz */

#endif /* DELAY_LIBRARY */

/* Exemple of use:
int main(int argc, char** argv) {
    initialisation();
    init_timer_1(0.001); // 1 ms
    I2C_Master_Init();
    led_octet(0); // clear LEDs
    I2C_EEPROM_Write(EEPROM, 0x30, 0x80); // 0x80 is written into EEPROM at the 0x30 adress
    I2C_EEPROM_Write(EEPROM, 0x22, 0xFF);
    led_octet((char)(I2C_EEPROM_Read(EEPROM, 0x30))); // LEDs print data at the 0x30 adress
    while(1);
}
*/

/*-------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------|END HEADER FILE|--------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------------------*/


/****************************************************************************/
/**
*
* @file config.c
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

/***************************** Include Files ********************************/

#include "config.h"

/****************************************************************************/
/**
*
* Initializes hardware functions of the Pictave card
*
* @param	None.
*
* @return
*
*		- PIC_SUCCESS if initialization was successful
*		- PIC_FAILED if it is not a pictave card.
*
* @note     None.
*
*
*****************************************************************************/
int initPictave(void)
{
#ifdef PICTAVE
    /*   Connector OUTPUT TOR  */
    
    TRISDbits.TRISD0=0;/* S0  */
    TRISDbits.TRISD1=0;/* S1  */
    TRISDbits.TRISD2=0;/* S2  */
    TRISDbits.TRISD3=0;/* S3  */
    TRISEbits.TRISE0=0;/* S4  */
    TRISEbits.TRISE1=0;/* S5  */
    TRISEbits.TRISE2=0;/* S6  */
    TRISEbits.TRISE3=0;/* S7  */
    
    /*   Connector INPUT TOR  */
    
    TRISDbits.TRISD4=1;/*  In0  */
    TRISDbits.TRISD5=1;/*  In1  */
    TRISDbits.TRISD6=1;/*  In2  */
    TRISDbits.TRISD7=1;/*  In3  */
    TRISDbits.TRISD8=1;/*  In4  */
    TRISDbits.TRISD9=1;/*  In5  */
    TRISDbits.TRISD10=1;/* In6  */
    TRISDbits.TRISD11=1;/* In7  */
    
    /*   Connector EXTEND  */
    
    TRISBbits.TRISB6=0;//  Ext0  */
    TRISBbits.TRISB7=0;//  Ext1  */
    TRISBbits.TRISB9=0;//  Ext2  */
    TRISBbits.TRISB10=0;// Ext3  */
    TRISBbits.TRISB15=0;// Ext4  */
    
    /*   Pins CONFIGURATIONS  */
    
    TRISEbits.TRISE5=1;/*   C0  */
    TRISEbits.TRISE6=1;/*   C1  */
    TRISEbits.TRISE7=1;/*   C2  */
    TRISBbits.TRISB11=1;/*  C3  */
    TRISBbits.TRISB12=1;/*  C4  */
    TRISBbits.TRISB13=1;/*  C5  */
    TRISCbits.TRISC13=1;/*  C6  */
    TRISCbits.TRISC14=1;/*  C7  */
    
    /*   LED RED AND GREEN  */
    
    TRISFbits.TRISF0=0; /*  Led Green  */
    TRISFbits.TRISF1=0; /*  Led Red    */
    
    /*   UART_1 */
    
    TRISFbits.TRISF2=1; /*  RX */
    TRISFbits.TRISF3=1; /*  TX */
    
    /*   UART_2 */
    
    TRISFbits.TRISF4=1;/*  RX  */
    TRISFbits.TRISF5=1;/*  TX  */
    
    /*   I2C1 */
    
    TRISGbits.TRISG2=0; /* SCL */
    TRISGbits.TRISG3=1; /* SDA */
    
    TRISFbits.TRISF6=0; /*  LATCH_DAC */
    
    AD1PCFG=0xFFF3; /* All pins to digital but not AN2 and AN3 */
    AD1CON1bits.ADON=0; /* analog disable */
    return PIC_SUCCESS;
#else
    return PIC_FAILED;
#endif /* PICTAVE */
}

/****************************************************************************/
/**
*
* @file timer.c
* 
*
* This file contains the implementation of the interface functions for timer 1.
* This library has only been experimented with Pictave for the moment.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date	    Changes
* ----- ------ -------- ----------------------------------------------
* 1.00	epeu   21/03/20 First Release
* 
*****************************************************************************/

/***************************** Include Files ********************************/

#include "timer.h"

/************************** Variable Definitions ****************************/

t_timerInit PicTimer;

/****************************************************************************/


#ifdef PICTAVE

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
* Valid only on PIC24FJ64GA006
*
*****************************************************************************/
void __attribute__((interrupt, auto_psv)) _T1Interrupt ( void )
{
    TMR1=0;
    u8 i;
    for(i = 0;i<sizeof(PicTimer.timer);i++)
    {
        if(PicTimer.timer[i]!=0)
            PicTimer.timer[i]--;
    }
    IFS0bits.T1IF=0;
}

#endif /* PICTAVE */

/****************************************************************************/
/**
*
* Initializes the timer 1 device with with an interrupt cycle whose period
* can be adjusted.
*
* @param	period is a real that defines the period of the interrupt cycle.
*
* @return
*
*		- PIC_SUCCESS if initialization was successful
*		- PIC_FAILED if the period is not possible because
*		  the inputclock frequency is not divisible with an acceptable
*		  amount of error.
*
* @note
*
* If there is an error with all your periods (PIC_FAILED), initialize the
* timer registers yourself. You can then use the timerEnable() function.
*
*****************************************************************************/
int timerInit(float period)
{
    memset(&PicTimer,0,sizeof(PicTimer));
#ifdef PICTAVE
    float min;
    float minB;
    double pr_f;
    u16 pr;
    u16 prUsed;
    u16 prescUsed = PRESC_256;
    u16 presc = PRESC_256;
    long double x;
    x = FCY / ((double)presc);
    if((x*period) <= (double)REGISTER_SIZE_16)
    {
        pr_f = x * period;
        pr = (u16)pr_f;
        min = (pr>pr_f)? (pr-pr_f):(pr_f-pr);
        prUsed = pr;
        pr++;
        minB = (pr>pr_f)? (pr-pr_f):(pr_f-pr);
        if(minB<min)
        {
            prUsed = pr;
            min = minB;
        }
    }
    else
        return PIC_FAILED;
    presc = PRESC_64;
    x = FCY / ((double)presc);
    if((x*period)<=sizeof(u16))
    {
        pr_f = x * period;
        pr = (u16)pr_f;
        minB = (pr>pr_f)? (pr-pr_f):(pr_f-pr);
        if(minB<min)
        {
            prUsed = pr;
            prescUsed = PRESC_64;
            min = minB;
        }
        pr++;
        minB = (pr>pr_f)? (pr-pr_f):(pr_f-pr);
        if(minB<min)
        {
            prUsed = pr;
            prescUsed = PRESC_64;
            min = minB;
        }
    }
    presc = PRESC_8;
    x = FCY / ((double)presc);
    if((x*period)<=sizeof(u16))
    {
        pr_f = x * period;
        pr = (u16)pr_f;
        minB = (pr>pr_f)? (pr-pr_f):(pr_f-pr);
        if(minB<min)
        {
            prUsed = pr;
            prescUsed = PRESC_8;
            min = minB;
        }
        pr++;
        minB = (pr>pr_f)? (pr-pr_f):(pr_f-pr);
        if(minB<min)
        {
            prUsed = pr;
            prescUsed = PRESC_8;
            min = minB;
        }
    }
    presc = PRESC_1;
    x = FCY / ((double)presc);
    if((x*period)<=sizeof(u16))
    {
        pr_f = x * period;
        pr = (u16)pr_f;
        minB = (pr>pr_f)? (pr-pr_f):(pr_f-pr);
        if(minB<min)
        {
            prUsed = pr;
            prescUsed = PRESC_1;
            min = minB;
        }
        pr++;
        minB = (pr>pr_f)? (pr-pr_f):(pr_f-pr);
        if(minB<min)
        {
            prUsed = pr;
            prescUsed = PRESC_1;
            min = minB;
        }
    }
    switch(prescUsed)
    {
        case PRESC_256:
            T1CONbits.TCKPS=0b11;
            break;
        case PRESC_64:
            T1CONbits.TCKPS=0b10;
            break;
        case PRESC_8:
            T1CONbits.TCKPS=0b01;
            break;
        case PRESC_1:
            T1CONbits.TCKPS=0b00;
            break;
        default:
            return PIC_FAILED;
            break;
    }
    PR1=(u16)prUsed;
    T1CONbits.TON=0;            /* timer 1 disable */
    T1CONbits.TSIDL=0;          /* state in idle mode */
    T1CONbits.TGATE=0;          /* disabled accumulation */
    T1CONbits.TCS=0;            /* internal clock */
    IFS0bits.T1IF=0;            /* overflow cleared */
    IEC0bits.T1IE=1;            /* timer 1 interrupt enable */
    IPC0bits.T1IP=1;            /* priority 1(low) */
#else
    return PIC_FAILED;
#endif /* PICTAVE */
    timerEnable();
    return PIC_SUCCESS;
}

/****************************************************************************/
/**
*
* This function Enable timer 1.
*
* @param	None.
*
* @return
*
*		- PIC_SUCCESS if timer enable
*		- PIC_FAILED if the pic used is not a PIC24FJ64GA006.
*
* @note
*
* Configure timer with function timerInit().
*
*****************************************************************************/
int timerEnable(void)
{
    memset(&PicTimer.timer,0,sizeof(PicTimer.timer));
#ifdef PICTAVE
    IFS0bits.T1IF=0;
    TMR1=0;
    T1CONbits.TON=1;
#else
    return PIC_FAILED;
#endif /* PICTAVE */
    PicTimer.State = TIMER_READY;
    return PIC_SUCCESS;
}

/****************************************************************************/
/**
*
* This function Disable timer 1.
*
* @param	None.
*
* @return
*
*		- PIC_SUCCESS if timer disable
*		- PIC_FAILED if the pic used is not a PIC24FJ64GA006.
*
* @note None.
*
*
*****************************************************************************/
int timerDisable(void)
{
#ifdef PICTAVE
    T1CONbits.TON=0; /* timer 1 disable */
#else
    return PIC_FAILED;
#endif /* PICTAVE */
    PicTimer.State = TIMER_NOT_READY;
    return PIC_SUCCESS;
}

/****************************************************************************/
/**
*
* This function return state of your timers. Can be used for a Delay 
* function as in function __delay_ms().
*
* @param	Timer number. (0-9)
*
* @return
*
*		- timer state
*
* @note
*
* Configure timer with function timerInit().
*
*****************************************************************************/
u16 getTimer(u8 timer)
{
    return PicTimer.timer[timer];
}

/****************************************************************************/
/**
*
* This function setting values for your timers. Can be used for a Delay 
* function as in function __delay_ms().
*
* @param	timer: Timer number. (0-9)
* @param	value: delay number.
*
* @return
*
*		- PIC_SUCCESS if timer disable
*		- PIC_FAILED if the timer is not enable.
*
* @note
*
* Configure timer with function timerInit().
*
*****************************************************************************/
int setTimer(u8 timer, u16 value)
{
    if(PicTimer.State == TIMER_NOT_READY)
        return PIC_FAILED;
    PicTimer.timer[timer] = value;
    return PIC_SUCCESS;
}

/****************************************************************************/
/**
*
* This function take a break of a determined duration in ms
*
* @param	break duration
*
* @return None.
*
* @note
*
* Configure timer with function timerInit().
* If you have set a time period other than ms, then this function will 
* use your time period. 
*
*****************************************************************************/
void __delay_ms(u16 t)
{
    if(PicTimer.State == TIMER_READY)
    {
        setTimer(0,t);
        while(getTimer(0));
    }
}

//end file
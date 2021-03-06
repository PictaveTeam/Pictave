/****************************************************************************/
/**
*
* @file timer.c
* 
*
* This file contains the implementation of the interface functions for timer 1
* or timer 0 (PIC18F).
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date	    Changes
* ----- ------ -------- ----------------------------------------------
* 1.00	epeu   21/03/20 First Release
* 1.01  epeu   22/03/20 Delete an include because already include in xc.h
* 2.00  epeu   06/04/20 Add support of PIC18F45K22
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
#ifdef ENEMEA
void timer_ISR(void)
{
    if(TMR0IF)
    {
        TMR0 = PicTimer.Pr;
        u8 i;
        for(i = 0;i<sizeof(PicTimer.timer);i++)
        {
            if(PicTimer.timer[i]!=0)
                PicTimer.timer[i]--;
        } 
        TMR0IF = 0;
    }
}
#endif /* ENEMEA */
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
    float min;
    float minB;
    double pr_f;
    u16 pr;
    u16 prUsed;
    u16 prescUsed = PRESC_256;
    u16 presc = PRESC_256;
    long double x;
#ifdef PICTAVE
    x = FCY / ((double)presc);
#elif defined(ENEMEA)
    x = FTIMER / ((double)presc);
#else
    return PIC_FAILED;
#endif /* PICTAVE | ENEMEA */
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
    
#ifdef PICTAVE
    prescaler(period, &prUsed, PRESC_64, &prescUsed, &min);
    prescaler(period, &prUsed, PRESC_8, &prescUsed, &min);
    prescaler(period, &prUsed, PRESC_1, &prescUsed, &min);
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
#endif /* PICTAVE */
#ifdef ENEMEA
    T0CONbits.TMR0ON=0;         /* timer 0 disable */
    T0CONbits.T08BIT=0;         /* timer 0 in 16 bits mode */
    T0CONbits.PSA=0;            /* timer 0 prescaler enable */
    T0CONbits.T0CS=0;           /* internal clock */
    T0CONbits.T0SE=0;           /* increment on low to high */
    TMR0IE = 1;
    
    prescaler(period, &prUsed, PRESC_128, &prescUsed, &min);
    prescaler(period, &prUsed, PRESC_64, &prescUsed, &min);
    prescaler(period, &prUsed, PRESC_32, &prescUsed, &min);
    prescaler(period, &prUsed, PRESC_16, &prescUsed, &min);
    prescaler(period, &prUsed, PRESC_8, &prescUsed, &min);
    prescaler(period, &prUsed, PRESC_4, &prescUsed, &min);
    prescaler(period, &prUsed, PRESC_2, &prescUsed, &min);
    switch(prescUsed)
    {
        case PRESC_256:
            T0CONbits.T0PS=0b111;
            break;
        case PRESC_128:
            T0CONbits.T0PS=0b110;
            break;
        case PRESC_64:
            T0CONbits.T0PS=0b101;
            break;
        case PRESC_32:
            T0CONbits.T0PS=0b100;
            break;
        case PRESC_16:
            T0CONbits.T0PS=0b011;
            break;
        case PRESC_8:
            T0CONbits.T0PS=0b010;
            break;
        case PRESC_4:
            T0CONbits.T0PS=0b001;
            break;
        case PRESC_2:
            T0CONbits.T0PS=0b000;
            break;
        default:
            return PIC_FAILED;
            break;
    }
    PicTimer.Pr = (u16)(REGISTER_SIZE_16 - prUsed);
#endif /* ENEMEA */
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
#elif defined(ENEMEA)
    TMR0IF=0;
    TMR0 = PicTimer.Pr;
    T0CONbits.TMR0ON=1;
#else
    return PIC_FAILED;
#endif /* PICTAVE | ENEMEA */
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
#elif defined(ENEMEA)
    T0CONbits.TMR0ON=0; /* timer 0 disable */
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
void delay(u16 t)
{
    if(PicTimer.State == TIMER_READY)
    {
        setTimer(0,t);
        while(getTimer(0));
    }
}

void prescaler(float period, u16 *prUsed, u16 presc, u16 *prescUsed, float *min)
{
    float minB;
    double pr_f;
    u16 pr;
    long double x;
    #ifdef PICTAVE
    x = FCY / ((double)presc);
    #elif defined(ENEMEA)
    x = FTIMER / ((double)presc);
    #endif /* PICTAVE | ENEMEA */
    if((x*period)<=(double)REGISTER_SIZE_16)
    {
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

//end file
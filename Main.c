#include <msp430.h>
//#include "msp430g2231.h"

/**
 * Main.c
 *
 * No watch dog
 * On board green() blink on button push
 * On board red() blink on timer using interrupt timer
 *
 *
 */

#define GREEN       (BIT6)
#define RED         (BIT0)
#define BUTTON      (BIT3)

#define BLINKY_DELAY_MS  (50)
#define ONE_SECOND   (10000/1)
#define buttonPress (P1IN & BUTTON)!=BUTTON

unsigned int i; //Counter
unsigned int OFCount;
unsigned int LEDCount;

unsigned int debounce();
void delayMS(int msecs);
void initTimer_A(void);

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; //Turn off watch dog timer
    P1DIR |= RED + GREEN;   // configure P1.0 P1.6 as output
    P1REN |= BUTTON;   // configure P1.3 with resistor
    P1OUT |= BUTTON;   // configure P1.3 as output
    //    P1IE |= BUTTON;    // set P1.3 as the interrupt pin
    //    P1IFG &= ~BUTTON;
    //Set MCLK = SMCLK = 1MHz
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    initTimer_A();
    _enable_interrupt();

    TACCR0 = ONE_SECOND; //Start Timer
    while(1)
    {
        //flip LED after each count
        if(OFCount>0)
        {
            P1OUT ^= RED; //Drive P1.0 HIGH or LOW - LED1 ON/OFF

            OFCount = 0;
            TACCR0 = ONE_SECOND; //Start Timer, Compare value for Up Mode to get 1ms delay per loop
        }
    }
}

    void initTimer_A(void)
    {
        //Timer0_A3 Configuration
        TACCR0 = 0; //Initially, Stop the Timer
        TACCTL0 |= CCIE; //Enable interrupt for CCR0.
        TACTL = TASSEL_2 + ID_0 + MC_1; //Select SMCLK, SMCLK/1, Up Mode
    }

    void delayMS(int msecs)
    {
        OFCount = 0; //Reset Over-Flow counter
        TACCR0 = 0; //Stop Timer

        TACCR0 = (msecs*10000)-1; //Start Timer, Compare value for Up Mode to get 1ms delay per loop
        //Total count = TACCR0 + 1. Hence we need to subtract 1.
        i = 0;
        //while(i<=msecs)i++;
        while(OFCount == 0);

    }

    //Timer ISR
    #pragma vector = TIMER0_A0_VECTOR
    __interrupt void Timer_A_CCR0_ISR(void)
    {
        OFCount++; //Increment Over-Flow Counter
    }

unsigned int debounce()
{
    unsigned int output = 1;
    for(i=3;i >= 1;i--)
    {
        delayMS(5);
        if(buttonPress)
        {
            output = 0;
        }
    }
    return output;
}




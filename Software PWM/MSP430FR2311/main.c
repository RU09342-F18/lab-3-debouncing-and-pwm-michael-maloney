#include <msp430.h>


/** Michael Maloney
 * main.c
 * 10/5/18
 *
 * For some reason it works only some of the time.
 * Only works after mashing reset sometimes.
 * Otherwise works exactly the same as G2553
 */

int on = 500; // on ccr value
int off = 500; // off ccr value
int onoff = 0; //on off "bool" to switch between both

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    TB0CCTL0 = CCIE;            // Flag enable


    TB0CCR0 = 500; //(1MHz)(1/1kHz) = 1000, so 500 on and 500 off creates 50% duty cycle @ 1kHz
    TB0CTL = TBSSEL_2 + MC_1; //Timer A0 = smclock, up count mode, divider of 4


    P1DIR = BIT0;                      // P1.0 and 1.6 set output, rest set input
    P2DIR = BIT0;

    P1OUT =  BIT1 + BIT0;                     // P1.3 pullup
    P1REN |= BIT1;                            // P1.3 resistor en
    P1IE |= BIT1;                             // P1.3 interrupt enabled
    P1IES |= BIT1;                            // P1.3 Hi/lo edge
    P1IFG &= ~BIT1;                           // P1 flag set off @ Bit 3

    __bis_SR_register(LPM0_bits + GIE);       // MSP enters LPM0 on interrupt
    }

#pragma vector=TIMER0_B0_VECTOR               // Interrupt based on timer B0.0
__interrupt void BUTTON(void){
    if (on == 1000){                           // If the on ccr value is at max i.e off
        P1OUT &= ~BIT0;                         // the LED is set completely off
    } else {
        P1OUT ^= BIT0;                        // Otherwise the LED toggles at a specific duty cycle
    }
    if (onoff == 0){                          // If onoff is off/0
    TB0CCR0 = off;                              //CCR0 is set to the off ccr value
    } else {
    TB0CCR0 = on;                             // Otherwise it is set to the on ccr value
    }
    onoff = ~onoff;                           // onoff toggles on and off (0xffff / 0x0000);
}

#pragma vector=PORT1_VECTOR                    //Button interrupt
__interrupt void Port1(void){
          on = on - 100;                      // on ccr value subtracted by 100, or 10% of 1000 total
          off = off + 100;                   //  off value added by 100 to keep their sum = 1000
          if (on < 0){                      // Once overflow,
              on = 1000;                     //on and off set to their values to have "0%" duty cycle
              off = 0;
          }
          P2OUT ^= BIT0;                    // LED P2.0 set toggled for debugging
          P1IFG &= ~BIT1;                   // Button flag turned off
}

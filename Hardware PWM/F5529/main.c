#include <msp430.h>


/** Michael Maloney
 * main.c
 * 10/5/18
 */

int ccr1 = 500;  //global ccr1 variable

int main(void) {
    WDTCTL = WDTPW + WDTHOLD; // Hold wdt
    P1DIR |= BIT0; //P1.0 and 4.7 set to output
    P4DIR |= BIT7;
    P1SEL |= BIT0; // P1.0 set as PWM output

    P1OUT =  BIT1;                            // P1.1 pullup
    P1REN |= BIT1;                            // P1.1 resistor en
    P1IE |= BIT1;                             // P1.1 interrupt enabled
    P1IES |= BIT1;                            // P1.1 Hi/lo edge
    P1IFG &= ~BIT1;                           // P1 flag set off @ Bit 1

    TA0CCR0 = 1000; //1kHz total period
    TA0CCTL1 = OUTMOD_7; //reset/set
    TA0CCR1 = ccr1; //ccr1 = 500 = 50% duty cycle
    TA0CTL = TASSEL_2 + MC_1; // smclk up mode

   __bis_SR_register(LPM0_bits + GIE); // MSP430 enters LPM0 on interrupt
}

#pragma vector=PORT1_VECTOR            // P1 button interrupt action
__interrupt void button(void){
    P1OUT ^= BIT0;                      //Toggles P1.0 for debugging
    if (ccr1 == 1000){                  // If ccr1 is at max value (CCR0 value)
        ccr1 = 0;                           // set back to minimum value
    } else {
        ccr1 = ccr1 + 100;              // Else increment ccr1 by 100, or 10% of CCR0
    }
    TA0CCR1 = ccr1;                     // Set TA0CCR1 to the ccr1 value for duty cycle
    P1IFG = ~BIT1;                      // Turn off interrupt flag
}

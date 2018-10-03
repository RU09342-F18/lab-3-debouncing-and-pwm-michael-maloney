#include <msp430.h> 


/** Michael Maloney
 * main.c
 * 10/3/18
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	TA0CCTL0 = CCIE; // Enable flags for timer A0
	TA0CCR0 = 18750; //(4MHz / 8)(.15sec)  

	P1DIR = BIT0;                             // P1.0 set output, rest set input
	P1OUT =  BIT3;                            // P1.3 set, else reset
	P1REN |= BIT3;                            // P1.3 pullup
	P1IE |= BIT3;                             // P1.3 interrupt enabled
	P1IES |= BIT3;                            // P1.3 Hi/lo edge
	P1IFG &= ~BIT3;                           // P1 flag set off @ Bit 3
	
	__bis_SR_register(LPM0_bits + GIE);       // MSP enters LPM0 on interrupt
}

#pragma vector=PORT1_VECTOR                 // Interupt on P1 hi/lo press
__interrupt void BUTTON(void){
    
    TA0CTL = TASSEL_2 + MC_1 + ID_3; //Timer A0 = smclock, up count mode, divider of 8
    P1IFG &= ~BIT3; // P1 flag turned off
    
}

#pragma vector=TIMER0_A0_VECTOR     // Timer A0 flag interrupt
__interrupt void TimerA0(void){
    P1OUT  ^= BIT0;                 // P1.0 LED toggled
    TA0CTL = MC_0;                  // Timer turned off
    TA0CTL = TACLR;                 // Timer cleared
}


#include <msp430.h> 


/** Michael Maloney
 * main.c
 * 10/5/18
 */
int on = 125; // on ccr value
int off = 125; // off ccr value
int onoff = 0; //on off "bool" to switch between both

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	TA0CCTL0 = CCIE;            // Flag enable


	TA0CCR0 = 125; //(4MHz / 4)(1/1kHz) = 250, so 125 on and 125 off creates 50% duty cycle @ 1kHz
	TA0CTL = TASSEL_2 + MC_1 + ID_2; //Timer A0 = smclock, up count mode, divider of 4


	P1DIR = BIT0 + BIT6;                      // P1.0 and 1.6 set output, rest set input


	P1OUT =  BIT3;                            // P1.3 pullup
	P1REN |= BIT3;                            // P1.3 resistor en
	P1IE |= BIT3;                             // P1.3 interrupt enabled
	P1IES |= BIT3;                            // P1.3 Hi/lo edge
	P1IFG &= ~BIT3;                           // P1 flag set off @ Bit 3

	__bis_SR_register(LPM0_bits + GIE);       // MSP enters LPM0 on interrupt
	}

#pragma vector=TIMER0_A0_VECTOR               // Interrupt based on timer A0.0
__interrupt void BUTTON(void){
    if (on == 250){                           // If the on ccr value is at max i.e off               
        P1OUT &= ~BIT0;                         // the LED is set completely off
    } else {
        P1OUT ^= BIT0;                        // Otherwise the LED toggles at a specific duty cycle
    }
    if (onoff == 0){                          // If onoff is off/0
    TA0CCR0 = off;                              //CCR0 is set to the off ccr value
    } else {
    TA0CCR0 = on;                             // Otherwise it is set to the on ccr value
    }
    onoff = ~onoff;                           // onoff toggles on and off (0xffff / 0x0000);
}

#pragma vector=PORT1_VECTOR                    //Button interrupt
__interrupt void Port1(void){
          on = on - 25;                      // on ccr value subtracted by 25, or 10% of 250 total
          off = off + 25;                   //  off value added by 25 to keep their sum = 250
          if (on < 0){                      // Once overflow,
              on = 250;                         //on and off set to their values to have "0%" duty cycle
              off = 0;
          }
          P1OUT ^= BIT6;                    // LED P1.6 set toggled for debugging
          P1IFG &= ~BIT3;                   // Button flag turned off
}

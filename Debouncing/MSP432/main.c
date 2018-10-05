/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2013, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP432 CODE EXAMPLE DISCLAIMER
 *
 * MSP432 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see http://www.ti.com/tool/mspdriverlib for an API functional
 * library & https://dev.ti.com/pinmux/ for a GUI approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP432P401 Demo - Timer0_A3, Toggle P1.0, CCR0 Up Mode ISR, DCO SMCLK
//
//  Description: Toggle P1.0 using software and TA_0 ISR. Timer0_A is
//  configured for up mode, thus the timer overflows when TAR counts
//  to CCR0. In this example, CCR0 is loaded with 50000.
//  ACLK = n/a, MCLK = SMCLK = TACLK = default DCO ~3MHz
//
//
//           MSP432P401x
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |               |
//        |           P1.0|-->LED
//
//
//   William Goh
//   Texas Instruments Inc.
//   Oct 2016 (updated) | November 2013 (created)
//   Built with CCSv6.1, IAR, Keil, GCC
//******************************************************************************
#include "msp.h"

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW |             // Stop WDT
            WDT_A_CTL_HOLD;

    // Configure GPIO
    P1->DIR = ~(uint8_t) BIT1;              // P1.1 direction set input
    P1->OUT |= BIT0;                        // P1.0 output set to ON

    P1->SEL0 = 0;                           //port1 set to GPIO
    P1->SEL1 = 0;                           //port1 set to GPIO

    P1->OUT |= BIT1;                         // P1 output register set to 0x02
    P1->REN = BIT1;                         // Enable pull-up resistor (P1.1 output high)

    P1->IES = BIT1;                         // Button Interrupt on high-to-low transition
    P1->IFG = 0;                            // Clear all P1 interrupt flags
    P1->IE = BIT1;                          // Enable interrupt for P1.1 (Button)



    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;    // Enable sleep on exit from ISR

    // Terminate all remaining pins on the device
        //All Port Directions turned to outputs, all outputing logic 0s
        P2->DIR |= 0xFF; P2->OUT = 0;
        P3->DIR |= 0xFF; P3->OUT = 0;
        P4->DIR |= 0xFF; P4->OUT = 0;
        P5->DIR |= 0xFF; P5->OUT = 0;
        P6->DIR |= 0xFF; P6->OUT = 0;
        P7->DIR |= 0xFF; P7->OUT = 0;
        P8->DIR |= 0xFF; P8->OUT = 0;
        P9->DIR |= 0xFF; P9->OUT = 0;
        P10->DIR |= 0xFF; P10->OUT = 0;

        // Configure Port J
        //Direction of bit 0 - 3 set to output, all outputting logic 0
        PJ->DIR |= (BIT0| BIT1 | BIT2 | BIT3);
        PJ->OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);

    // Ensures SLEEPONEXIT takes effect immediately
    __DSB();

    // Enable global interrupt
    __enable_irq();

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);           // Enable TA0.0 Interrupt
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);           // Enable Port1 Interrupt

    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;          // Timer A CCIFG flag enable
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 18750;               // CCR0 set to 18750 for debounce

    __sleep();
    __no_operation();
}
// Timer A0_0 interrupt service routine

void TA0_0_IRQHandler(void) {
    // Clear the compare interrupt flag
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;      // Timer flag off
    P1->OUT ^= BIT0;                        // Toggle P1.0 LED
    TIMER_A0->CTL = TIMER_A_CTL_MC__STOP;   // Stops timer until next button press
    TIMER_A0->CTL = TIMER_A_CTL_CLR;        // Clears timer
}

/* Port1 ISR */
void PORT1_IRQHandler(void)
{
    // Toggling the output on the LED
        if(P1->IFG & BIT1)
        {
            TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, UP mode, 1/8
                        TIMER_A_CTL_MC__UP |
                        TIMER_A_CTL_ID_3;
        }

    P1->IFG &= ~BIT1;                       // Turn off P1 interrupt flag
}

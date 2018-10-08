# Read Me
## Hardware PWM
Hardware PWM is simply a way for a timer module to connect directly to a pin and output a PWM signal.  This is done by first setting up the timer in outmode 7 where two CCRs are used as values to turn on and turn off the signal.  The one CCR value is allows for duty cycle setting and changing at any desired frequency.

This timer is then connected directly to a pin.  In the case of this program, a 50% duty cycle at 1kHz is first set.  After every button press the duty cycle increases by 10% until 100%, then switching to a 0% duty cycle.  This is done by incrementing the CCR that determines the duty cycle by 10% every button press, and once it hits a certain value an if statment sets it back to 0, repeating the process.

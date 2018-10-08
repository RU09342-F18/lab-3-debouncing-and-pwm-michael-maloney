# Read Me
## Software Debouncing
This program is designed to fix a substantial problem found in most digital buttons.  Buttons are designed to only send a signal when pressed.  However, when unpressed the button might bounce and send multiple unwannted signals to the processor.

To combat this, a debouncing program was made.  This program is simple and implements two interrupts. The first interrupt is a button interrupt that toggles an LED.  This interrupt then turns on a timer interrupt that only interrupts after a select time.  This time was chosen as the optimal delay after a button press to ignore the bouncing signals while also allowing the user to press the button at a reasonable rate without missing a wanted signal.  Once this timer interrupts, the timer turns off, clears, and turns off the button interrupt flag, allowing the button to be pressed again to toggle the LED.

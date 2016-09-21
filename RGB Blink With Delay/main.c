/*
Author:         Yash Bansod
Date:           20th September 2016
Topic :         RBG LED blink with delay of 1s.
Filename:       Delay Rev 0.0

Author's Note:  Please add the preprocessor directories the project options.

                Ex:     C:\Program Files (x86)\IAR Systems\Embedded Workbench 7.5\arm\CMSIS\Include
                        C:\TI-CMSIS-Devices-8636\TI\LM4F\Include
                These are the locations where the CMSIS library files are available.
*/

#include <lm4f120h5qr.h>        // CMSIS library header for tm4c123gh6pm/lm4f120h5qr

#define SECOND 3200000          // Counted according to the machine cycles at 16MHz

void delay(int);

  // ** MAIN **
int main()
{
  // General-Purpose Input/ Output Run Mode Clock Gating Control (RCGCGPIO)
  SYSCTL->RCGCGPIO = 0x20;      // 0b100000 (enables PORTF only)
  
  // The RBG LEDs are connected to PF1, PF2, and PF3
  
  // GPIO Direction (GPIODIR)
  GPIOF->DIR = 0xE;             // 0b1110 output enable PF1, PF2, PF3
  
  // GPIO Digital Enable (GPIODEN)
  GPIOF->DEN = 0xE;             // 0b1110 digital enable PF1,PF2, PF3
  
  int Second = 1 * SECOND;
  
  while(1){
    // GPIO Data (GPIODATA)
    GPIOF-> DATA = 0x02;        // 0b0010 green off, blue off, red on
    delay(Second);                   // Delay of one second. 
    GPIOF-> DATA = 0x04;        // 0b0100 green off, blue on, red off
    delay(Second);                   // Delay of one second. 
    GPIOF-> DATA = 0x08;        // 0b1000 green on, blue off, red off
    delay(Second);                   // Delay of one second. 
  }
  return 0;
}

void delay(int Second) {
  int clockCounter = 0;
  while(clockCounter++ < Second);
}
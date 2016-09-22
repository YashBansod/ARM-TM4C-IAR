/*
Author:         Yash Bansod
Date:           21st September 2016
Topic :         RBG LED blink with button push.
Filename:       Button Rev 0.0

Author's Note:  Please note that C++ file. This was required to use the GPIOCR register in the TI CMSIS Library.
                SW1 == Blue LED ; SW2 == Greeen LED ; SW1 AND SW2== Red;
                Please add the preprocessor directories the project options.

                Ex:     C:\Program Files (x86)\IAR Systems\Embedded Workbench 7.5\arm\CMSIS\Include
                        C:\TI-CMSIS-Devices-8636\TI\LM4F\Include
                These are the locations where the CMSIS library files are available.
*/

#include <lm4f120h5qr.h>                                // CMSIS library header for tm4c123gh6pm/lm4f120h5qr

// ** MAIN **
int main()
{
  SYSCTL->RCGCGPIO = 0x20;                              // 0b100000 (enables PORTF only)
  GPIOF->LOCK = 0x4C4F434B;                             // unlock value defined in datasheet, any other value will lock it
  GPIOF->CR = 0xFF;                                     // 0b111111 enable ability to write to PUR for PF0
  GPIOF->DIR = 0x0E;                                    // 0b01110, make PF0 and PF4 inputs, make PF1, PF2, and PF3 outputs
  GPIOF->PUR = 0x11;                                    // 0b10001, enable pull up resistors on PF0 and PF4
  GPIOF->DEN = 0x1F;                                    // 0b00011111, enable PF0 to PF4
  
  while(1){
    
    switch(GPIOF->DATA & 0x11){                         // GPIOF->DATA = GPIOF->DATA & 0b00010001 because we only care about PF0 AND PF4
    case 0x00:                                          // both switches are pressed
      GPIOF->DATA =(1<<1);                              // 1<<1 == 0b10, turn on red led
      break;
    
    case 0x01:
      GPIOF->DATA =(1<<2);                              // 1<<2 == 0b100 turn on blue led
      break;
    
    case 0x10:
      GPIOF->DATA =(1<<3);                              // 1<<3 == 0b1000 turn on the green led
      break;
      
    default:
      GPIOF->DATA &= ~ ((1<<1)|(1<<2)|(1<<3));          //GPIOF->DATA = GPIOF->DATA & 0b11110001
      break;
    }
  }
  return 0;
}

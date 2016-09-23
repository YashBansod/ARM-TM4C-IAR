/*
Author:         Yash Bansod
Date:           22nd September 2016
Topic :         RBG LED blink with Timer Interrupt (using Timer0A)
Filename:       Timer Interrupt Rev 0.0

Author's Note:  Please add the cstartup_M.c in the project directory. Also add it in the IAR Project.
                Please add the preprocessor directories the project options.

                Ex:     C:\Program Files (x86)\IAR Systems\Embedded Workbench 7.5\arm\CMSIS\Include
                        C:\TI-CMSIS-Devices-8636\TI\LM4F\Include
                These are the locations where the CMSIS library files are available.
*/

#include <lm4f120h5qr.h>                        // CMSIS library header for tm4c123gh6pm/lm4f120h5qr
#define FLAG_NONE 0x00000000                    // The flag will be reset to this when Interrupt is detected in the Main.
#define FLAG_TOGGLE_LED 0x00000001              // The flag will be set to this when Interrupt is generated and Handler function executes.
#define SECONDS 16000000                        // Number of clock pulses per second.

volatile uint32_t flags = FLAG_NONE;            // A variable flags is declared which gets modified by interrupt(hence volatile)

void TIMER0A_Handler( void );                   // Prototype for the TIMER0A_Handler function.

// ** MAIN **
int main()
{
  int Seconds = SECONDS*1;                      // Timer is set for 1s.
  int Counter = 0;                              // Counter for changing the LEDs
  
// ** Configure PF1, PF2, PF3.
  SYSCTL->RCGCGPIO = (1<<5);                    // enable clock on PORTF`
  GPIOF->DIR = (1<<1)|(1<<2)|(1<<3);            // make PF1,PF2, PF3 output
  GPIOF->DEN = (1<<1)|(1<<2)|(1<<3);            // digital i/o enable PF1, PF2, PF3
  
/*11.4 Initialization and Configuration */
  SYSCTL->RCGCTIMER |= (1<<0);                  // To use a GPTM, the appropriate TIMERn bit must be set in the RCGCTIMER
  
/*11.4.1 One-shot/Periodic Timer Mode with Interrupt generation */
  TIMER0->CTL = ~(1<<0);                        // Timer0 is disabled (the TnEN bit in the GPTMCTL register is cleared)
  TIMER0->CFG = 0x00000000;                     // Write the GPTMCFG with a value of 0x0000.0000.
  TIMER0->TAMR |= (0x2<<0);                     // Periodic timer mode in the TnMR field in the GPTMTnMR.
  TIMER0->TAMR &= ~(1<<4);                      // TnCDIR bits in the GPTMTnMR register set for count down direction.
  TIMER0->TAILR = Seconds;                      // Load the start value into the GPTMTnILR.
  TIMER0->CTL = (1<<0);                         // Set the TnEN bit in the GPTMCTL register to enable the timer.
  TIMER0->IMR |= (1<<0);                        // Set the appropriate bits in the (GPTMIMR) to enable the interrupt mask
  NVIC_EnableIRQ(TIMER0A_IRQn);                 // Enable the TIMER0A interrupt request in NVIC.
  TIMER0->CTL = (1<<0);                         // Set the TnEN bit in the GPTMCTL register to enable the timer.
  
  while(1){
// Wait for the interrupt to be generated. The status flags are cleared by writing a 1 to the (GPTMICR)
    if (flags == FLAG_TOGGLE_LED){
      
      switch(Counter){
      case 0:                                   // Glow RED LED        
        TIMER0->ICR |= (1<<0);
        GPIOF->DATA =(1<<1);
        Counter ++;
        break;
        
      case 1:                                   // Glow BLUE LED   
        TIMER0->ICR |= (1<<0);
        GPIOF->DATA =(1<<2);
        Counter ++;
        break;
        
      case 2:                                   // Glow GREEN LED   
        TIMER0->ICR |= (1<<0);
        GPIOF->DATA =(1<<3);
        Counter = 0;
        break;
        
      default:                                  // Impossible condition so skip
        break;
      }
      flags = FLAG_NONE;
    }
  }
  return 0;
}

/* Handler Function for the TIMER0A interrupt. This executes when interrupt happens*/
void TIMER0A_Handler( void ){                   
  flags = FLAG_TOGGLE_LED;                      // The flag is modified to FLAG_TOGGLE_LED so it can be detected in Main.
  TIMER0->ICR |= (1<<0);                    
}
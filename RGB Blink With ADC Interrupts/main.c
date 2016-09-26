/*
Author:         Yash Bansod
Date:           25th September 2016
Topic :         RBG LED blink with ADC Interrupt (using ADC1SS3)
Filename:       ADC Interrupt Rev 0.0

Author's Note:  Please add the cstartup_M.c in the project directory. Also add it in the IAR Project.
Please add the preprocessor directories the project options.

Ex:     C:\Program Files (x86)\IAR Systems\Embedded Workbench 7.5\arm\CMSIS\Include
C:\TI-CMSIS-Devices-8636\TI\LM4F\Include
These are the locations where the CMSIS library files are available.
*/

#include <lm4f120h5qr.h>                                // CMSIS library header for tm4c123gh6pm/lm4f120h5qr

void ADC1SS3_Handler(void);                             // Prototype for the ADC1SS3_Handler
void RBG_Glow(void);                                    // Prototype for the RGB_Glow Funciton     

volatile static uint32_t adcResult = 0;                 // Initialize a volatile variable to store value from ADC     

/** MAIN **/
int main(){
  
  /** Enabling the ADC, GPIOE and GPIOF Modules **/
  
  SYSCTL->RCGCADC = (1<<1);                             // Enable the CLOCK to ADC1 using the RCGCADC register
  
  SYSCTL->RCGCGPIO |= (1<<4);                           // Enable clock to PORTE
  GPIOE->DIR &= ~(1<<1);                                // Set the PORT E1 as input
  GPIOE->AFSEL = (1<<1);                                // Enable alternate functionality of PE1. Note PE1 is AIN2, representing Analog Input 2
  GPIOE->DEN &= ~(1<<1);                                // Clear the digital enable of PE1
  GPIOE->AMSEL = (1<<1);                                // Disable the analog isolation on PE1
  
  SYSCTL->RCGCGPIO |= (1<<5);                           // Enable clock to PORTF
  GPIOF->DEN = 0xFF;                                    // Digital Enable PORTF
  GPIOF->AFSEL = 0x00;                                  // Disable alternate functions, we are using digital. Note that this can be skipped
  GPIOF->DIR = 0xFF;                                    // Set the PORT F as input
  
  /** Sample Sequencer Configuration 3 **/
  
  ADC1->ACTSS &= ~(1<<3);                               // Sample Sequencer 3 is disabled
  ADC1->EMUX = (0xF<<12);                               // Trigger event is set to continously sample mode
  ADC1->SSMUX3 = 2;                                     // Input the value 2 to the SSMUX3 to tell it to use AIN2 (PE1) as input
  ADC1->SSCTL3 = 0x6;                                   // Set the bit 1 to specify End of Sequence and bit 2 for Sample Interrupt Enable
  ADC1->IM = (1<<3) ;                                   // Interrupt MASK bit in the ADCIM register
  ADC1->ACTSS |= (1<<3);                                // Enable the Sample Sequencer 3
  ADC1->ISC = (1<<3);                                   // Clear the interrupt flags
  
  NVIC_EnableIRQ(ADC1SS3_IRQn);                         // Enable the ADC1SS3 interrupt request in NVIC
  
  while(1){
    // NOP
  }
  return 0;
}

/* Handler Function for the ADC1SS3 interrupt. This executes when interrupt happens*/
void ADC1SS3_Handler(void)
{
  adcResult = ADC1->SSFIFO3;                            // Update the value of adcResult from Sample Sequencer 3
  ADC1->ISC = (1<<3);                                   // Clear the Interrupt flag
  RBG_Glow();
}

/* A funtion that changes the LED state according to the updated value of adcResult */
void RBG_Glow(){
  if(adcResult < 1024){
    GPIOF->DATA = ~((1<<1)|(1<<2)|(1<<3));              // Shut of all LEDs
  } 
  else if(adcResult > 1024 && adcResult < 2048 ) {
    GPIOF->DATA =(1<<1);                                // Glow RED LED
  }
  else if(adcResult > 2048 && adcResult < 3072 ) {
    GPIOF->DATA =(1<<2);                                // Glow BLUE LED
  }
  else if(adcResult > 3072 && adcResult < 4096 ) {
    GPIOF->DATA =(1<<3);                                // Glow GREEN LED
  }
}
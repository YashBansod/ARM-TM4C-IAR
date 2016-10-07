/*
Author:         Yash Bansod
Date:           7th October 2016
Topic :         RBG LED blink with multiple UART inputs. (UART 0, 1)
Filename:       MULTI-UART Rev 0.0

Author's Note:  Please add the preprocessor directories the project options.

                Ex:     C:\Program Files (x86)\IAR Systems\Embedded Workbench 7.5\arm\CMSIS\Include
                        C:\TI-CMSIS-Devices-8636\TI\LM4F\Include
                These are the locations where the CMSIS library files are available.

                The baud rate divisor(BRD) = 16,000,000 / (16 * 9600) = 104.1666666 => 104
                UARTFBRD[DIVFRAC] = integer(0.166666 * 64 + 0.5) = 11.1666666 => 11


*/

#include <lm4f120h5qr.h>                                // CMSIS library header for tm4c123gh6pm/lm4f120h5qr

char readChar(void);                                    // Prototype for a function that reads the user's input
void printChar(char c);                                 // Prototype for a function that prints the user's input
void printString(char *  string);                       // Prototype for a function that prints the predefined string

/* MAIN */
int main()
{
  char c;                                               // Variable to store the user's input
  SYSCTL->RCGCUART |= (1<<0)|(1<<1);                    // Enable the UART0 and UART1 module using the RCGCUART register
  SYSCTL->RCGCGPIO |= (1<<0)|(1<<1);                    // Enables the Port A and Port B GPIO via the RCGCGPIO register
  
  GPIOA->AFSEL = (1<<1)|(1<<0);                         // enables the alternate function on pin PA0 & PA1
  GPIOB->AFSEL = (1<<1)|(1<<0);                         // enables the alternate function on pin PB0 & PB1
  

  GPIOA->PCTL = (1<<0)|(1<<4);                          // Configure the GPIOPCTL register to select UART0 in PA0 and PA1
  GPIOB->PCTL = (1<<0)|(1<<4);                          // Configure the GPIOPCTL register to select UART1 in PB0 and PB1
  
  GPIOA->DEN = (1<<0)|(1<<1);                           // Enable the digital functionality in PA0 and PA1   
  GPIOB->DEN = (1<<0)|(1<<1);                           // Enable the digital functionality in PB0 and PB1 
  
  UART0->CTL &= ~(1<<0);                                // Disable UART0 by clearing UARTEN bit in the UARTCTL register
  UART1->CTL &= ~(1<<0);                                // Disable UART1 by clearing UARTEN bit in the UARTCTL register
  
  
  UART0->IBRD = 104;                                    // Write the integer portion of the BRD to the UARTIRD register     
  UART1->IBRD = 104;                                    // Write the integer portion of the BRD to the UARTIRD register
  
  UART0->FBRD = 11;                                     // Write the fractional portion of the BRD to the UARTFBRD register
  UART1->FBRD = 11;                                     // Write the fractional portion of the BRD to the UARTFBRD register
  
  UART0->LCRH = (0x3<<5);                               // 8-bit, no parity, 1 stop bit
  UART1->LCRH = (0x3<<5);                               // 8-bit, no parity, 1 stop bit
  
  
  
  UART0->CC = 0x0;                                      // Configure the UART clock source as system clock
  UART0->CC = 0x0;
  
  UART0->CTL = (1<<0)|(1<<8)|(1<<9);                    // UART0 Enable, Transmit Enable, Recieve Enable
  UART1->CTL = (1<<0)|(1<<8)|(1<<9);                    // UART1 Enable, Transmit Enable, Recieve Enable
  
  // Configure LED pins 
  SYSCTL->RCGCGPIO |= (1<<5);                           // enable clock on PortF
  GPIOF->DIR = (1<<1)|(1<<2)|(1<<3);                    // make LED pins (PF1, PF2, AND PF3) outputs
  GPIOF->DEN = (1<<1)|(1<<2)|(1<<3);                    // enable digital funcition on LED pins
  GPIOF->DATA &= ~((1<<1)|(1<<2)|(1<<3));               // turn off leds
  
  while(1)
  {
    printString("Enter \"r\", \"g\", or \"b\":\n\r");   // \r is the carriage return character    

    c = readChar();                                     // reads the user inputted character
    printChar(c);                                       // prints the user inputted character so that the user knows what he typed
    printString("\n\r");                                // create a new line and a carraige return for the users character.
    
    switch(c)
    {
    case 'r':
      GPIOF->DATA = (1<<1);                             // red LED on
      break;
    case 'b':
      GPIOF->DATA = (1<<2);                             // blue LED on
      break;
    case 'g':
      GPIOF->DATA = (1<<3);                             // green LED on
      break;
    default:
      GPIOF->DATA &= ~((1<<1)|(1<<2)|(1<<3));           // all LEDs off
      break;
    }
  }
  return 0;
}

char readChar(void)                                                     // function to read user's input
{
  char c;
  while(((UART1->FR & (1<<4)) != 0)&&((UART0->FR & (1<<4)) != 0));      // waits the program till recieve FIFO is empty
  if (((UART0->FR & (1<<4)) == 0))                                      // if UART0 FIFO not empty then this
    c = UART0->DR;                                                      // value of UART0 DATA REGISTER is put into c
  else                                                                  // else we know UART1 FIFO not empty so this
  c = UART1->DR;                                                        // value of UART1 DATA REGISTER is put into c               
  return c;
}

void printChar(char c)                                                  // function to print user's input.
{
  while((UART0->FR &(1<<5)) != 0);                                      // waits the program till the previous transmission was completed
  UART0->DR =c;                                                         // value of UART0 DATA REGISTER is equal to c
  while((UART1->FR &(1<<5)) != 0);                                      // waits the program till the previous transmission was completed
  UART1->DR =c;                                                         // value of UART0 DATA REGISTER is equal to c
}

void printString(char * string)                                         // function to print a predefined string
{
  while(*string)
  {
    printChar(*(string++));                                             // printChar is called with every increment in string pointer
  }
}
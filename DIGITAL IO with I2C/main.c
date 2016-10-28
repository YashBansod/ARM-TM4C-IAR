/*
Author:         Yash Bansod
Date:           27th October 2016
Topic :         Digital IO with I2C (interfaced with IO expander PCF8574A with I2C1)
Filename:       I2C Master Write Rev 0.0

Author's Note: Please download the attached datasheet of the PCF8574A for pin layout and details.
Please add the preprocessor directories the project options.
Ex:     C:\Program Files (x86)\IAR Systems\Embedded Workbench 7.5\arm\CMSIS\Include
        C:\TI-CMSIS-Devices-8636\TI\LM4F\Include
These are the locations where the CMSIS library files are available.

Note: The the Read/ Write bit is not a part of the slave address. Thus slave adress is usually only 7bit long
      Please consider the start bit to be the Bit7 and write the appropriate HEX value in the SlaveAddress.
*/

#include <lm4f120h5qr.h>                                // CMSIS library header for tm4c123gh6pm/lm4f120h5qr

void setSlaveAddress(uint8_t);                          // Prototype for function for setting slave address
void setRW(uint8_t);                                    // Prototype for function to put communication to read(1)/write(0) mode
void writeByte(uint8_t,uint8_t);                        // Prototype for function to write the passed value to the bus

int main()
{
  SYSCTL->RCGCI2C = (1<<1);                             // Enable the provide a clock to I2C module 1
  SYSCTL->RCGCGPIO = (1<<0);                            // I2C Module 1 uses port A, enable the clock on Port A
  GPIOA->AFSEL = (1<<6)|(1<<7);                         // Enabling the alternate functions for PA6, PA7
  GPIOA->DEN = (1<<6)|(1<<7);                           // Enabling the Digital functionality
  GPIOA->ODR = (1<<7);                                  // Enable the open-drain configuration of PA7 that corresponds to SDA
  GPIOA->PCTL = (3<<28)|(3<<24);                        // Specify the alternate function 3 in PMC7 and PMC6 (pins PA7 and PA6 respectively)
  I2C1->MCR = (1<<4);                                   // Initialize I2C in Master mode
  I2C1->MTPR = (7<<0);                                  // Set the desired SCL clockspeed of 100 Kbps
  
  setSlaveAddress(0x38);                                // Bit7 - Bit1, can be found in the datasheet of the slave device
  setRW(0);                                             // Read(1)/Write(0) Bit (Bit0) is set
  writeByte(0x00, (1<<0)|(1<<1)|(1<<2));                // writeByte(DATA ,(START|RUN|STOP), initally 0 byte is sent to the slave
  
  while(1)
  {
    writeByte(0x10, (1<<0)|(1<<1)|(1<<2));              // writeByte(DATA ,(START|RUN|STOP), Write the DATA onto the bus
    for(int i = 0; i < 1000000; i++);                   // Arbitrarily a delay is given. Not need for this if timed delay desired
    
    writeByte(0x00, (1<<0)|(1<<1)|(1<<2));              // writeByte(DATA ,(START|RUN|STOP), Write the DATA onto the bus
    for(int i = 0; i < 1000000; i++);                   // Arbitrarily a delay is given. Not need for this if timed delay desired
  }
  return 0;
}

void setSlaveAddress(uint8_t slaveAddress)
{
  I2C1->MSA = (slaveAddress<<1);                        // Specify the slave address of the slave to be transmitted to
}

void setRW(uint8_t mode)
{
  if(mode == 0)                                         // If Bit0 is set as 
  {
    I2C1->MSA &= ~(1<<0);                               // Clear the bit. Transmitting data mode
  }
  else
  {
    I2C1->MSA |= (1<<0);                                // Set the bit. Receiving data mode
  }
}

void writeByte(uint8_t dataByte, uint8_t conditions)
{
  I2C1->MDR = dataByte;                                 // This register contains the data transferred during a transaction
  I2C1->MCS = conditions;                               // This register controls the START and STOP etc. of the communication
  
  while( (I2C1->MCS & (1<<0)) != 0);                    // While the I2C channel is busy, the program loops here
  
  if( (I2C1->MCS & (1<<1) ) != 0)                       // If an error is generated
  {
    if( (I2C1->MCS & (1<<4)) == 1){}                    // An Arbitration was lost
    else
    {
      I2C1->MCS = (1<<2);                               // Check the Acknowledgement bit
      while( (I2C1->MCS & (1<<0)) != 0);                // Wait till the transmitted data was acknowledged
    }
  }        
}
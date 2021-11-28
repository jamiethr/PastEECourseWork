// Jamie Thorup
// EE 474
// Lab 1
// Task 1 A

#include <stddef.h>
#include <stdint.h>
#include "task1A.h"

//int * ptr = NULL;

int main(void)
{
    volatile unsigned short delay = 0;
    RCGCGPIO |= 0x1120;   // Enable PortF, PortJ, PortN GPIO
    delay++;    // Delay 2 more cycles before access Timer registers
    delay++;    // Refer to Page. 756 of Datasheet for info
    
    GPIODEN_F = 0x11;   // Enables PF4's, PF0's digital port features
    GPIODEN_N = 0x3;    //Enables PN1, PN0 (D1, D2 LEDs) to digital port

    //    GPIODIR_F = 0x1;     // Set PF0 to output
    GPIODIR_F = 0x11;     // Set PF4, PF0 to output
    GPIODIR_N = 0x3;      //sets PN1, PN0 to output
    
    int j;
    while(1){
      GPIODATA_F = 0x1;   // Set PF0 to 1
      for (j = 0; j < 1000000; j++) {}
      GPIODATA_F = 0x10;  // set PF4 to 1, set PF0 to 0
      for (j = 0; j < 1000000; j++) {}
      GPIODATA_F = 0x0;   //set PF4 to 0
      GPIODATA_N = 0x1;   //turn PN0 to 1
      for (j = 0; j < 1000000; j++) {}
      GPIODATA_N = 0x2;   //set PN1 to 1, set PN0 to 0
      for (j = 0; j < 1000000; j++) {}
      GPIODATA_N = 0x0;   //set PN1, PN0 to 0
      for (j = 0; j < 1000000; j++) {}
    }       
    return 0;
}

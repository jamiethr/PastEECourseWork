// Jamie Thorup
// EE 474
// Lab 2 Task 1 A
// 10-23-20

#include <stddef.h>
#include <stdint.h>
#include "task1A.h"
#include "t1A_functions.c"

int main()
{   
    initLED();
    initTimer();
    
//    int j;
    while(1){
      
      GPIODATA_F = 0x1;   // Set PF0 to 1
//      for (j = 0; j < 1000000; j++) {}
      pollTimer0();
      
      GPIODATA_F = 0x10;  // set PF4 to 1, set PF0 to 0
//      for (j = 0; j < 1000000; j++) {}
      pollTimer0();
      
      GPIODATA_F = 0x0;   //set PF4 to 0
      GPIODATA_N = 0x1;   //turn PN0 to 1
//      for (j = 0; j < 1000000; j++) {}
      pollTimer0();
      
      GPIODATA_N = 0x2;   //set PN1 to 1, set PN0 to 0
//      for (j = 0; j < 1000000; j++) {}
      pollTimer0();
      
      GPIODATA_N = 0x0;   //set PN1, PN0 to 0
//      for (j = 0; j < 1000000; j++) {}
      pollTimer0();
    }
  
  return 0;
}

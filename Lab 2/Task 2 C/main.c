// Jamie Thorup
// EE 474
// Lab 2 Task 2 C
// 11-3-20

#include <stddef.h>
#include <stdint.h>
#include "task2C.h"
#include "t2C_functions.c"


int main()
{
  initPerifs(); // initialize the external LEDs, switches
  initTimer();  // initialize the timer
  s = goInit(); // initialize the traffic light to the its initial state
  
  while(1){
//    pollTimer1();      // wait 5 seconds
//    s = fsmTick(nextState);       // update states
  }
  return 0;
}

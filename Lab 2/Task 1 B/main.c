// Jamie Thorup
// EE 474
// Lab 2 Task 1 B
// 10-24-20

#include <stddef.h>
#include <stdint.h>
#include "task1B.h"
#include "t1B_functions.c"


int main()
{
  initPerifs(); // initialize the external LEDs, switches
  initTimer();  // initialize the timer
    
//  state s;
  state nextState;
//  s = init;
//  events x;
  goInit();
  
  while(1){
    pollTimer1();      // wait 5 seconds
//    x = getEvent();   // update events based on triggers
    s = fsmTick(nextState);       // update states
  }
  return 0;
}

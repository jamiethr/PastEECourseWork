// Jamie Thorup
// EE 474
// Lab 1
// Task 2

#include <stddef.h>
#include <stdint.h>
#include "task2.h"


// functions I guess
void LED_init(void){
    volatile unsigned short delay = 0;
//    RCGCGPIO |= 0x04;     // activate clock for Port C.
    RCGCGPIO |= 0x1120;   // Enable PortF, PortJ, PortN GPIO    
    delay++;
    delay++;
    
//    GPIOLOCK_C = 0x4C4F434B;    // unlock Port C
//    
//    GPIOCR_C |= 0x10;   // commits bits in PC4
//    
//    GPIOAMSEL_C &= ~0x10;   // disable analog function of PC4 (LEDs)
    GPIOAMSEL_L &= ~0x3;    // disable analog function of PL1,0 (Switches)
    
    GPIODEN_C   |= 0x10;    // enable digital output on PC4
    GPIODEN_L   |= 0x3;     // enable digital function on PL1,0 (SW's)
    
    GPIODEN_F = 0x11;       // Enables PF4's, PF0's digital port features
    GPIODEN_N = 0x3;        //Enables PN1, PN0 (D1, D2 LEDs) to digital port
    
    GPIODIR_C   |= 0x10;    // set PC4 to output
    GPIODIR_L   &= ~0x3;    // set PL1,0 (SW's) to input direction
    
    GPIODIR_F = 0x11;       // Set PF4, PF0 to output
    GPIODIR_N = 0x3;        // sets PN1, PN0 to output
    
    GPIOAFSEL_C &= ~0x10;   // set PC4 regular port function
    GPIOAFSEL_L &= ~0x3;    // set PL1,0 (SW's) to be controlled by peripheral
    
    
    delay++;
    delay++;
}

//turn on LED connected to PC4
void LED_on(void){
    int j;
    GPIODATA_C |= 0x10;
    GPIODATA_F |= 0x10;    //turn on stop light (PF4 or D3)
    for (j = 0; j < 1000000; j++) {}
    GPIODATA_N |= 0x1;    //turn on warn light (PN0 or D2)
    for (j = 0; j < 1000000; j++) {}
    GPIODATA_N |= 0x2;    //turn on go light (PN1 or leD1)
    for (j = 0; j < 1000000; j++) {}
}

//turn off LED connected to PC4
void LED_off(void){
    int j;
    GPIODATA_C &= ~0x10;
    GPIODATA_N &= ~0x2;   //turn off go light (PN1 or D1)
//    for (j = 0; j < 1000000; j++) {}
    GPIODATA_N &= ~0x1;   //turn off warn light (PN0 or D2)
//    for (j = 0; j < 1000000; j++) {}
    GPIODATA_F &= ~0x10;   //turn off stop light (PF4 or D3)
//    for (j = 0; j < 1000000; j++) {}
}


//define FSM states
typedef enum{init, go, warn, stop} state;

//define FSM events
typedef enum{start_stop, ped_button} events;

// checks buttons and returns proper event that is triggered from them.
events getEvent(void){  
  if( (GPIODATA_L & 0x2) == 0x2 ){      // if pedestrian button is pressed.
                                        //check to see if L1 is on 
      return ped_button;
  } else if ( (GPIODATA_L & 0x1) == 0x1 ){      // if stop button is pressed
                                                //check to see if L0 is on                      
      return start_stop;
  }
}

// when go state is triggered, returns go state and lights up proper LEDs
state goGo(void){
  GPIODATA_F &= ~0x10;   //turn off stop light (PF4 or D3) by ANDing with 0_1111
  GPIODATA_N |= 0x2;    //turn on go light (PN1 or leD1) by ORing it with 0010
  return go;
}

// when warn state is triggered, returns warn state and lights up warn light
state goWarn(void){
  GPIODATA_N &= ~0x2;   //turn off go light (PN1 or D1) by ANDing with 1101
  GPIODATA_N |= 0x1;    //turn on warn light (PN0 or D2) by ORing with 0001
  return warn;          //return warn state.
}

// when stop state is triggered, returns stop state and lights up stop light
state goStop(void){
  GPIODATA_N &= ~0x2;   //turn off go light (PN1 or D1) by ANDing with 1101
  GPIODATA_N &= ~0x1;   //turn off warn light (PN0 or D2) by ANDing with 1110
  GPIODATA_F |= 0x10;    //turn on stop light (PF4 or D3) by ORing with 1_0000
  return stop;
}

state goInit(void){
  GPIODATA_N &= ~0x2;   //turn off go light (PN1 or D1) by ANDing with 1101
  GPIODATA_N &= ~0x1;   //turn off warn light (PN0 or D2) by ANDing with 1110
  GPIODATA_F &= ~0x10;   //turn off stop light (PF4 or D3) by ANDing with 0_1111
  return init;
}



int main()
{
  int j;
  
  LED_init();
  
  while(1){
    LED_on();
//    for (j = 0; j < 1000000; j++) {}
//    LED_off();
  }

//  state s = start;
//  state nextState;
//  events x;
//  while(1){
//      x = getEvent() // this function checks which enumerated event it is rn?
//      if(s == init){
//        if(x == start_stop){
//          nextState = goGo();          //go to go state.
//        }
//      }
//      else if(s == go){
//        if(x == ped_button){
//          nextState = goWarn();       //go to warn state
//        }
//        else{
//          nextState = goStop();       //go to stop state
//        }
//      }
//      else if(s == warn){
//        nextState = goStop();         //go to stop state
//      }
//      else if(s == stop){
//        if(x == start_stop){
//          nextState = goInit();       //go to init state
//        }
//        else {
//          nextState = goGo();       //go to go state
//        }
//      }
//      
//      //maybe we put s = nextState; here???
//  }
  
  return 0;
}

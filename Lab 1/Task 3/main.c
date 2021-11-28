// Jamie Thorup
// EE 474
// Lab 1
// Task 2

#include <stddef.h>
#include <stdint.h>
#include "task2.h"

// let's abitrarily decide that LED's 1,2,3 is green, yellow, red

void initialize(void){
    volatile unsigned short delay = 0;
    RCGCGPIO |= 0x1520;   // Enable PortF, Port L, PortJ, PortN GPIO
    delay++;    // Delay 2 more cycles before access Timer registers
    delay++;    // Refer to Page. 756 of Datasheet for info
    
    // switch stuff & LEDs
    GPIOAMSEL_L &= ~0x1F;       // Disable PL4-0 analog function
    GPIOAFSEL_L &= ~0x1F;       // Select PL4-0 to run on peripheral
    GPIODIR_L   &= ~0x2;        // Set PL1, 0 to digital input (switches)
    GPIODIR_L   |=  0x1C;       // Set PL4-2 to digital output (LEDs)
    GPIODEN_L   |= 0x1F;        // Enable PL1, 0 digital function
    
    GPIODATA_L  |= 0x1C;       // turn em on real quick
    GPIODATA_L  &= ~0x1C;       // Turn off the LEDs (P4-2 or 11100)
    
    // built-in stuff
    GPIODEN_F = 0x11;   // Enables PF4's, PF0's digital port features
    GPIODEN_N = 0x3;    //Enables PN1, PN0 (D1, D2 LEDs) to digital port
    GPIODEN_J = 0x3;    //Enables PJ1, PJ0 (USR_SW2, USR_SW1) to digital port

    GPIODIR_F = 0x11;     // Set PF4, PF0 to output
    GPIODIR_N = 0x3;      //sets PN1, PN0 to output
//    GPIODIR_J =         //by default, PJ1, PJ0 are inputs (reg. value is zero)
    
    //to write to GPIOCR, we must set the status of GPIOLOCK reg. to unlocked.
    GPIOLOCK_J = 0x4C4F434B;      //must set it this special value to unlock it.
    //to enable pull-up resistors, we need to enable same bits in GPIOCR
    GPIOCR_J = 0x3;       //identical bit assignment as GPIOPUR as to allow
                          //  GPIOPUR to enable pull-up resistors for same bits
    GPIOPUR_J = 0x3;      //enables weak pull-up resistor for PJ1, PJ0;
}


typedef enum{init, go, warn, stop} state;       //define FSM states
typedef enum{start_stop, ped_button, noIn} events;    //define FSM events

events getEvent(void){
  
  // if this doesn't work, don't NOT gpiodata_L
  if((0x1 & GPIODATA_L) == 0x1){       // if start button (PL0) is pressed
      return start_stop;
  }
  
  if((0x2 & GPIODATA_L) == 0x2){       // if ped. button (PL1) is pressed
      return ped_button;
  }
  
  return noIn;  // output no input if no buttons are triggered
}

// the initial state of the FSM before it's turned on. All lights are off.
state goInit(void){
   // works with internal 
  GPIODATA_N &= ~0x2;   //turn off go light (PN1 or D1) by ANDing with 1101
  GPIODATA_N &= ~0x1;   //turn off warn light (PN0 or D2) by ANDing with 1110
  GPIODATA_F &= ~0x10;   //turn off stop light (PF4 or D3) by ANDing with 0_1111
  
  GPIODATA_L &= ~0x1C;       // Turn off the LEDs (P4-2 
  
  return init;
}

// when go state is triggered, returns go state and lights up proper LEDs
state goGo(void){
  int j;
  for (j = 0; j < 1000000; j++) {}      // pause for 0.3 seconds
  for (j = 0; j < 1000000; j++) {}      // pause for 0.3 seconds
  GPIODATA_F &= ~0x10;   //turn off stop light (PF4 or D3) by ANDing with 0_1111
  GPIODATA_N |= 0x2;    //turn on go light (PN1 or leD1) by ORing it with 0010
  
  GPIODATA_L &= ~0xC;   // turn off red and yellow lights (& w/ 0011)
  GPIODATA_L |=  0x10;  // turn on green light
  return go;
}

// when warn state is triggered, returns warn state and lights up warn light
state goWarn(void){
  int j;
  for (j = 0; j < 1000000; j++) {}      // pause for 0.3 seconds
  for (j = 0; j < 1000000; j++) {}      // pause for 0.3 seconds
  GPIODATA_N &= ~0x2;   //turn off go light (PN1 or D1) by ANDing with 1101
  GPIODATA_N |= 0x1;    //turn on warn light (PN0 or D2) by ORing with 0001
  
  GPIODATA_L &= ~0x18;   // turn off the green and red lights (& w/ 10111)
  GPIODATA_L |=  0x8;    // turn on yellow light
  return warn;          //return warn state.
}

// when stop state is triggered, returns stop state and lights up stop light
state goStop(void){
  int j;
  for (j = 0; j < 1000000; j++) {}      // pause for 0.3 seconds
  for (j = 0; j < 1000000; j++) {}      // pause for 0.3 seconds
  GPIODATA_N &= ~0x2;   //turn off go light (PN1 or D1) by ANDing with 1101
  GPIODATA_N &= ~0x1;   //turn off warn light (PN0 or D2) by ANDing with 1110
  GPIODATA_F |= 0x10;    //turn on stop light (PF4 or D3) by ORing with 1_0000
  
  GPIODATA_L &= ~0x18;   // turn off green and yellow lights (& w/ 11000)
  GPIODATA_L |=  0x4;    // turn on red light
  
  return stop;
}

int main(void)
{
    
    initialize();
    
    state s;
    state nextState;
    s = init;
    events x;
    goInit();
    
    while(1){
      x = getEvent();   // update to events based on triggers
      
      if(s == init){            // initial state
        if(x == start_stop){
           nextState = goGo();  // turn to go state and turn green
        }
      }
      
      else if(s == go){         // 'go' state
        if(x == start_stop){
          nextState = goInit();
        } else if(x == ped_button){    // if we request a stop
          nextState = goWarn();       // go to warn state and turn yellow
        } else {                // else, turn to stop state and turn red
          nextState = goStop();
        }
      }
      
      else if(s == warn){       // 'warn' state
        nextState = goStop();   // go to stop state and turn red
      }
      
      else if(s == stop){       // 'stop' state
        if(x == start_stop){    // if we hit the start/stop button
          nextState = goInit();         // turn the system off
        } else {                // else, return to 'go' state and turn green
          nextState = goGo();
        }
      }
      
      s = nextState;
      
    }       
    return 0;
}

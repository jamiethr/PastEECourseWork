// Jamie Thorup
// EE 474
// Lab 2 Task 2 C
// 11-3-20

// function definitions for task 2C

volatile state s = init;        // initialize state flag to initial state
volatile events x = noIn;       // init. event flag to its default: no input
volatile state nextState;       // init. next state flag

// handler for the timeout of Timer 1 (5 seconds); updates states
void Timer1A_Handler( void ){
  GPTMICR_1 |= 0x1;     // clears interrupt flag for Timer 1
  s = fsmTick();        // update states
}

// handler for external switch presses (port L); triggers Timer 0 (2 seconds)
void ExternSW_Handler( void ){
  
  GPIOICR_L   |= 0x3;     // clears interrupt bits for PL1, PL0
  GPTMCTL_0   |= 0x1;     // enable Timer A for Timer 0

}

// handler for the timerout of Timer 0; updates events
void Timer0A_Handler( void ){
    GPTMICR_0 |= 0x1;           // clear Timer 0 flag
    GPTMCTL_0 &= ~0x1;          // disable Timer A for Timer 0
    
    GPTMIMR_1     &= ~0x1;      // disable Timer 1 timeout interrupt
    x = getEvent();            // changes x from default state based on switches
    
    if((s == go) && (x == ped_button)){     // check if we need to go to warn
      
      GPTMCTL_1     &= ~0x81;         // disable Timer A, B for Timer 1
      GPTMTAILR_1   = 0x4C4B400;      // refresh Timer 1 by loading it again
      s = goWarn();                   // set up the lights for warn state
      GPTMCTL_1     |= 0x81;          // enable Timer A, B for Timer 1
    }
    
    GPTMIMR_1 |= 0x1;   // enable Timer 1 timeout interrupt
}
                
// initializes the external LEDs and switches.
void initPerifs(void){
  volatile unsigned short delay = 0;
  RCGCGPIO |= 0x1520;   // Enable PortF, Port L, PortJ, PortN GPIO
  delay++;              // Delay 2 more cycles before access Timer registers
  delay++;              // Refer to Page. 756 of Datasheet for info
  
  // switch stuff & LEDs
  GPIOAMSEL_L &= ~0x1F;       // Disable PL4-0 analog function
  GPIOAFSEL_L &= ~0x1F;       // Select PL4-0 to run on peripheral
  GPIODIR_L   &= ~0x2;        // Set PL1, 0 to digital input (switches)
  GPIODIR_L   |=  0x1C;       // Set PL4-2 to digital output (LEDs)
  GPIODEN_L   |= 0x1F;        // Enable PL1, 0 digital function
  
  // set up the interrupt for Port L
  GPIOIS_L    &= ~0x3;    // make PL1, PL0 edge-sensitive
  GPIOIBE_L   &= ~0x3;    // makes PL1, PL0 edges controlled by GPIOIEV register
  GPIOIEV_L   |= 0x3;     // make PL1, PL0 interrupt trigger from rising edge
  GPIOIM_L    |= 0x3;     // unmask interrupt for PL1, PL0
  GPIOICR_L   |= 0x3;     // clears interrupt bits for PL1, PL0
  EN1    |= 0x200000;  // enables port L interrupt (interrupt #53)
  PRI13 |= 0x200;     // set highest priority for swtich interrupt (#53)
  
  GPIODATA_L  |= 0x1C;        // turn LEDs on real quick
  GPIODATA_L  &= ~0x1C;       // Turn off the LEDs (P4-2 or 11100)
  
  // built-in stuff
  GPIODEN_F = 0x11;   // Enables PF4's, PF0's digital port features
  GPIODEN_N = 0x3;    // Enables PN1, PN0 (D1, D2 LEDs) to digital port
  GPIODEN_J = 0x3;    // Enables PJ1, PJ0 (USR_SW2, USR_SW1) to digital port

  GPIODIR_F = 0x11;     // Set PF4, PF0 to output
  GPIODIR_N = 0x3;      // sets PN1, PN0 to output
  
  //to write to GPIOCR, we must set the status of GPIOLOCK reg. to unlocked.
  GPIOLOCK_J = 0x4C4F434B;      //must set it this special value to unlock it.
  //to enable pull-up resistors, we need to enable same bits in GPIOCR
  GPIOCR_J = 0x3;       // identical bit assignment as GPIOPUR as to allow
                        // GPIOPUR to enable pull-up resistors for same bits
  GPIOPUR_J = 0x3;      // enables weak pull-up resistor for PJ1, PJ0;
}


// initializes Timers 0 and 1
void initTimer(void){
  RCGCTIMER     |= 0x1;     // enable Timer 0 (2 seconds)
  RCGCTIMER     |= 0x2;     // enable Timer 1 (5 seconds)
  
  GPTMCTL_0     &= ~0x81;   // disable Timer A, B for Timer 0
  GPTMCTL_1     &= ~0x81;   // disable Timer A, B for Timer 1
  
  GPTMCFG_0     &= 0x00000000;  // reset the GPTM config. register for Timer 0
  GPTMCFG_1     &= 0x00000000;  // reset the GPTM config. register for Timer 1
  
  GPTMCFG_0     |= 0x0;         // set the Timer 0 to 32-bit mode
  GPTMCFG_1     |= 0x0;         // set the Timer 1 to 32-bit mode
  
  GPTMTAMR_0    |= 0x2;         // set Timer 0 to periodic mode
  GPTMTAMR_1    |= 0x2;         // set Timer 1 to periodic mode
  
  GPTMTAMR_0    &= ~0x10;       // set Timer 0 to count down
  GPTMTAMR_1    &= ~0x10;       // set Timer 1 to count down
  
  GPTMTAILR_0   = 0x1E84800;    // load Timer 0 (32 million ==> 0.5 Hz; 2 sec.)  
  GPTMTAILR_1   = 0x4C4B400;    // load Timer 1 (80 million ==> 5 sec.)

  GPTMCTL_1     |= 0x1;        // enable Timer A for Timer 1
  
  GPTMIMR_0     |= 0x1;         // enable Timer 0 timeout interrupt
  GPTMIMR_1     |= 0x1;         // enable Timer 1 timeout interrupt
  EN0           |= 0x80000;     // enable interrupt #19 (Timer 0 interrupt)
  EN0           |= 0x200000;    // enable interrupt #21 (Timer 1 interrupt)
}


// uses inputs from external components to emulate FSM behavior
state fsmTick(){  
  
  if(s == init){            // initial state
    if(x == start_stop){
      nextState = goStop();     // turn on SM and turn red.
    }
  }
  
  else if(s == go){         // 'go' state
    if(x == start_stop){
      nextState = goInit();
    } else if(x == ped_button){   // if we request a stop
      nextState = goWarn();       // go to warn state and turn yellow
    } else {                      // else, turn to stop state and turn red
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
  
  x = noIn;     // once we've used our x input, set x back to its default state
  s = nextState;
  return s;
}

// uses external switches to determine if any valid event has occurred. 
events getEvent(void){
    
  if((0x1 & GPIODATA_L) == 0x1){      // if start button (PL0) is pressed
    return start_stop;
  }
  if((0x2 & GPIODATA_L) == 0x2){      // if ped. button (PL1) is pressed
    return ped_button;
  }
    
  return noIn;  // output no input if no buttons are triggered
}


// the initial state of the FSM before it's turned on. All lights are off.
state goInit(void){
   // modify built-in LEDs 
  GPIODATA_N &= ~0x2;   // turn off go light (PN1 or D1) by ANDing with 1101
  GPIODATA_N &= ~0x1;   // turn off warn light (PN0 or D2) by ANDing with 1110
  GPIODATA_F &= ~0x10;  // turn off stop light (PF4 or D3) by ANDing with 0_1111
  
  // modify external LEDs
  GPIODATA_L &= ~0x1C;       // Turn off the LEDs (P4-2)  
  return init;
}

// returns go state and configures LEDs for go state
state goGo(void){
  GPIODATA_F &= ~0x10;  // turn off stop light (PF4 or D3) by ANDing with 0_1111
  GPIODATA_N |= 0x2;    // turn on go light (PN1 or leD1) by ORing it with 0010
  
  GPIODATA_L &= ~0xC;   // turn off red and yellow lights (& w/ 0011)
  GPIODATA_L |=  0x10;  // turn on green light  
  return go;
}

// returns warn state and configures LEDs for warn state
state goWarn(void){
  GPIODATA_N &= ~0x2;   // turn off go light (PN1 or D1) by ANDing with 1101
  GPIODATA_N |= 0x1;    // turn on warn light (PN0 or D2) by ORing with 0001
  
  GPIODATA_L &= ~0x18;   // turn off the green and red lights (& w/ 10111)
  GPIODATA_L |=  0x8;    // turn on yellow light
  
  return warn;           // return warn state.
}

// when stop state is triggered, returns stop state and lights up stop light
state goStop(void){
  GPIODATA_N &= ~0x2;   // turn off go light (PN1 or D1) by ANDing with 1101
  GPIODATA_N &= ~0x1;   // turn off warn light (PN0 or D2) by ANDing with 1110
  GPIODATA_F |= 0x10;   // turn on stop light (PF4 or D3) by ORing with 1_0000
  
  GPIODATA_L &= ~0x18;   // turn off green and yellow lights (& w/ 11000)
  GPIODATA_L |=  0x4;    // turn on red light

  return stop;
}

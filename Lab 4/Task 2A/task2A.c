// Jamie Thorup
// EE 474
// Lab 4 Task 2 A
// 11-30-20

#include "task2A.h"
#include "tm4c1294ncpdt.h"
#include "SSD2119_Display.h"     // include display driver
#include "SSD2119_Touch.h"       // include touch driver
#include <stdint.h>
#include <stdio.h>

// variable definitions
volatile state s = init;        // initialize state flag to initial state
volatile events x = noIn;       // init. event flag to its default: no input
volatile state nextState;       // init. next state flag
static unsigned long x1;               // x coordinate read from touch screen
static unsigned long y;                // y coordinate read from touch screen
unsigned short pressed;         // used to determine if a virtual button is pressed
enum frequency freq;            // global variable to set frequency of PLL

const unsigned long LOAD0 = 0x7270E00;  // load for timer 0
const unsigned long LOAD1 = 0x11E1A300; // load for timer 1

// macro definitions

// definitions for interrupts
#define GPTMIMR_0       (*((volatile uint32_t *) 0x40030018 ))
#define GPTMIMR_1       (*((volatile uint32_t *) 0x40031018 ))

#define GPTMICR_0       (*((volatile uint32_t *) 0x40030024 ))
#define GPTMICR_1       (*((volatile uint32_t *) 0x40031024 ))

#define EN0             (*((volatile uint32_t *) 0xE000E100 ))
#define EN1             (*((volatile uint32_t *) 0xE000E104 ))

#define PRI13           (*((volatile uint32_t *) 0xE000E434 ))

#define GPIOIS_L        (*((volatile uint32_t *) 0x40062404 ))
#define GPIOIBE_L       (*((volatile uint32_t *) 0x40062408 ))
#define GPIOIEV_L       (*((volatile uint32_t *) 0x4006240C ))
#define GPIOIM_L        (*((volatile uint32_t *) 0x40062410 ))
#define GPIOICR_L       (*((volatile uint32_t *) 0x4006241C ))

#define ADCISC_1        (*((volatile uint32_t *) 0x4003900C ))

// definitions for timers
#define RCGCTIMER       (*((volatile uint32_t *) 0x400FE604 ))

#define GPTMCTL_0       (*((volatile uint32_t *) 0x4003000C ))
#define GPTMCTL_1       (*((volatile uint32_t *) 0x4003100C ))

#define GPTMCFG_0       (*((volatile uint32_t *) 0x40030000 ))
#define GPTMCFG_1       (*((volatile uint32_t *) 0x40031000 ))

#define GPTMTAMR_0      (*((volatile uint32_t *) 0x40030004 ))
#define GPTMTAMR_1      (*((volatile uint32_t *) 0x40031004 ))

#define GPTMTAILR_0     (*((volatile uint32_t *) 0x40030028 ))
#define GPTMTAILR_1     (*((volatile uint32_t *) 0x40031028 ))

#define GPTMRIS_0       (*((volatile uint32_t *) 0x4003001C ))
#define GPTMRIS_1       (*((volatile uint32_t *) 0x4003101C ))

// definitions for GPIO
#define RCGCGPIO        (*((volatile uint32_t *) 0x400FE608 ))

#define GPIOAMSEL_L     (*((volatile uint32_t *) 0x40062528 ))
#define GPIOAFSEL_L     (*((volatile uint32_t *) 0x40062420 ))

#define GPIODEN_F       (*((volatile uint32_t *) 0x4005D51C ))
#define GPIODEN_N       (*((volatile uint32_t *) 0x4006451C ))
#define GPIODEN_J       (*((volatile uint32_t *) 0x4006051C ))
#define GPIODEN_L       (*((volatile uint32_t *) 0x4006251C ))

#define GPIODIR_F       (*((volatile uint32_t *) 0x4005D400 ))
#define GPIODIR_N       (*((volatile uint32_t *) 0x40064400 ))
#define GPIODIR_J       (*((volatile uint32_t *) 0x40060400 ))
#define GPIODIR_L       (*((volatile uint32_t *) 0x40062400 ))

#define GPIOLOCK_J      (*((volatile uint32_t *) 0x40060520 ))
#define GPIOCR_J        (*((volatile uint32_t *) 0x40060524 ))
#define GPIOPUR_J       (*((volatile uint32_t *) 0x40060510 ))

#define GPIODATA_F      (*((volatile uint32_t *) 0x4005D3FC ))
#define GPIODATA_N      (*((volatile uint32_t *) 0x400643FC ))
#define GPIODATA_J      (*((volatile uint32_t *) 0x400603FC ))
#define GPIODATA_L      (*((volatile uint32_t *) 0x400623FC ))

// ******************** CLOCK CONTROL REGISTERS ********************** //
#define MOSCCTL                 (*(volatile uint32_t *) 0x400FE07C)
#define RIS                     (*(volatile uint32_t *) 0x400FE050)
#define RSCLKCFG                (*(volatile uint32_t *) 0x400FE0B0)
#define PLLFREQ0                (*(volatile uint32_t *) 0x400FE160)
#define PLLFREQ1                (*(volatile uint32_t *) 0x400FE164)
#define PLLSTAT                 (*(volatile uint32_t *) 0x400FE168)
#define MEMTIM0                 (*(volatile uint32_t *) 0x400FE0C0)

// function definitions for task 2

//////////////////////////////////////////////////////////////
//                INTERRUPT HANDLERS
//////////////////////////////////////////////////////////////

// handler for external switch presses (port L); triggers Timer 0 (2 seconds)
void ExternSW_Handler( void ){
  
  printf("extern switch handled!\n");

  GPIOICR_L   |= 0x3;     // clears interrupt bits for PL1, PL0
  GPTMCTL_0   |= 0x1;     // enable Timer A for Timer 0

}

// handles SSD's touch screen inputs
void ADC1SS3_Handler ( void ) {
  
  printf("ADC1 handled!\n");
  ADCISC_1 |= 0x8;    // clear the ADC interrupt flag
  
}

// handler for the timeout of Timer 1 (5 seconds); updates states
void Timer1A_Handler( void ){
  printf("timer1 handled!\n");
  GPTMICR_1 |= 0x1;     // clears interrupt flag for Timer 1
  s = fsmTick();        // update states
}

// handler for the timerout of Timer 0; updates events
void Timer0A_Handler( void ){
    GPTMICR_0 |= 0x1;           // clear Timer 0 flag
    GPTMCTL_0 &= ~0x1;          // disable Timer A for Timer 0
    
    GPTMIMR_1     &= ~0x1;      // disable Timer 1 timeout interrupt
    x = getEvent();            // changes x from default state based on switches
    
    if((s == init) && (x == start_stop)){          // check if we need to turn on
      GPTMCTL_1     &= ~0x81;         // disable Timer A, B for Timer 1
      GPTMTAILR_1   = LOAD1;          // refresh Timer 1 by loading it again
      s = goStop();                   // set up the lights for warn state
      GPTMCTL_1     |= 0x81;          // enable Timer A, B for Timer 1
    
    } else if((s == go) && (x == ped_button)){     // check if we need to go to warn
      
      GPTMCTL_1     &= ~0x81;         // disable Timer A, B for Timer 1
      GPTMTAILR_1   = LOAD1;          // refresh Timer 1 by loading it again
      s = goWarn();                   // set up the lights for warn state
      GPTMCTL_1     |= 0x81;          // enable Timer A, B for Timer 1
    
    } else if(((s == go) || (s == stop)) && (x == start_stop)){   // check if we need to turn off 
      GPTMCTL_1     &= ~0x81;         // disable Timer A, B for Timer 1
      GPTMTAILR_1   = LOAD1;          // refresh Timer 1 by loading it again
      s = goInit();                   // set up the lights for warn state
      GPTMCTL_1     |= 0x81;          // enable Timer A, B for Timer 1
    }
    
    printf("timer0 handled!\n");
    GPTMIMR_1 |= 0x1;   // enable Timer 1 timeout interrupt
}

//////////////////////////////////////////////////////////////
//                INITIALIZATION FUCNTIONS
//////////////////////////////////////////////////////////////
                
// PLL initialization function given in the lab files
int PLL_Init(enum frequency freq) {
    // Do NOT modify this function.
    MOSCCTL &= ~(0x4);                      // Power up MOSC
    MOSCCTL &= ~(0x8);                      // Enable MOSC
    while ((RIS & 0x100) == 0) {};          // Wait for MOSC to be ready
    RSCLKCFG |= (0x3 << 20);                // Select MOSC as system clock source
    RSCLKCFG |= (0x3 << 24);                // Select MOSC as PLL clock source

    PLLFREQ0 |= 0x60;                       // Set MINT field
    PLLFREQ1 |= 0x4;                        // Set N field

    MEMTIM0 &= ~((0xF << 22) | (0xF << 6));     // Reset FBCHT and EBCHT
    MEMTIM0 &= ~((0xF << 16) | (0xF << 0));     // Reset EWS and FWS
    MEMTIM0 &= ~((0x1 << 21) | (0x1 << 5));     // Reset FBCE and EBCE

    RSCLKCFG &= ~(0x1 << 28);                   // Temporarilly bypass PLL

    switch (freq) {
        case 120:
            MEMTIM0 |= (0x6 << 22) | (0x6 << 6);    // Set FBCHT and EBCHT
            MEMTIM0 |= (0x5 << 16) | (0x5 << 0);    // Set EWS and FWS
            RSCLKCFG |= 0x3;                        // Set PSYSDIV to use 120 MHZ clock
            RSCLKCFG &= ~0x3FC;                     // Update PSYSDIV field
            break;
        case 60:
            MEMTIM0 |= (0x3 << 22) | (0x3 << 6);    // Set FBCHT and EBCHT
            MEMTIM0 |= (0x2 << 16) | (0x2 << 0);    // Set EWS and FWS
            RSCLKCFG |= 0x7;                        // Set PSYSDIV to use 60 MHZ clock
            RSCLKCFG &= ~0x3F8;                     // Update PSYSDIV field
            break;
        case 12:
            MEMTIM0 |= (0x1 << 21) | (0x1 << 5);    // Set FBCE and EBCE
            RSCLKCFG |= 0x27;                       // Set PSYSDIV to use 12 MHZ clock
            RSCLKCFG &= ~0x3D8;                     // Update PSYSDIV field
            break;
        default:
            return -1;
    }

    RSCLKCFG |= (0x1 << 30);                // Enable new PLL settings
    PLLFREQ0 |= (0x1 << 23);                // Power up PLL
    while ((PLLSTAT & 0x1) == 0) {};        // Wait for PLL to lock and stabilize

    RSCLKCFG |= (0x1u << 31) | (0x1 << 28);  // Use PLL and update Memory Timing Register
    return 1;
}

// initializes the external LEDs and switches.
void initPerifs(void){
  
  // create an interrupt for ADC1 AKA SSD's touch screen
//  ADC1_ACTSS_R &= ~0x000F;      // Disable all sequencers for configuration 
//  
//  ADCISC_1 |= 0x8;    // clear the ADC interrupt flag
//  
//  // enable interrupt for ADC1 (touch screen capabilities)
//  ADC1_IM_R |= 0x8;       // SS3 raw interrupt signal is sent to ADCRIS, INR3 bits
//  EN1 |= 0x20000; // enable ADC1_SS3 interrupt (interrupt #49)
//
//  // Re-enable sample sequencer 3
//  ADC1_ACTSS_R |= 0x0008;
  
//############################### old --> might delete later ########################################
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
  
  GPTMTAILR_0   = LOAD0;        // load Timer 0 (120 million ==> 0.5 Hz; 2 sec.)  
  GPTMTAILR_1   = LOAD1;        // load Timer 1 (300 million ==> 5 sec.)

  GPTMCTL_1     |= 0x1;         // enable Timer A for Timer 1
  
  GPTMIMR_0     |= 0x1;         // enable Timer 0 timeout interrupt
  GPTMIMR_1     |= 0x1;         // enable Timer 1 timeout interrupt
  EN0           |= 0x80000;     // enable interrupt #19 (Timer 0 interrupt)
  EN0           |= 0x200000;    // enable interrupt #21 (Timer 1 interrupt)
}

//////////////////////////////////////////////////////////////
//                      'GET' FUNCTIONS
//////////////////////////////////////////////////////////////

// uses external switches to determine if any valid event has occurred. 
events getEvent(void){
  
  // update X, Y input coordinates from the touch screen
  x1 = Touch_ReadX();
  y = Touch_ReadY();
  
  if((x1 > 500) && (x1 < 1400) && (y > 500) && (y < 1200)) {     // if "on/off" button was pressed
      return start_stop;  
           
  } else if((x1 > 1400) && (x1 < 2100) && (y > 700) && (y < 1400)) {    // if "request yellow" was pressed
      return ped_button;
  }  
  return noIn;  // output no input if no buttons are triggered
}


// polls SSD's touch screen to check if one of the on-screen buttons have been pressed
void getTouch(void) {
  // update X, Y input coordinates from the touch screen
    x1 = Touch_ReadX();
    y = Touch_ReadY();
    
    // respond to virtual button presses
    if((x1 > 5) && (y > 5)){     // account for noise at low coordinates
      
      // check for "on/off" on-screen button press
      if((x1 > 500) && (x1 < 1400) &&
         (y > 500) && (y < 1200) && (pressed == 0x0)){
             pressed = 0x1F4;   // set to 500             
             GPTMCTL_0   |= 0x1;     // enable Timer A for Timer 0 (2 seconds)
      
             // check for "request yellow" on-screen button
      } else if((x1 > 1400) && (x1 < 2100) &&
                (y > 700) && (y < 1400) && (pressed == 0x0)) {
             
             pressed = 0x1F4;   // set to 500
             GPTMCTL_0   |= 0x1;     // enable Timer A for Timer 0 (2 seconds)
      } 
    }
    if(pressed > 0x0){  // if we're just receiving low-level noise, start resetting button input
      pressed--;         
    }
}

//////////////////////////////////////////////////////////////
//                      FSM FUNCTIONS
//////////////////////////////////////////////////////////////

// uses inputs from external components to emulate FSM behavior
state fsmTick(){  
  
  if(s == go){         // 'go' state
    if(x == start_stop){          // if we hit the start/stop button
      nextState = goInit();       // turn off the system
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

// the initial state of the FSM before it's turned on. All lights are off.
state goInit(void){
   // modify built-in LEDs 
  GPIODATA_N &= ~0x2;   // turn off go light (PN1 or D1) by ANDing with 1101
  GPIODATA_N &= ~0x1;   // turn off warn light (PN0 or D2) by ANDing with 1110
  GPIODATA_F &= ~0x10;  // turn off stop light (PF4 or D3) by ANDing with 0_1111
  
  // modify external LEDs
  GPIODATA_L &= ~0x1C;       // Turn off the LEDs (P4-2)  
  
  // use SSD to draw the traffic system
  LCD_DrawFilledCircle(55, 50, 30, Color4[2]);          // draw dimmed green light
  LCD_DrawFilledCircle(155, 50, 30, Color4[6]);         // draw dimmed yellow light
  LCD_DrawFilledCircle(255, 50, 30, Color4[4]);         // draw dimmed red light
  
  LCD_DrawFilledCircle(75, 175, 30, Color4[12]);        // draw "on/off" button
  LCD_SetCursor(50, 125);                            
  LCD_PrintString( "On / off" );
  
  LCD_DrawFilledCircle(225, 175, 30, Color4[12]);       // draw the "request yellow" button  
  LCD_SetCursor(175, 125);                              
  LCD_PrintString( "Request yellow light" );

  
  return init;
}

// returns go state and configures LEDs for go state
state goGo(void){
  
  LCD_DrawFilledCircle(255, 50, 30, Color4[4]);         // draw dimmed red light
  LCD_DrawFilledCircle(55, 50, 30, Color4[10]);         // draw lit green light
  LCD_DrawFilledCircle(155, 50, 30, Color4[6]);         // draw dimmed yellow light
  
  GPIODATA_F &= ~0x10;  // turn off stop light (PF4 or D3) by ANDing with 0_1111
  GPIODATA_N |= 0x2;    // turn on go light (PN1 or leD1) by ORing it with 0010
  
  GPIODATA_L &= ~0xC;   // turn off red and yellow lights (& w/ 0011)
  GPIODATA_L |=  0x10;  // turn on green light  
  return go;
}

// returns warn state and configures LEDs for warn state
state goWarn(void){
  
  LCD_DrawFilledCircle(55, 50, 30, Color4[2]);          // draw dimmed green light
  LCD_DrawFilledCircle(155, 50, 30, Color4[14]);        // draw lit yellow light
  
  GPIODATA_N &= ~0x2;   // turn off go light (PN1 or D1) by ANDing with 1101
  GPIODATA_N |= 0x1;    // turn on warn light (PN0 or D2) by ORing with 0001
  
  GPIODATA_L &= ~0x18;   // turn off the green and red lights (& w/ 10111)
  GPIODATA_L |=  0x8;    // turn on yellow light
  
  return warn;           // return warn state.
}

// when stop state is triggered, returns stop state and lights up stop light
state goStop(void){
  
  LCD_DrawFilledCircle(55, 50, 30, Color4[2]);          // draw dimmed green light
  LCD_DrawFilledCircle(155, 50, 30, Color4[6]);         // draw dimmed yellow light
  LCD_DrawFilledCircle(255, 50, 30, Color4[12]);        // draw lit red light
  
  GPIODATA_N &= ~0x2;   // turn off go light (PN1 or D1) by ANDing with 1101
  GPIODATA_N &= ~0x1;   // turn off warn light (PN0 or D2) by ANDing with 1110
  GPIODATA_F |= 0x10;   // turn on stop light (PF4 or D3) by ORing with 1_0000
  
  GPIODATA_L &= ~0x18;   // turn off green and yellow lights (& w/ 11000)
  GPIODATA_L |=  0x4;    // turn on red light

  return stop;
}

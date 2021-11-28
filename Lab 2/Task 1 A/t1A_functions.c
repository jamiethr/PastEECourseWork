// Jamie Thorup
// EE 474
// Lab 2 Task 1 A
// 10-23-20

// function definitions for task 1A

// initializes on-board LEDs 
void initLED(void){
  volatile unsigned short delay = 0;
  RCGCGPIO |= 0x1120;   // Enable PortF, PortJ, PortN GPIO
  delay++;    // Delay 2 more cycles before access Timer registers
  delay++;    // Refer to Page. 756 of Datasheet for info
  
  GPIODEN_F = 0x11;   // Enables PF4's, PF0's digital port features
  GPIODEN_N = 0x3;    //Enables PN1, PN0 (D1, D2 LEDs) to digital port

  //    GPIODIR_F = 0x1;     // Set PF0 to output
  GPIODIR_F = 0x11;     // Set PF4, PF0 to output
  GPIODIR_N = 0x3;      //sets PN1, PN0 to output
}


// initializes timer 0
void initTimer(void){
  RCGCTIMER     |= 0x1;     // enable Timer 0
  GPTMCTL_0     &= ~0x81;   // disable Timer A, B for Timer 0
              //~(1000_0001)
                            // note to self: maybe we only need to control A,
                            //          since B is tied to A in 32-bit mode.
  
  GPTMCFG_0     &= 0x00000000;  // reset the GPTM config. register (?) 
  GPTMCFG_0     |= 0x0;         // set the timer to 32-bit mode
  GPTMTAMR_0    |= 0x2;         // set Timer A (& therefore B) to periodic mode
  GPTMTAMR_0    &= ~0x10;       // set timer to count down
  GPTMTAILR_0   = 0xF42400;    // load timer (16,000,0000 ==> 1 Hz)  

//  -------->   CONFIGURE GPTMIMR FOR INTERRUPTS AT THIS STEP      <--------
  GPTMCTL_0     |= 0x81;   // enable Timer A, B for Timer 0
}


// polls timer 0, which is set to 1 Hz
void pollTimer0(void){
  // poll the tatoris GPTMRIS_0 to see when it's counted down to zero.
  // then, clear the bit by writing to GPTMICR
  // if GPTMRIS_0 has first bit == 1, it's done counting
  
  while((GPTMRIS_0 & 0x1) != 0x1){}  // while the timer hasn't timed out
  GPTMICR_0 |= 0x1;   // sets the first bit to one --> clears GPTMRIS
}
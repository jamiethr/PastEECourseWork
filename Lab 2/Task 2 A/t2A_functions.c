// Jamie Thorup
// EE 474
// Lab 2 Task 2 A
// 10-23-20

// function definitions for task 1A

void Timer0A_Handler( void ){
  GPTMICR_0 |= 0x1;   // sets the first bit to one --> clears GPTMRIS (flag)

  if((GPIODATA_F & 0x1) == 0x1){        // if the first light is on
    GPIODATA_F &= ~0x1;         // turn the first light off
    GPIODATA_F |= 0x10;         // turn on the second light
  } else if ((GPIODATA_F & 0x10) == 0x10){      // if the second light is on
    GPIODATA_F &= ~0x10;        // turn off the second light
    GPIODATA_N |= 0x1;          // turn on third light
  } else if((GPIODATA_N & 0x1) == 0x1) {        // if the third light is on
    GPIODATA_N &= ~0x1;         // turn off third light
    GPIODATA_N |= 0x2;         // turn on fourth light
  } else if((GPIODATA_N & 0x2) == 0x2) {      // if the fourth light is on
    GPIODATA_N &= ~0x2;        // turn off the fourth light
    GPIODATA_F |= 0x1;          // turn on the first light
  } else {
    GPIODATA_F |= 0x1;          // turn on the first light
  }
}

// initializes on-board LEDs 
void initLED(void){
  volatile unsigned short delay = 0;
  RCGCGPIO |= 0x1120;   // Enable PortF, PortJ, PortN GPIO
  delay++;              // Delay 2 more cycles before access Timer registers
  delay++;              // Refer to Page. 756 of Datasheet for info
  
  GPIODEN_F = 0x11;     // Enables PF4's, PF0's digital port features
  GPIODEN_N = 0x3;      // Enables PN1, PN0 (D1, D2 LEDs) to digital port

  GPIODIR_F = 0x11;     // Set PF4, PF0 to output
  GPIODIR_N = 0x3;      // sets PN1, PN0 to output
}


// initializes timer 0
void initTimer(void){
  RCGCTIMER     |= 0x1;         // enable Timer 0
  GPTMCTL_0     &= ~0x1;        // disable Timer A for Timer 0
  GPTMCFG_0     &= 0x00000000;  // reset the GPTM config. register 
  GPTMCFG_0     |= 0x0;         // set the timer to 32-bit mode
  GPTMTAMR_0    |= 0x2;         // set Timer A (& therefore B) to periodic mode
  GPTMTAMR_0    &= ~0x10;       // set timer to count down
  GPTMTAILR_0   = 0xF42400;     // load timer (16,000,0000 ==> 1 Hz)  

//  -------->   CONFIGURE GPTMIMR FOR INTERRUPTS AT THIS STEP      <--------
  GPTMCTL_0     |= 0x1;   // enable Timer A for Timer 0
  GPTMIMR_0     |= 0x1;         // enable Timer 0 timout interrupt
  EN0           |= 0x80000;     // enable interrupt #19 (Timer 0 interrupt)
}


// polls timer 0, which is set to 1 Hz
void pollTimer0(void){
  // poll the tatoris GPTMRIS_0 to see when it's counted down to zero.
  // then, clear the bit by writing to GPTMICR
  // if GPTMRIS_0 has first bit == 1, it's done counting
  
  while((GPTMRIS_0 & 0x1) != 0x1){}  // while the timer hasn't timed out
  GPTMICR_0 |= 0x1;   // sets the first bit to one --> clears GPTMRIS
}
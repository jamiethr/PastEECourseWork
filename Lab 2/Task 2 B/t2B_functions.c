// Jamie Thorup
// EE 474
// Lab 2 Task 2 B
// 10-31-20

// function definitions for task 1A

// handles the Timer 0 once it is timed out; changes LEDs that are lit
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

// handles user switch interrupt when triggered; enable/disables Timer 0
void UserSW_Handler( void ){
  
    if((GPIORIS_J & 0x1) == 0x1){       // if PJ0 (USR_SW1)caused the interrupt
      GPTMCTL_0     |= 0x1;   // enable Timer A for Timer 0
    }
    else if ((GPIORIS_J & 0x2)>>1 == 0x1) {     // if PJ1 (USR_SW2) caused the interrupt
      GPTMCTL_0     &= ~0x1;  // disable Timer A for Timer 0
    }
    
    GPIOICR_J |= 0x3;     // clears interrupt bits for PJ1, PJ0
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
    
    GPIODATA_F &= ~0x11;  // turn off PF4, PF0
    GPIODATA_N &= ~0x3;   // turn off PN1, PN0  
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
    GPTMIMR_0     |= 0x1;         // enable Timer 0 timeout interrupt
    EN0           |= 0x80000;     // enable interrupt #19 (Timer 0 interrupt)
}

// initializes the user switches and interrupts
void initSwitch(void){
    GPIODEN_J = 0x3;    // Enables PJ1, PJ0 (USR_SW2, USR_SW1) to digital port
    
    //to write to GPIOCR, we must set the status of GPIOLOCK reg. to unlocked.
    GPIOLOCK_J = 0x4C4F434B;      // must set it this special value to unlock it.
    //to enable pull-up resistors, we need to enable same bits in GPIOCR
    GPIOCR_J  = 0x3;      // identical bit assignment as GPIOPUR as to allow
                          //  GPIOPUR to enable pull-up resistors for same bits
    GPIOPUR_J = 0x3;      // enables weak pull-up resistor for PJ1, PJ0;
    
    GPIOIS_J  &= ~0x3;    // make PJ1, PJ0 edge-sensitive
    GPIOIBE_J &= ~0x3;    // makes PJ1, PJ0 edges controlled by GPIOIEV register
    GPIOIEV_J |= 0x3;     // make PJ1, PJ0 interrupt trigger from rising edge
    GPIOIM_J  |= 0x3;     // unmask interrupt for PJ1, PJ0
    GPIOICR_J |= 0x3;     // clears interrupt bits for PJ1, PJ0
    
    EN1 |= 0x80000;       // enables port J interrupt (interrupt #51)
    PRI12 |= 0x20000000;  // set highest priority for switch interrupt (#51)

}

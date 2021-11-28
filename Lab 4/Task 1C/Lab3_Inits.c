// Jamie Thorup
// EE 474
// Lab 4
// All code below (with the exception of the PLL_Init() function, is my own

#include "PLL_Header.h"
#include "Lab3_Inits.h"
#include <stdio.h>
#include <string.h>


#include "Thorup_HeaderL4T1B.h"

#include "C:\Users\Jamie!\Documents\UW\EE 474\Labs\Lab 4\SSD2119\SSD2119_Display.h"     // include display driver
#include "C:\Users\Jamie!\Documents\UW\EE 474\Labs\Lab 4\SSD2119\SSD2119_Touch.h"       // include touch driver
   
volatile unsigned short delay = 0;      // delay used throughout

// character arrays used to store displayed text
char textOne[27] = "The current temperature is ";
char textTwo[4] = " C, ";
char textThree[3] = " F.";

char textFour[31] = "The current clock frequency is ";
char textFive[5] = " MHz.";

//////////////////////////////////////////////////////////////
//              INITIALIZATION FUNCTIONS
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

// initializes the ADC to read from the temperature sensor on the timeout of the clock
void ADCRead_Init(void) {
  RCGCADC |= 0x1;       // Enable the ADC0 clock
  
  // Delay for RCGCADC (Refer to page 1073)
  delay++;      // delay three clock cycles
  delay++;
  delay++;
  delay++;      // one extra delay just in case

  // Power up the PLL (if not already)
  PLLFREQ0 |= 0x00800000; 
  
  // Wait for the PLL to lock
  while (PLLSTAT != 0x1);
  
  // Configure ADCCC to use the clock source defined by ALTCLKCFG
//  ALTCLKCFG &= 0x0;
  delay++;
  delay++;
  ADCCC_0 |= 0x1;
  
  // Enable clock to the appropriate GPIO Modules (Hint: Table 15-1)
  RCGCGPIO |= 0x10;     // enable port E
  
  // Delay for RCGCGPIO
  delay++;
  delay++;
  
  GPIOAFSEL_E   |= 0xF;         // set PE3-0 (to 1) --> controlled by peripheral (ADC)
  GPIODEN_E     &= ~0xF;        // clear PE3-0 --> disable digital functions
  GPIOAMSEL_E   |= 0xF;         // disables analog isolation for PE3-0
  ADCACTSS_0    &= ~0x8;        // disable SS3 for ADC0
  ADCEMUX_0     |= 0x5000;      // select timer trigger for SS3
  ADCSSMUX3_0   |= 0xF;         // choose all pins (PE3-PE0) to act as analog inputs for SS3     
  ADCSSCTL3_0   |= 0xE;         // reads from temperature sensor
                                // also, raw interrupt is asserted after conversion of sample & set this sample to end of sequence
  
  ADCIM_0|= 0x8;        // set interrupt mask --> raw interrupt signal is sent to ADCRIS, INR3 bits
  EN0 |= 0x20000;       // enable ADC0_SS3 interrupt (interrupt #17) in NVIC
  
  ADCACTSS_0 |= 0x8;    // enable SS3 in ADC0
}

// initializes the timer and configures its timeout to sample from ADC
void TimerADCTrigger_Init(enum frequency freq) {
  RCGCTIMER     |= 0x1;    // enable clock for Timer 0
  GPTMCTL_0     &= ~0x1;   // disable Timer A for Timer 0
  
  GPTMCFG_0     &= 0x00000000;  // reset the GPTM config. register (?) 
  GPTMCFG_0     &= 0x0;         // set the timer to 32-bit mode
  GPTMTAMR_0    |= 0x2;         // set Timer A (& therefore B) to periodic mode
  GPTMTAMR_0    &= ~0x10;       // set timer to count down
  
  // load clock with different amount based on PLL frequency
  if(freq == 120){
    printf("Freq: 120\n");     // print the frequency
    GPTMTAILR_0   = 0x7270E00;     // load timer (120,000,0000 ==> 1 Hz)
  }
  else if(freq == 60){
    printf("Freq: 60\n");     // print the frequency
    GPTMTAILR_0   = 0x3938700;     // load timer (60,000,0000 ==> 1 Hz)
  }
  else if(freq == 12){
    printf("Freq: 12\n");     // print the frequency
    GPTMTAILR_0   = 0xB71B00;     // load timer (12,000,0000 ==> 1 Hz)
  }

  GPTMADCEV_0   |= 0x1;         // sends trigger signal to ADC on timeout
  GPTMCTL_0     |= 0x20;        // set TAOTE, enables timer A trigger for ADC
  
  GPTMCTL_0     |= 0x1;   // enable Timer A for Timer 0
}


// initializes the user switches and interrupts
void initSwitch(void){
//    RCGCGPIO |= (0x1 << 8);
    RCGCGPIO |= 0x1120;   // Enable PortF, PortJ, PortN GPIO
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

// initializes UART0 for serial data transfer
void UART0_Init(void){
  RCGCUART      |= 0x4;         // provide a clock for UART2
  RCGCGPIO      |= 0x1;         // enable Port A in GPIO
  GPIOAFSEL_A   |= 0xC0;        // set PA7, 6 (U0Tx, U0Rx) to be controled by UART2
  GPIODEN_A     |= 0xC0;        // enable digital functions for PA7, 6
  GPIOPCTL_A    |= 0x11000000;  // encode PA7, 6 as U0Tx, U0Rx
  
  UARTCTL_2     &= ~(0x1);      // disable UART2
  delay++;                      // delay to allow UART to finish trans/recieving
  
  // baud rate here is 9600. int/ fraction values calculated with SysClk = 60 MHz
  UARTIBRD_2    = 0x186;        // load 390 into integer part of the Baud Rate Divisor
  UARTFBRD_2    = 0x28;         // load 40 into fraction part of the Baud Rate Divisor
  UARTLCRH_2    &= ~(0x10);     // flush transmit FIFO, but also update Baud Rate Divisor 
  UARTLCRH_2    |= 0x60;        // set UART word length to 8 bits
           
  UARTCTL_2     |= 0x1;      // enable UART2
}

// initializes (draws) the on-screen text using the SSD2119 LCD
void text_Init(void){
  // draw first sentence
  LCD_SetTextColor(255, 255, 255);      // set text color to white 
  LCD_SetCursor(0, 0);                  // have cursor draw at the top left of the screen
  LCD_PrintString( textOne );           // print "the curr. temp. is..."
  LCD_Goto( strlen(textOne) + 9, 0);    // go to end of temperature in celcius
  LCD_PrintString( textTwo );           // print " C, "
  LCD_Goto( strlen(textOne) + 9 + strlen(textTwo) + 7, 0);    // go the end of temperature in fahrenheit
  LCD_PrintString( textThree );         // print " F."
  
  // draw second sentence
  LCD_Goto(0,1);                // go to new line
  LCD_PrintString( textFour );  // print "the current clock frequency..."
  LCD_Goto( strlen(textFour) + 2, 1);    // go to end of clock frequency
  LCD_PrintString( textFive );  // print " MHz."
}

// initializes (draws) the on-screen buttons using the SSD2119 LCD
void button_Init(void){
  LCD_DrawFilledCircle(75, 175, 30, Color4[12]);        // draw "lower freq" button
  LCD_SetCursor(25, 125);                               // have cursor draw at the next line
  LCD_PrintString( "Lower clock freq." );
  
  LCD_DrawFilledCircle(225, 175, 30, Color4[12]);       // draw the "increase frequency" button  
  LCD_SetCursor(175, 125);                              // have cursor draw at the nxt line
  LCD_PrintString( "Increase clock freq." );
}

//////////////////////////////////////////////////////////////
//                  UPDATE FUNCTIONS
//////////////////////////////////////////////////////////////

// updates the temperatures and clock frequency on-screen
void update_Stats(float temperature,
                  enum frequency freq){
  LCD_Goto( strlen(textOne), 0);        // go to end of temperature string
  LCD_SetTextColor(0, 255, 255);        // set text color to cyan  
  LCD_PrintFloat( temperature );
  LCD_Goto( strlen(textOne) + 13, 0);   // go to end of " C, " in the message
  LCD_SetTextColor(255, 255, 0);        // set text color to yellow
  LCD_PrintFloat((1.8 * temperature) + 32);
//  LCD_SetTextColor(255, 255, 255);      // set text color to white
  
  // print the clock freqency
  LCD_Goto(strlen(textFour)+2, 1);    // go to end of frequency text
  LCD_SetTextColor(0, 0, 0);          // set text color to black
  LCD_PrintInteger( 0 );
  
  LCD_Goto(strlen(textFour), 1);    // go to end of frequency text
  LCD_SetTextColor(0, 255, 0);          // set text color to lime
  LCD_PrintInteger( freq );
}

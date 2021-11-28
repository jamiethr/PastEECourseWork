/**
 * EE/CSE 474: Lab3 drivers starter code
 */

#include "PLL_Header.h"
#include "Lab3_Inits.h"
#include <stdio.h>

// STEP 0a: Include your header file here
#include "Thorup_Header.h"
   
volatile unsigned short delay = 0;      // delay used throughout

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

void LED_Init(void) {
  // STEP 1: Initialize the 4 on board LEDs by initializing the corresponding
  // GPIO pins.
  RCGCGPIO |= 0x1120;   // Enable PortF, PortJ, PortN GPIO
  delay++;    // Delay 2 more cycles before access Timer registers
  delay++;    // Refer to Page. 756 of Datasheet for info
  
  GPIODEN_F = 0x11;   // Enables PF4's, PF0's (D3, D4 LEDs) digital port features
  GPIODEN_N = 0x3;    //Enables PN1, PN0 (D1, D2 LEDs) to digital port

  GPIODIR_F = 0x11;     // Set PF4, PF0 to output
  GPIODIR_N = 0x3;      //sets PN1, PN0 to output
  
  GPIODATA_F &= ~0x11;  // set PF4 (D3, D4) to 0
  GPIODATA_N &= ~0x3;   // set PN1, PN0 (D1, D2) to 0
}

void ADCReadPot_Init(void) {
  // STEP 2: Initialize ADC0 SS3.
  // 2.1: Enable the ADC0 clock
  RCGCADC |= 0x1;
  // 2.2: Delay for RCGCADC (Refer to page 1073)
  delay++;      // delay three clock cycles
  delay++;
  delay++;
  delay++;      // one extra delay just in case
  // 2.3: Power up the PLL (if not already)
  PLLFREQ0 |= 0x00800000; // we did this for you
  // 2.4: Wait for the PLL to lock
  while (PLLSTAT != 0x1); // we did this for you
  // 2.5: Configure ADCCC to use the clock source defined by ALTCLKCFG
  ADCCC_0 |= 0x1;
  // 2.6: Enable clock to the appropriate GPIO Modules (Hint: Table 15-1)
  RCGCGPIO |= 0x10;     // enable port E
  // 2.7: Delay for RCGCGPIO
  delay++;
  delay++;
  // 2.8: Set the GPIOAFSEL bits for the ADC input pins
  GPIOAFSEL_E |= 0xF;   // set PE3-0 (to 1) --> controlled by peripheral
  // 2.9: Clear the GPIODEN bits for the ADC input pins
  GPIODEN_E &= ~0xF;    // clear PE3-0
  // 2.10: Disable the analog isolation circuit for ADC input pins (GPIOAMSEL)
  GPIOAMSEL_E |= 0xF;   // disables analog isolation for PE3-0
  // 2.11: Disable sample sequencer 3 (SS3)
  ADCACTSS_0 &= ~0x8;  // disable SS3 for ADC0
  // 2.12: Select timer as the trigger for SS3
  ADCEMUX_0 |= 0x5000;      // select timer trigger for SS3
  // 2.13: Select the analog input channel for SS3 (Hint: Table 15-1)
  ADCSSMUX3_0 |= 0xF;   // configure all pins (PE3-PE0) to act as analog inputs    
  // 2.14: Configure ADCSSCTL3 register
  ADCSSCTL3_0 &= ~0x8;  // clear bit to read from AIN0 --> set to 1 to read from temperature sensor!
  ADCSSCTL3_0 |= 0x6;   // also, raw interrupt is asserted after conversion of sample & set this sample to end of sequence
                        // --> set to 0xE to read from temperature sensor!
  // 2.15: Set the SS3 interrupt mask
  ADCIM_0 |= 0x8;       // raw interrupt signal is sent to ADCRIS, INR3 bit
  // 2.16: Set the corresponding bit for ADC0 SS3 in NVIC
  EN0 |= 0x20000; // enable ADC0_SS3 interrupt (interrupt #17)
  // 2.17: Enable ADC0 SS3
  ADCACTSS_0 |= 0x8;    // enables SS3 in ADC0
  
//  ADCPSSI_0 |= 0x8;     // begin sampling on SS3
}

void TimerADCTriger_Init(void) {
  // STEP 3: Initialize Timer0A to trigger ADC0 at 1 HZ.
  // Hint: Refer to section 13.3.7 of the datasheet

  // YOUR CODE HERE
  RCGCTIMER     |= 0x1;    // enable clock for Timer 0
  GPTMCTL_0     &= ~0x1;   // disable Timer A for Timer 0
  
  GPTMCFG_0     &= 0x00000000;  // reset the GPTM config. register (?) 
  GPTMCFG_0     &= 0x0;         // set the timer to 32-bit mode
  GPTMTAMR_0    |= 0x2;         // set Timer A (& therefore B) to periodic mode
  GPTMTAMR_0    &= ~0x10;       // set timer to count down
  GPTMTAILR_0   = 0x3938700;    // load timer (60 mill ==> 1 Hz)

  GPTMADCEV_0   |= 0x1;         // sends trigger signal to ADC on timeout
  GPTMCTL_0     |= 0x20;        // set TAOTE, enables timer A trigger for ADC
  
  GPTMCTL_0     |= 0x1;   // enable Timer A for Timer 0
  
}

// NEXT STEP: Go to Lab3_Task1a.c and finish implementing ADC0SS3_Handler

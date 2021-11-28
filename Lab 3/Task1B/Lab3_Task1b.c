/**
 * EE/CSE 474: Lab3 Task1a main function
 */

// NOTE: This is the main function for Task 1a. You should start by
// implementing Lab3_Driver.c (STEP 0a, 1-3) and then come back to finish
// the ISR and the main function (STEP 0b, 4-5).

#include <stdint.h>
#include "Lab3_Inits.h"
#include <stdio.h>

// STEP 0b: Include your header file here
#include "Thorup_HeaderT1B.h"

uint32_t ADC_value;
enum frequency freq;    // global variable to set frequency of PLL

int main(void) {
  // Select system clock frequency preset
  freq = PRESET2; // 60 MHz
  PLL_Init(freq);        // Set system clock frequency to 60 MHz
  LED_Init();            // Initialize the 4 onboard LEDs (GPIO)
  initSwitch();          // initialize the user switches and their interrupt
  ADCReadPot_Init();     // Initialize ADC0 to read from the potentiometer
  TimerADCTriger_Init(freq); // Initialize Timer0A to trigger ADC0
  float temperature;
    
  while(1) {}
  return 0;
}

void ADC0SS3_Handler(void) { 
  // 4.2: Save the ADC value to global variable ADC_value
  ADC_value = ADCSSFIFO3_0;     // read value from ADC
  
  float temperature;
  temperature = (147.5 - (247.5 * ADC_value) / 4096.0);         // update temperature
  printf("Temperature: %f\n", temperature);                     // print temperature
  
  GPTMICR_0 |= 0x1;   // clear Timer 0 interrupt flag
  ADCISC_0 |= 0x8;    // clear the ADC interrupt flag  
}

// handles on-board switch input 
void UserSW_Handler( void ){
  GPTMCTL_0     &= ~0x1;  // disable Timer A for Timer 0
  
  if((GPIORIS_J & 0x1) == 0x1){       // if PJ0 (USR_SW1)caused the interrupt
    if(freq == PRESET3){        // if the clock is already set to 12 MHz
      freq = PRESET2;   // set system to 60 MHz
    } else {
      freq = PRESET3;   // set system to 12 MHz
    }
  } else if ((GPIORIS_J & 0x2)>>1 == 0x1) {     // if PJ1 (USR_SW2) caused the interrupt
      freq = PRESET1;       // set system to 120 MHz
  }
  
  // re-initialize the timer and PLL
  TimerADCTriger_Init(freq);    // Initialize Timer0A to trigger ADC0
  PLL_Init(freq);               // Set system clock frequency to 60 MHz
    
  GPIOICR_J |= 0x3;     // clears interrupt bits for PJ1, PJ0
}

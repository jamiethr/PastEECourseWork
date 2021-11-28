/**
 * EE/CSE 474: Lab3 Task1a main function
 */

// NOTE: This is the main function for Task 1a. You should start by
// implementing Lab3_Driver.c (STEP 0a, 1-3) and then come back to finish
// the ISR and the main function (STEP 0b, 4-5).

#include <stdint.h>
#include "Lab3_Inits.h"

// STEP 0b: Include your header file here
#include "Thorup_Header.h"

uint32_t ADC_value;

int main(void) {
  // Select system clock frequency preset
  enum frequency freq = PRESET2; // 60 MHz
  PLL_Init(freq);        // Set system clock frequency to 60 MHz
  LED_Init();            // Initialize the 4 onboard LEDs (GPIO)
  ADCReadPot_Init();     // Initialize ADC0 to read from the potentiometer
  TimerADCTriger_Init(); // Initialize Timer0A to trigger ADC0
  float resistance;
    
  while(1) {
    // STEP 5: Change the pattern of LEDs based on the resistance.
    // 5.1: Convert ADC_value to resistance in kilo-ohm
    resistance = (ADC_value / (4095.0)) * 10.0;
    // 5.2: Change the pattern of LEDs based on the resistance
    int j;
    if (resistance < 2.5) {
      GPIODATA_N |= 0x2;        // set PN1 (D1) to 1
      for (j = 0; j < 5000000; j++) {}
      GPIODATA_N &= ~0x2;       // set PN1 (D1) to 0
      for (j = 0; j < 5000000; j++) {}
    
    } else if (resistance < 5.0) {
      GPIODATA_N |= 0x3;        // set PN1, PN0 (D1, D2) to 1
      for (j = 0; j < 5000000; j++) {}
      GPIODATA_N &= ~0x3;       // set PN1, PN0 (D1, D2) to 0
      for (j = 0; j < 5000000; j++) {}
    
    } else if (resistance < 7.5) {
      GPIODATA_N |= 0x3;        // set PN1, PN0 (D1, D2) to 1
      GPIODATA_F |= 0x10;       // set PF4 (D3) to 1
      for (j = 0; j < 5000000; j++) {}
      GPIODATA_F &= ~0x10;       // set PF4 (D3) to 0
      GPIODATA_N &= ~0x3;   // set PN1, PN0 (D1, D2) to 0
      for (j = 0; j < 5000000; j++) {}
    
    } else {
      GPIODATA_N |= 0x3;        // set PN1, PN0 (D1, D2) to 1
      GPIODATA_F |= 0x11;       // set PF4, PN0 (D3, D4) to 1
      for (j = 0; j < 5000000; j++) {}
      GPIODATA_F &= ~0x11;       // set PF4 (D3, D4) to 0
      GPIODATA_N &= ~0x3;   // set PN1, PN0 (D1, D2) to 0
      for (j = 0; j < 5000000; j++) {}
    }
    printf("Resistance: %f\n", resistance);     // print the resistance
  }
  return 0;
}

void ADC0SS3_Handler(void) {
  // STEP 4: Implement the ADC ISR.
  // 4.1: Clear the ADC0 interrupt flag
//  ADCISC_0 |= 0x8;      // clear the interrupt flag  
  // 4.2: Save the ADC value to global variable ADC_value
  ADC_value = ADCSSFIFO3_0;     // read value from ADC
    
  GPTMICR_0 |= 0x1;   // sets the first bit to one --> clears GPTMRIS
  ADCISC_0 |= 0x8;      // clear the interrupt flag  
}

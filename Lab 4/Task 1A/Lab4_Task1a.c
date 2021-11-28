// Jamie Thorup
// EE 474
// Lab 4 Task 1 & 2
// 11-21-19

#include <stdint.h>
#include "Lab3_Inits.h"
#include <stdio.h>
#include <string.h>

#include "Thorup_HeaderL4T1B.h"

#include "C:\Users\Jamie!\Documents\UW\EE 474\Labs\Lab 4\SSD2119\SSD2119_Display.h"     // include display driver

uint32_t ADC_value;
uint32_t count;         // keeps track of which character to send
char tempC[3];          // temperature string to print (in C)
char tempF[5];          // temperature string to print (in F)enum frequency freq;
enum frequency freq;    // global variable to set frequency of PLL
float temperature;      // gobal temperature variable

int main(void) {
  // Select system clock frequency preset
  freq = PRESET2; // 60 MHz
  count = 0;
  PLL_Init(freq);        // Set system clock frequency to 60 MHz
  initSwitch();          // initialize the user switches and their interrupt
  ADCRead_Init();        // Initialize ADC0 to read from the temperature sensor
  TimerADCTrigger_Init(freq); // Initialize Timer0A to trigger ADC0
  UART0_Init();          // initialize UART0 to be used for serial data transfer
    
  LCD_Init();   // initialize LCD screen
//  LCD_ColorFill(Color4[12]);    // fill screen with bright red
  LCD_SetTextColor(255, 255, 255);    // set text color to white  
  
  while(1) {
    if((UARTFR_2 & 0x10) == 0x0) {      // if UART 2's recieve register is not empty
      char temp = (UARTDR_2 & 0xFF);    // create char variable from recieved data
      UARTDR_2    |= temp;        // put recieved character onto transmit holder
    }    
  }
  return 0;
}

//////////////////////////////////////////////////////////////
//                INTERRUPT HANDLERS
//////////////////////////////////////////////////////////////

void ADC0SS3_Handler(void) { 
  ADC_value = ADCSSFIFO3_0;     // read value from ADC
  
  temperature = (147.5 - (247.5 * ADC_value) / 4096.0);         // update temperature
  printf("Temperature to send: %f\n", temperature);             // print temperature

  // update sentences to print
  // print the first sentence
  LCD_SetCursor(0, 0);  // have cursor draw at the top left of the screen
  LCD_PrintString( "The current temperature is " );
  LCD_SetTextColor(0, 255, 255);        // set text color to cyan  
  LCD_PrintFloat( temperature );
  LCD_SetTextColor(255, 255, 255);      // set text color to white  
  LCD_PrintString(" C, ");
  LCD_SetTextColor(255, 255, 0);        // set text color to yellow
  LCD_PrintFloat((1.8 * temperature) + 32);
  LCD_SetTextColor(255, 255, 255);      // set text color to white
  LCD_PrintString(" F.");
  
  // print the second sentence
  LCD_SetCursor(0, 10);  // have cursor draw at the nxt line
  LCD_PrintString( "The current clock frequency is " );
  LCD_SetTextColor(0, 255, 0);          // set text color to lime
  LCD_PrintFloat( freq );
  LCD_SetTextColor(255, 255, 255);      // set text color to white
  LCD_PrintString( " MHz." );

  
  sprintf(tempC, "%f", temperature);                    // update temp. in C to send to LCD
  sprintf(tempF, "%f", (1.8 * temperature) + 32);       // update temp. in F to send to LCD

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
  TimerADCTrigger_Init(freq);   // Initialize Timer0A to trigger ADC0
  PLL_Init(freq);               // Set system clock frequency to 60 MHz
    
  GPIOICR_J |= 0x3;     // clears interrupt bits for PJ1, PJ0
}

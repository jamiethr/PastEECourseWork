// Jamie Thorup
// EE 474
// Lab 4 Task 1 & 2
// 11-21-19

#include <stdint.h>
#include "Lab3_Inits.h"
#include <stdio.h>

#include "Thorup_HeaderL4T1B.h"

#include "SSD2119_Display.h"     // include display driver
#include "SSD2119_Touch.h"       // include touch driver

uint32_t ADC_value;     // value read from ADC
uint32_t count;         // keeps track of which character to send
char tempC[3];          // temperature string to print (in C)
char tempF[5];          // temperature string to print (in F)
enum frequency freq;    // global variable to set frequency of PLL
float temperature;      // gobal temperature variable

unsigned long x;        // x coordinate read from touch screen
unsigned long y;        // y coordinate read from touch screen
unsigned char pressed;  // used to determine if a virtual button is pressed


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
  Touch_Init(); // initialize LCD's touch inputs
  text_Init();  // initialize output text
  button_Init();        // initialize the on-screen buttons
  
  
  while(1) {
    
    // update X, Y input coordinates from the touch screen
    x = Touch_ReadX();
    y = Touch_ReadY();
    
    // respond to virtual button presses
    if((x > 5) && (y > 5)){     // account for noise at low coordinates
      printf("X: %ld\n", x);
      printf("Y: %ld\n", y);
      
      // check for "lower freq" on-screen button press
      if((x > 500) && (x < 1400) &&
         (y > 500) && (y < 1200) && (pressed == 0x0)){
             printf("lower freq pls\n");
             GPIODATA_J |= 0x1;                         // set data corresponding to PJ0 to 1
//             LCD_DrawFilledCircle(75, 175, 30, Color4[4]);       // draw pressed "lower" button
             pressed = 0x1F4;
             
              freq = PRESET3;   // set system to 12 MHz
              TimerADCTrigger_Init(freq);   // Initialize Timer0A to trigger ADC0
              PLL_Init(freq);               // Set system clock frequency to 60 MHz
      
             // check for "higher freq" on-screen button
      } else if((x > 1400) && (x < 2100) &&
                (y > 700) && (y < 1400) && (pressed == 0x0)) {
             printf("raise freq pls\n");
//             LCD_DrawFilledCircle(225, 175, 30, Color4[4]);     // draw pressed "increase" button
             pressed = 0x1F4;
             
              freq = PRESET1;       // set system to 120 MHz
              TimerADCTrigger_Init(freq);   // Initialize Timer0A to trigger ADC0
              PLL_Init(freq);               // Set system clock frequency to 60 MHz

      } 
                
    }
    if(pressed > 0x0){  // if we're just receiving low-level noise, start resetting button input
      pressed--;                
    }
//    LCD_DrawFilledCircle(75, 175, 30, Color4[12]);      // draw unpressed "lower" button
//    LCD_DrawFilledCircle(225, 175, 30, Color4[12]);     // draw unpressed "increase" button
  }
  return 0;
}

//////////////////////////////////////////////////////////////
//                INTERRUPT HANDLERS
//////////////////////////////////////////////////////////////

void ADC0SS3_Handler(void) { 
  ADC_value = ADCSSFIFO3_0;     // read value from ADC
  
  temperature = (147.5 - (247.5 * ADC_value) / 4096.0);         // update temperature
//  printf("Temperature to send: %f\n", temperature);             // print temperature
  
  // print the updated temperatures on the screen
  update_Stats(temperature, freq);
  
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

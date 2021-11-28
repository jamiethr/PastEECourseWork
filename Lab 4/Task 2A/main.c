// Jamie Thorup
// EE 474
// Lab 4 Task 2 A
// 11-30-20

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "task2A.h"

#include "SSD2119_Display.h"     // include display driver
#include "SSD2119_Touch.h"       // include touch driver

int main()
{
  freq = PRESET2; // 60 MHz
  PLL_Init(freq);        // Set system clock frequency to 60 MHz
  
  initPerifs(); // initialize the external LEDs, switches
  initTimer();  // initialize the timer
  
  LCD_Init();   // initialize LCD screen
  LCD_ColorFill(Color4[0]);     // set screen to black
  
  Touch_Init(); // initialize LCD's touch inputs
  
  s = goInit(); // initialize the traffic light to the its initial state
  
  while(1){
    getTouch();
  }
  return 0;
}

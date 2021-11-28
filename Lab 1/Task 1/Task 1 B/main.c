// Jamie Thorup
// EE 474
// Lab 1
// Task 1 B

#include <stddef.h>
#include <stdint.h>
#include "task1B.h"

int main()
{
    volatile unsigned short delay = 0;
    RCGCGPIO |= 0x1120;   // Enable PortF, PortJ, PortN GPIO
    delay++;    // Delay 2 more cycles before access Timer registers
    delay++;    // Refer to Page. 756 of Datasheet for info
    
    GPIODEN_F = 0x11;   // Enables PF4's, PF0's digital port features
    GPIODEN_N = 0x3;    //Enables PN1, PN0 (D1, D2 LEDs) to digital port
    GPIODEN_J = 0x3;    //Enables PJ1, PJ0 (USR_SW2, USR_SW1) to digital port

    GPIODIR_F = 0x11;     // Set PF4, PF0 to output
    GPIODIR_N = 0x3;      //sets PN1, PN0 to output
//    GPIODIR_J =         //by default, PJ1, PJ0 are inputs (reg. value is zero)
    
    //to write to GPIOCR, we must set the status of GPIOLOCK reg. to unlocked.
    GPIOLOCK_J = 0x4C4F434B;      //must set it this special value to unlock it.
    //to enable pull-up resistors, we need to enable same bits in GPIOCR
    GPIOCR_J = 0x3;       //identical bit assignment as GPIOPUR as to allow
                          //  GPIOPUR to enable pull-up resistors for same bits
    GPIOPUR_J = 0x3;      //enables weak pull-up resistor for PJ1, PJ0;
    
    
    while(1){
        // D1 to SW1 ==> PN1 |= (PJ0 << 1) & (0x2)
        GPIODATA_N = ((~GPIODATA_J << 1) & (0x2));   //0000_0010
//        GPIODATA_N = ((GPIODATA_J & 0x2)) << 1;
        // D2 to SW2
        GPIODATA_N = ((~GPIODATA_J >> 1) & (0x1));
    }
  return 0;
}

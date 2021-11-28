/**
 * EE/CSE 474: Lab3 drivers header file
 */

// NOTE: All necessary functions for Task 1a have been declared in
// this header file, and thus you should not modify it for task 1a.
// However, you may modify it if you want to reuse it for other tasks.

#ifndef _Lab3_Inits_H_
#define _Lab3_Inits_H_

// Preset frequency for 120 MHZ, 60 MHZ, and 12 MHZ clock
// Pass in the preset to Timer1_Init and PLL_Init to configure
// the system clock to specified frequency
enum frequency {PRESET1 = 120, PRESET2 = 60, PRESET3 = 12};

// Initializes the PLL module and generates a system clock frequency
// that equal to the frequency preset.
// Returns 1 if configured successfully, -1 if you select a non-exist preset.
int PLL_Init(enum frequency freq);

// Initializes the 4 onboard LEDs.
void LED_Init(void);

// Initializes ADC Module 0 Sample Sequencer 3. The module is triggered by
// Timer module 0 subtimer A at 1 HZ. The interupt of ADC0 SS3 will be
// generated whenever the A/D conversion is done.
void ADCReadPot_Init(void);

// Initializes Timer 0A to trigger ADC0 at 1 HZ.
void TimerADCTriger_Init(enum frequency freq);


/* ============================================================================
    note: not sure why, but my functions would throw undefined errors when
          I put them in my own header. So I put them here instead.
   ============================================================================ */

//functions used in my code
void initSwitch(void);
void UART0_Init(void);
// user switch init. and interrupt registers 
#define GPIORIS_J       (*((volatile uint32_t *) 0x40060414 ))
#define GPIOIS_J        (*((volatile uint32_t *) 0x40060404 ))
#define GPIOIBE_J       (*((volatile uint32_t *) 0x40060408 ))
#define GPIOIEV_J       (*((volatile uint32_t *) 0x4006040C )) 
#define GPIOIM_J        (*((volatile uint32_t *) 0x40060410 )) 
#define GPIOICR_J       (*((volatile uint32_t *) 0x4006041C ))

#define GPIODEN_J       (*((volatile uint32_t *) 0x4006051C ))
#define GPIODIR_J       (*((volatile uint32_t *) 0x40060400 ))
#define GPIOLOCK_J      (*((volatile uint32_t *) 0x40060520 ))
#define GPIOCR_J        (*((volatile uint32_t *) 0x40060524 ))
#define GPIOPUR_J       (*((volatile uint32_t *) 0x40060510 ))

#define EN1             (*((volatile uint32_t *) 0xE000E104 ))

#define PRI12           (*((volatile uint32_t *) 0xE000E430 ))

#endif  // _Lab3_Inits_H_

// Jamie Thorup
// EE 474
// Lab 2 Task 1 A
// 10-23-20

#ifndef __HEADER1_H__
#define __HEADER1_H__

// include function declarations
void initLED(void);             // initializes GPIO for the on-board LEDs
void initTimer(void);           // initializes Timer 0 in 32-bit, periodic mode
void pollTimer0(void);          // polls timer 0 (counts down every second)

// definitions for timer stuff
#define RCGCTIMER       (*((volatile uint32_t *) 0x400FE604 ))
#define GPTMCTL_0       (*((volatile uint32_t *) 0x4003000C ))
#define GPTMCFG_0       (*((volatile uint32_t *) 0x40030000 ))
#define GPTMTAMR_0      (*((volatile uint32_t *) 0x40030004 ))
#define GPTMTAILR_0     (*((volatile uint32_t *) 0x40030028 ))
#define GPTMRIS_0       (*((volatile uint32_t *) 0x4003001C ))
#define GPTMICR_0       (*((volatile uint32_t *) 0x40030024 ))


// below is from Task 1A of Lab 1
#define RCGCGPIO        (*((volatile uint32_t *) 0x400FE608 ))
#define GPIODEN_F       (*((volatile uint32_t *) 0x4005D51C ))
#define GPIODEN_N       (*((volatile uint32_t *) 0x4006451C ))

#define GPIODIR_F       (*((volatile uint32_t *) 0x4005D400 ))
#define GPIODIR_N       (*((volatile uint32_t *) 0x40064400 ))

#define GPIODATA_F      (*((volatile uint32_t *) 0x4005D3FC ))
#define GPIODATA_N      (*((volatile uint32_t *) 0x400643FC ))

#endif  //__HEADER1_H__
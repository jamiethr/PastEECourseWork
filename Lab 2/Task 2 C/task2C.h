// Jamie Thorup
// EE 474
// Lab 2 Task 2 C
// 11-3-20

// declare pointers for the typedef variables
//typedef struct events *Events_ptr;
//typedef struct state *State_ptr;

typedef enum{init, go, warn, stop} state;       //define FSM states
typedef enum{start_stop, ped_button, noIn} events;    //define FSM events

// function prototypes
void initPerifs(void);             // initializes GPIO for the on-board LEDs
state fsmTick();
events getEvent(void);
state goInit(void);
state goGo(void);
state goWarn(void);
state goStop(void);
void initTimer(void);           // initializes Timer 0 in 32-bit, periodic mode
void pollTimer0(void);          // polls timer 0 (counts down every second)
void pollTimer1(void);


#ifndef __HEADER1_H__
#define __HEADER1_H__

// definitions for interrupts
#define GPTMIMR_0       (*((volatile uint32_t *) 0x40030018 ))
#define GPTMIMR_1       (*((volatile uint32_t *) 0x40031018 ))

#define GPTMICR_0       (*((volatile uint32_t *) 0x40030024 ))
#define GPTMICR_1       (*((volatile uint32_t *) 0x40031024 ))

#define EN0             (*((volatile uint32_t *) 0xE000E100 ))
#define EN1             (*((volatile uint32_t *) 0xE000E104 ))

#define PRI13           (*((volatile uint32_t *) 0xE000E434 ))

#define GPIOIS_L        (*((volatile uint32_t *) 0x40062404 ))
#define GPIOIBE_L       (*((volatile uint32_t *) 0x40062408 ))
#define GPIOIEV_L       (*((volatile uint32_t *) 0x4006240C ))
#define GPIOIM_L        (*((volatile uint32_t *) 0x40062410 ))
#define GPIOICR_L       (*((volatile uint32_t *) 0x4006241C ))

// definitions for timers
#define RCGCTIMER       (*((volatile uint32_t *) 0x400FE604 ))

#define GPTMCTL_0       (*((volatile uint32_t *) 0x4003000C ))
#define GPTMCTL_1       (*((volatile uint32_t *) 0x4003100C ))

#define GPTMCFG_0       (*((volatile uint32_t *) 0x40030000 ))
#define GPTMCFG_1       (*((volatile uint32_t *) 0x40031000 ))

#define GPTMTAMR_0      (*((volatile uint32_t *) 0x40030004 ))
#define GPTMTAMR_1      (*((volatile uint32_t *) 0x40031004 ))

#define GPTMTAILR_0     (*((volatile uint32_t *) 0x40030028 ))
#define GPTMTAILR_1     (*((volatile uint32_t *) 0x40031028 ))

#define GPTMRIS_0       (*((volatile uint32_t *) 0x4003001C ))
#define GPTMRIS_1       (*((volatile uint32_t *) 0x4003101C ))

// definitions for GPIO
#define RCGCGPIO        (*((volatile uint32_t *) 0x400FE608 ))

#define GPIOAMSEL_L     (*((volatile uint32_t *) 0x40062528 ))
#define GPIOAFSEL_L     (*((volatile uint32_t *) 0x40062420 ))

#define GPIODEN_F       (*((volatile uint32_t *) 0x4005D51C ))
#define GPIODEN_N       (*((volatile uint32_t *) 0x4006451C ))
#define GPIODEN_J       (*((volatile uint32_t *) 0x4006051C ))
#define GPIODEN_L       (*((volatile uint32_t *) 0x4006251C ))

#define GPIODIR_F       (*((volatile uint32_t *) 0x4005D400 ))
#define GPIODIR_N       (*((volatile uint32_t *) 0x40064400 ))
#define GPIODIR_J       (*((volatile uint32_t *) 0x40060400 ))
#define GPIODIR_L       (*((volatile uint32_t *) 0x40062400 ))

#define GPIOLOCK_J      (*((volatile uint32_t *) 0x40060520 ))
#define GPIOCR_J        (*((volatile uint32_t *) 0x40060524 ))
#define GPIOPUR_J       (*((volatile uint32_t *) 0x40060510 ))

#define GPIODATA_F      (*((volatile uint32_t *) 0x4005D3FC ))
#define GPIODATA_N      (*((volatile uint32_t *) 0x400643FC ))
#define GPIODATA_J      (*((volatile uint32_t *) 0x400603FC ))
#define GPIODATA_L      (*((volatile uint32_t *) 0x400623FC ))

#endif //__HEADER1_H__
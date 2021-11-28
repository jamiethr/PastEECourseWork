// Jamie Thorup
// EE 474
// Lab 4 Task 2 A
// 11-30-20

//#include <stddef.h>

#ifndef __HEADER1_H__
#define __HEADER1_H__

typedef enum{init, go, warn, stop} state;       //define FSM states
typedef enum{start_stop, ped_button, noIn} events;    //define FSM events
enum frequency {PRESET1 = 120, PRESET2 = 60, PRESET3 = 12};     // define frequencies used by PLL
extern volatile state s;        // global variable for the state of FSM
extern enum frequency freq;     // global variable to set frequency of PLL

// function prototypes

extern void initPerifs(void);           // initializes GPIO for the on-board LEDs
extern void initTimer(void);            // initializes Timer 0 in 32-bit, periodic mode

extern events getEvent(void);           // updates events variable depending on button press
extern void getTouch(void);             // polls SDD's touch screen to see if an on-screen button has been pressed

extern state fsmTick();                 // FSM tick function
extern state goInit(void);              // updates FSM to be in initial state
extern state goGo(void);                // updates FSM to be in 'go' state
extern state goWarn(void);              // updates FSM to be in 'warn' state
extern state goStop(void);              // updates FSM to be in 'stop' state

// function prototypes from past labs

// Initializes the PLL module and generates a system clock frequency
// that equal to the frequency preset.
// Returns 1 if configured successfully, -1 if you select a non-exist preset.
extern int PLL_Init(enum frequency freq);

#endif //__HEADER1_H__
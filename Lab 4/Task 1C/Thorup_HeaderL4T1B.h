// Jamie Thorup
// Lab 3
// Task 1 B 

#ifndef __HEADER1_H__
#define __HEADER1_H__

// initializes the user switches
void initSwitch(void);

// initializes UART 0
void UART0_Init(void);

// initializes the on-screen text
void text_Init(void);

// initializes the on-screen buttons
void button_Init(void);

// updates the onscreen measurements
void update_Stats( float temperature,
                   enum frequency freq);

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

#define GPIODATA_J      (*((volatile uint32_t *) 0x400603FC ))

#define EN1             (*((volatile uint32_t *) 0xE000E104 ))

#define PRI12           (*((volatile uint32_t *) 0xE000E430 ))

// UART registers
#define RCGCUART        (*((volatile uint32_t *) 0x400FE618 ))
#define GPIOAFSEL_A     (*((volatile uint32_t *) 0x40058420 ))
#define GPIOPCTL_A      (*((volatile uint32_t *) 0x4005852C ))
#define GPIODEN_A       (*((volatile uint32_t *) 0x4005851C ))

#define UARTCTL_0       (*((volatile uint32_t *) 0x4000C030 ))
#define UARTCTL_2       (*((volatile uint32_t *) 0x4000E030 ))

#define UARTLCRH_0      (*((volatile uint32_t *) 0x4000C02C ))
#define UARTLCRH_2      (*((volatile uint32_t *) 0x4000E02C ))

#define UARTIBRD_0      (*((volatile uint32_t *) 0x4000C024 ))
#define UARTIBRD_2      (*((volatile uint32_t *) 0x4000E024 ))

#define UARTFBRD_0      (*((volatile uint32_t *) 0x4000C028 ))
#define UARTFBRD_2      (*((volatile uint32_t *) 0x4000E028 ))

#define UARTFR_0        (*((volatile uint32_t *) 0x4000C018 ))
#define UARTFR_2        (*((volatile uint32_t *) 0x4000E018 ))

#define UARTDR_0        (*((volatile uint32_t *) 0x4000C000 ))
#define UARTDR_2        (*((volatile uint32_t *) 0x4000E000 ))

#define EN0             (*((volatile uint32_t *) 0xE000E100 ))

// ADC registers
#define RCGCADC         (*((volatile uint32_t *) 0x400FE638 ))
#define ADCCC_0         (*((volatile uint32_t *) 0x40038FC8 ))

#define GPIOAFSEL_E     (*((volatile uint32_t *) 0x4005C420 ))
#define GPIODEN_E       (*((volatile uint32_t *) 0x4005C51C ))
#define GPIOAMSEL_E     (*((volatile uint32_t *) 0x4005C528 ))

#define ADCACTSS_0      (*((volatile uint32_t *) 0x40038000 ))
#define ADCEMUX_0       (*((volatile uint32_t *) 0x40038014 ))
#define ADCSSMUX3_0     (*((volatile uint32_t *) 0x400380A0 ))
#define ADCSSCTL3_0     (*((volatile uint32_t *) 0x400380A4 ))
#define ADCIM_0         (*((volatile uint32_t *) 0x40038008 ))
#define ADCACTSS_0      (*((volatile uint32_t *) 0x40038000 ))

#define GPTMADCEV_0     (*((volatile uint32_t *) 0x40030070 ))

#define ADCPSSI_0       (*((volatile uint32_t *) 0x40038028 ))
#define ADCSSFIFO3_0    (*((volatile uint32_t *) 0x400380A8 ))

#define ADCEMUX_0       (*((volatile uint32_t *) 0x40038014 ))
#define ADCISC_0        (*((volatile uint32_t *) 0x4003800C ))

// registers for Timer 0
#define RCGCTIMER       (*((volatile uint32_t *) 0x400FE604 ))
#define GPTMCTL_0       (*((volatile uint32_t *) 0x4003000C ))
#define GPTMCFG_0       (*((volatile uint32_t *) 0x40030000 ))
#define GPTMTAMR_0      (*((volatile uint32_t *) 0x40030004 ))
#define GPTMTAILR_0     (*((volatile uint32_t *) 0x40030028 ))
#define GPTMRIS_0       (*((volatile uint32_t *) 0x4003001C ))
#define GPTMICR_0       (*((volatile uint32_t *) 0x40030024 ))

// registers for setting up the LEDs
#define RCGCGPIO        (*((volatile uint32_t *) 0x400FE608 ))
#define GPIODEN_F       (*((volatile uint32_t *) 0x4005D51C ))
#define GPIODEN_N       (*((volatile uint32_t *) 0x4006451C ))

#define GPIODIR_F       (*((volatile uint32_t *) 0x4005D400 ))
#define GPIODIR_N       (*((volatile uint32_t *) 0x40064400 ))

#define GPIODATA_F      (*((volatile uint32_t *) 0x4005D3FC ))
#define GPIODATA_N      (*((volatile uint32_t *) 0x400643FC ))

#endif //__HEADER1_H__
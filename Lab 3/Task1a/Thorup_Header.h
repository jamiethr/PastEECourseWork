// Jamie Thorup
// Lab 3
// Task 1 A 

#ifndef __HEADER1_H__
#define __HEADER1_H__

// ADC stuff
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

#define EN0             (*((volatile uint32_t *) 0xE000E100 ))

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
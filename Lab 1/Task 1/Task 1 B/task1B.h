// Jamie Thorup
// EE 474
// Lab 1
// Task 1 B

#ifndef __HEADER1_H__
#define __HEADER1_H__

#define RCGCGPIO        (*((volatile uint32_t *) 0x400FE608 ))
#define GPIODEN_F       (*((volatile uint32_t *) 0x4005D51C ))
#define GPIODEN_N       (*((volatile uint32_t *) 0x4006451C ))
#define GPIODEN_J       (*((volatile uint32_t *) 0x4006051C ))

#define GPIODIR_F       (*((volatile uint32_t *) 0x4005D400 ))
#define GPIODIR_N       (*((volatile uint32_t *) 0x40064400 ))
#define GPIODIR_J       (*((volatile uint32_t *) 0x40060400 ))

#define GPIOLOCK_J      (*((volatile uint32_t *) 0x40060520 ))
#define GPIOCR_J        (*((volatile uint32_t *) 0x40060524 ))
#define GPIOPUR_J       (*((volatile uint32_t *) 0x40060510 ))

#define GPIODATA_F      (*((volatile uint32_t *) 0x4005D3FC ))
#define GPIODATA_N      (*((volatile uint32_t *) 0x400643FC ))
#define GPIODATA_J      (*((volatile uint32_t *) 0x400603FC ))

#endif //__HEADER1_H__
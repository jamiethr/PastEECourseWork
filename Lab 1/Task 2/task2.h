// Jamie Thorup
// EE 474
// Lab 1
// Task 2

#ifndef __HEADER1_H__
#define __HEADER1_H__

#define RCGCGPIO         (*((volatile uint32_t *) 0x400FE608 ))

#define GPIOLOCK_C       (*((volatile uint32_t *) 0x4005A520 ))
#define GPIOCR_C         (*((volatile uint32_t *) 0x4005A524 ))

#define GPIOAMSEL_C      (*((volatile uint32_t *) 0x4005A528 ))
#define GPIOAMSEL_L      (*((volatile uint32_t *) 0x40062528 ))

#define GPIODIR_C        (*((volatile uint32_t *) 0x4005A400 ))
#define GPIODIR_L        (*((volatile uint32_t *) 0x40062400 ))
#define GPIODIR_F        (*((volatile uint32_t *) 0x4005D400 ))
#define GPIODIR_N        (*((volatile uint32_t *) 0x40064400 ))

#define GPIOAFSEL_C      (*((volatile uint32_t *) 0x4005A420 ))
#define GPIOAFSEL_L      (*((volatile uint32_t *) 0x40062420 ))

#define GPIODEN_C        (*((volatile uint32_t *) 0x4005A51C ))
#define GPIODEN_L        (*((volatile uint32_t *) 0x4006251C ))
#define GPIODEN_F        (*((volatile uint32_t *) 0x4005D51C ))
#define GPIODEN_N        (*((volatile uint32_t *) 0x4006451C ))

#define GPIODATA_C       (*((volatile uint32_t *) 0x4005A3FC ))
#define GPIODATA_L       (*((volatile uint32_t *) 0x400623FC ))
#define GPIODATA_F       (*((volatile uint32_t *) 0x4005D3FC ))       
#define GPIODATA_N       (*((volatile uint32_t *) 0x400643FC ))

#endif  //__HEADER1_H__
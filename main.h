/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

//#define USE_STDPERIPH_DRIVER 1

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define GPIO_HIGH(a,b) 		a->BSRRL = b
#define GPIO_LOW(a,b)			a->BSRRH = b
#define GPIO_TOGGLE(a,b) 	a->ODR ^= b 

/* Exported functions ------------------------------------------------------- */
void NVIC_Conf(void);
void Voltage_Conf(void);
void GPIO_Conf(void);
void SysTick_Conf(void);
void TIM3_Conf(void);

#endif /* __MAIN_H */

#ifndef REAL_TIME_H
#define REAL_TIME_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include <stdio.h>

void SuperDebounce(uint16_t * key_state, volatile uint16_t *KPIN,
		uint16_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(void), void (*rep_proc)(void) );
		
void kl1_press(void);
void kl1_rep(void);
void kl2_press(void);
void kl2_rep(void);
void kl3_press(void);
void kl3_rep(void);
void kl4_press(void);
void kl4_rep(void);

#endif

#include "SuperDebounce.h"

volatile uint16_t Timer1;									//8 bitowe timery programowe 100Hz
uint16_t k1, k2, k3, k4;													//Zmienne pomocnicza do funcji SuperDebounce (tutaj dla dwóch przycisków)

/************** funkcja SuperDebounce do obslugi pojedynczych klawiszy ***************
 * 							AUTOR: Miroslaw Kardas
 * ZALETY:
 * 		- nie wprowadza najmniejszego spowalnienia
 * 		- posiada funkcje REPEAT (powtarzanie akcji dla dluzej wcisnietego przycisku)
 * 		- mozna przydzielic rózne akcje dla trybu REPEAT i pojedynczego klikniecia
 * 		- mozna przydzielic tylko jedna akcje wtedy w miejsce drugiej przekazujemy 0 (NULL)
 *
 * Wymagania:
 * 	Timer programowy utworzony w oparciu o Timer sprzetowy (przerwanie 100Hz)
 *
 * 	Parametry wejsciowe:
 * 	*key_state - wskaznik na zmienna w pamieci RAM (1 bajt)
 *  *KPIN - nazwa PINx portu na którym umieszczony jest klawisz, np: PINB
 *  key_mask - maska klawisza np: (1<<PB3)
 *  rep_time - czas powtarzania funkcji rep_proc w trybie REPEAT
 *  rep_wait - czas oczekiwania do przejscia do trybu REPEAT
 *  push_proc - wskaznik do wlasnej funkcji wywolywanej raz po zwolenieniu przycisku
 *  rep_proc - wskaznik do wlasnej funkcji wykonywanej w trybie REPEAT
 
 np. SuperDebounce(&k1, &(GPIOB->IDR), GPIO_Pin_15, 200, 1000, kl1_press, kl1_rep) - gdzie (GPIOB->IDR) to rejestr wejciowy portu B
 **************************************************************************************/
void SuperDebounce(uint16_t * key_state, volatile uint16_t *KPIN,
		uint16_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(void), void (*rep_proc)(void) ) 
{

	enum {idle, debounce, go_rep, wait_rep, rep};

	uint16_t key_press = !(*KPIN & key_mask);
	
	if(!rep_time) rep_time=20;			//czasy domyslne
	if(!rep_wait) rep_wait=150;			//czasy domyslne
	
	if( key_press && !*key_state ) 
	{
		*key_state = debounce;
		Timer1 = 5;
	} else if( *key_state  ) 
	{

		if( key_press && debounce==*key_state && !Timer1 ) {
			*key_state = 2;
			Timer1=3;
		} else
		if( !key_press && *key_state>1 && *key_state<4 ) {
			if(push_proc) push_proc();						// KEY_UP
			*key_state=idle;
		} else
		if( key_press && go_rep==*key_state && !Timer1 ) {
			*key_state = wait_rep;
			Timer1=rep_wait;
		} else
		if( key_press && wait_rep==*key_state && !Timer1 ) {
			*key_state = rep;
		} else
		if( key_press && rep==*key_state && !Timer1 ) {
			Timer1 = rep_time;
			if(rep_proc) rep_proc();						// KEY_REP
		}
	}
	if( *key_state>=3 && !key_press ) *key_state = idle;
}

//void kl1_press(void)
//{
//	return;
//}

//void kl1_rep(void)
//{
//	return;
//}

//void kl2_press(void)
//{
//	return;
//}

//void kl2_rep(void)
//{
//	return;
//}

//void kl3_press(void)
//{
//	return;
//}
//void kl3_rep(void)
//{
//	return;
//}

//void kl4_press(void)
//{
//	return;
//}

//void kl4_rep(void)
//{
//	return;
//}

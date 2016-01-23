#include "main.h"
#include "SuperDebounce.h"

extern uint16_t k1, k2, k3, k4;																																

//
uint8_t ShortLED_State = 0;
uint8_t LongLED_State = 0;
float ShortLED_Value = 0.15f;
float LongLED_Value = 0.15f;


int main(void)
{
	NVIC_Conf();
	SysTick_Conf();
	GPIO_Conf();
	TIM3_Conf();
	
	
	//GPIO_LOW(GPIOA,GPIO_Pin_6);				//PWM1
	
  while (1)
  {
		SuperDebounce(&k1, &(GPIOB->IDR), GPIO_Pin_15, 0, 1000, kl1_press, kl1_rep); //pierwszy czas (w ms) to oczekiwanie pomiedzy kolejnymi powtórzeniami przy przytrzymaniu klawisza
		SuperDebounce(&k2, &(GPIOB->IDR), GPIO_Pin_14, 0, 1000, kl2_press, kl2_rep); //drugi to czas pomiedzy nacisnieciem przycisku a wykryciem jego przytrzymania
		SuperDebounce(&k3, &(GPIOB->IDR), GPIO_Pin_13, 0, 1000, kl3_press, kl3_rep);
		SuperDebounce(&k4, &(GPIOB->IDR), GPIO_Pin_12, 0, 1000, kl4_press, kl4_rep);
		
		
  }
}

void RCC_Conf(void)
{
	
}

void GPIO_Conf(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC, ENABLE);
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	{
	/*-- Uziemienie nieuzywanych linii --*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, DISABLE);
	/*-----------------------------------*/
	}
}

void NVIC_Conf(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	#ifdef  VECT_TAB_RAM  
  		// Jezeli tablica wektorow w RAM, to ustaw jej adres na 0x20000000
  		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  // VECT_TAB_FLASH
  		// W przeciwnym wypadku ustaw na 0x08000000
  		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif
	
	// Wlacz przerwanie od PWM_TIM
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


void SysTick_Conf(void)
{   
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);								// 16MHz/1 = 16MHz	
	//SysTick->Load = 16000;																				// 16000 w rejestrze kalibracyjnym jest wartoscia domyslna dla F4 (dla F1 wynosi ona 9000)
	if (SysTick_Config(SystemCoreClock / 1000))											// Przerwanie generowane co 1ms bo 16MHz/1000 = 16000
	{				
		while(1);																											// W przypadku bledu konfiguracji pozostan w nieskonczonej petli
	}					
}

void TIM3_Conf(void)	//Konfiguracja timera
{
	GPIO_InitTypeDef 					GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef 	TIM_TimeBaseStructure;
	TIM_OCInitTypeDef					TIM_OCInitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Ustawienie wyjscia PWM_TIM jako wyjscia o funkcji alternatywnej
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		
	//Ustawienia ukladu podstawy czasu															// TIM3CLK = SystemCoreClock /1 = 16MHz
	TIM_TimeBaseStructure.TIM_Prescaler = 0;												// Wylacza prescaler
	TIM_TimeBaseStructure.TIM_Period = 32000-1;											// TIM_ARR (Timer Auto Reload Register)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;					// Okresla czestotliwosc taktowania generatora czasu martwego oraz wejsciowych filtrów cyfrowych - czy ma ona zostac dodatkowo podzielona
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 		// Tryb pracy (zliczanie w gore)
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);		  						// Przesyla strukture do funcji inicjalizujacej podstawe czasu timera 3	

	//Konfiguracja kanalu 1												
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;								// Wybór trybu kanalu - tutaj generuje przerwanie gdy timer doliczy do wartoœci TIM_Pulse
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		// Okresla czy sygnal z timera moze byc wykorzystany do sterowania innymi elementami systemu tutaj kontolerem przerwañ VNIC
	TIM_OCInitStructure.TIM_Pulse = 0;															// Okresla po ilu impulsach zostanie wywolane przerwanie
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;				// Wybór stanu aktywnego - wysoki lub niski
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);												// Przesyla strukture do funkcji inicjalizujacej konkretny kanal
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);								// Wlaczenie lub wylaczenie buforowania (preload) - gdy wylaczony ewentualne zmiany konfiguracji uwzgledniane sa od razu natomiast gdy wlaczony to zmiany wchodza w zycie dopiero przy wyzerowaniu timera
	
	//Konfiguracja kanalu 2												
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;								// Wybór trybu kanalu - tutaj generuje przerwanie gdy timer doliczy do wartoœci TIM_Pulse
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		// Okresla czy sygnal z timera moze byc wykorzystany do sterowania innymi elementami systemu tutaj kontolerem przerwañ VNIC
	TIM_OCInitStructure.TIM_Pulse = 0;															// Okresla po ilu impulsach zostanie wywolane przerwanie
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;				// Wybór stanu aktywnego - wysoki lub niski
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);												// Przesyla strukture do funkcji inicjalizujacej konkretny kanal
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);								// Wlaczenie lub wylaczenie buforowania (preload) - gdy wylaczony ewentualne zmiany konfiguracji uwzgledniane sa od razu natomiast gdy wlaczony to zmiany wchodza w zycie dopiero przy wyzerowaniu timera

	//Wlaczenie mechanizmu aktualizacji po przeladowaiu
	TIM_ARRPreloadConfig(TIM3, ENABLE);												// Wlaczenie aktualizacji wartosci rejestru okresu i impulsu dopiero po przeladowaniu timera - ARR (Auto Reload Register)
	
	//Wlaczenie przerwan od wybranego kanalu
  TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);

	TIM_Cmd(TIM3, ENABLE);
	TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Enable);
	TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);
}

void kl1_press(void)																	//Short Up
{
	if(ShortLED_State == 1)															//Sprawdza czy ustawiono flage wlaczajaca krotkie swiatla
	{
		ShortLED_Value += 0.15f;													//Zwikszenie wspolczynnika wypelnienia o 15%
		if(ShortLED_Value > 0.75f) ShortLED_Value = 0.75f;	//Ograniczenie wartosci wsp. wypelnienia jesli przekracza on 100%
		TIM3->CCR1 = (uint16_t)(32000*ShortLED_Value);		//Przeliczenie i wpisanie wartosci do rejestru czasu trwania impulsu
	}
	return;
}

void kl1_rep(void)																		//Short ON
{
	ShortLED_State = 1;																	//Ustawienie flagi stanu krotkich swiatel - ON
	ShortLED_Value = 0.15f;															//Ustawienie wartosci domyslnej wsp. wypelnienia w %
	TIM3->CCR1 = (uint16_t)(32000*ShortLED_Value);			//Przeliczenie i wpisanie wartosci do rejestru czasu trwania impulsu kanalu 1
	return;
}
void kl2_press(void)																	//Short Down
{
	if(ShortLED_State == 1)                             //Sprawdza czy ustawiono flage wlaczajaca krotkie swiatla
	{
		ShortLED_Value -= 0.15f;													//Zmniejszenie wspolczynnika wypelnienia o 15%
		if(ShortLED_Value < 0.15f) ShortLED_Value = 0.15f;	//Ograniczenie wartosci wsp. wypelnienia jesli spada on ponizej 10%
		TIM3->CCR1 = (uint16_t)(32000*ShortLED_Value);		//Przeliczenie i wpisanie wartosci do rejestru czasu trwania impulsu kanalu 1
	}
	return;
}

void kl2_rep(void)																		//Short OFF
{
	ShortLED_State = 0;																	//Zerowanie flagi stanu krotkich swiatel - OFF
	ShortLED_Value = 0.0f;															//Zmniejszenie wsp. wypelnienia do 0% - wylaczenie krotkich swiatel
	TIM3->CCR1 = (uint16_t)(32000*ShortLED_Value);			//Przeliczenie i wpisanie wartosci do rejestru czasu trwania impulsu kanalu 1
	return;
}

void kl3_press(void)																	//Long Up
{
	if(LongLED_State == 1)															//Sprawdza czy ustawiono flage wlaczajaca dlugie swiatla
	{                                                   
		LongLED_Value += 0.15f;                           //Zwikszenie wspolczynnika wypelnienia o 15%
		if(LongLED_Value > 0.75f) LongLED_Value = 0.75f;    //Ograniczenie wartosci wsp. wypelnienia jesli przekracza on 100%
		TIM3->CCR2 = (uint16_t)(32000*LongLED_Value);     //Przeliczenie i wpisanie wartosci do rejestru czasu trwania impulsu
	}
	return;
}
void kl3_rep(void)																		//Long ON
{
	LongLED_State = 1;																	//Ustawienie flagi wlaczajacej dlugie swiatla
	LongLED_Value = 0.15f;																//Ustawienie wartosci domyslnej wsp. wypelnienia w %
	TIM3->CCR2 = (uint16_t)(32000*LongLED_Value);				//Przeliczenie i wpisanie wartosci do rejestru czasu trwania impulsu kanalu 2
	return;
}

void kl4_press(void)																	//Long Down
{
	if(LongLED_State == 1)															//Sprawdza czy ustawiono flage wlaczajaca dlugie swiatla
	{                                                   
		LongLED_Value -= 0.15f;                           //Zmniejszenie wspolczynnika wypelnienia o 15%
		if(LongLED_Value < 0.15f) LongLED_Value = 0.15f;    //Ograniczenie wartosci wsp. wypelnienia jesli spada on ponizej 10%
		TIM3->CCR2 = (uint16_t)(32000*LongLED_Value);     //Przeliczenie i wpisanie wartosci do rejestru czasu trwania impulsu kanalu 2
	}
	return;
}
void kl4_rep(void)																		//Long OFF
{
	LongLED_State = 0;																	//Zerowanie flagi stanu dlugich swiatel - OFF
	LongLED_Value = 0.0f;                               //Zmniejszenie wsp. wypelnienia do 0% - wylaczenie krotkich swiatel
	TIM3->CCR2 = (uint16_t)(32000*LongLED_Value);       //Przeliczenie i wpisanie wartosci do rejestru czasu trwania impulsu kanalu 2
	return;
}


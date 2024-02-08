 /* Includes */
#include "stm32f4xx.h"
int ant=0, at=0, s=0, conttemp=0, contclck=0;
int valorcom=0;
const enum comando{
	b13s, b23s, b33s, b3m
}com;
void TIM1_UP_TIM10_IRQHandler(void)
	{
	TIM10->SR&=~TIM_SR_UIF;

	if (s==1){
		GPIOC->ODR^=GPIO_ODR_ODR_8;
	}
}
void TIM1_TRG_COM_TIM11_IRQHandler(void)
	{
	TIM11->SR&=~TIM_SR_UIF;
	ant=at;
	at=(GPIOB->IDR & GPIO_IDR_IDR_8)>>8;
	if (ant==0 && at==1){
		if (TIM9->SR == TIM_SR_UIF){
		TIM9->SR&=~TIM_SR_UIF;
		}
		conttemp=0;
		contclck++;
	}
	if (ant==1 && at==0){
		conttemp=0;
	}
	if (ant == at){
		conttemp++;
	}
}
void TIM1_BRK_TIM9_IRQHandler(void)
{
	if (conttemp==60){ //60x50ms=3s
		valorcom=b3m;
	}
	if (contclck==1){  //1 click em 3s
		valorcom=b13s;
	}
	if (contclck==2){  //2 clicks em 3s
		valorcom=b23s;
	}
	if (contclck==3 || contclck>3){  //3 ou mais clicks em 3s
		valorcom=b33s;
	}
	contclck=0;
	conttemp=0;
}

int main(void)
{

	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR|=RCC_APB2ENR_TIM9EN|RCC_APB2ENR_TIM10EN|RCC_APB2ENR_TIM11EN;

	GPIOB->MODER&=~(GPIO_MODER_MODER0|GPIO_MODER_MODER1|GPIO_MODER_MODER8);
	GPIOB->PUPDR&=~(GPIO_PUPDR_PUPDR0|GPIO_PUPDR_PUPDR1|GPIO_PUPDR_PUPDR8);
	GPIOB->PUPDR|=(GPIO_PUPDR_PUPDR0_1|GPIO_PUPDR_PUPDR1_1|GPIO_PUPDR_PUPDR8_1);

	GPIOC->MODER&=~GPIO_MODER_MODER8;
	GPIOC->MODER|=GPIO_MODER_MODER8_0;

	TIM9->CR1|=TIM_CR1_CEN;
	TIM9->PSC=15999;
	TIM9->ARR=999;
	TIM9->SR|=TIM_SR_UIF;
	TIM9->DIER|=TIM_DIER_UIE;

	//TIM10->CR1|=TIM_CR1_CEN;
	TIM10->ARR=999;
	TIM10->DIER|=TIM_DIER_UIE;

	TIM11->CR1|=TIM_CR1_CEN; //setado para 50ms
	TIM11->PSC=319;
	TIM11->ARR=999;
	TIM11->DIER|=TIM_DIER_UIE;

	NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
	NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 3);

	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 2);

	NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
	NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 1);

  while (1)
  {
	  switch(valorcom){
	  case b13s:
		  TIM10->CR1|=TIM_CR1_CEN;
		  TIM10->PSC=3999;
	  break;
	  case b23s:
		  TIM10->CR1|=TIM_CR1_CEN;
		  TIM10->PSC=1142;
	  break;
	  case b33s:
		  TIM10->CR1&=~TIM_CR1_CEN;
		  GPIOC->ODR|=GPIO_ODR_ODR_8;
	  break;
	  case b3m:
		  TIM10->CR1&=~TIM_CR1_CEN;
		  GPIOC->ODR&=~GPIO_ODR_ODR_8;
	  break;
	  }
  }
}

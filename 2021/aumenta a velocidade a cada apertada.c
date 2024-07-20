#include "stm32f4xx.h"

/* Defines ------------------------------------------------------------------*/

int main(void)
{
	//clocks;
	RCC->AHB1ENR=0x87;
	RCC->APB2ENR|=RCC_APB2ENR_TIM9EN;
	//setting up i/o;
	//inputs;
	GPIOA->MODER&=~GPIO_MODER_MODER6;
	GPIOA->MODER|=GPIO_MODER_MODER6_0;
	GPIOA->MODER|=GPIO_MODER_MODER5_0;

	//outputs;
	GPIOC->MODER=0x5555;
	//GPIOC->MODER&=GPIO_MODER_MODER0_0; that's how it looks with def. OBS: Only PC0;
	//setting up timers;

	TIM9->PSC=(16000-1);//each ARR = 1ms
	TIM9->ARR=(1000-1);//100ms
	TIM9->CR1|=TIM_CR1_CEN | TIM_CR1_ARPE;//enabling the counter

	int phases[10]={
	0b00111111,//0
	0b00000110,//1
	0b01011011,//2
	0b01001111,//3
	0b01100110,//4
	0b01101101,//5
	0b01111101,//6
	0b00000111,//7
	0b01111111,//8
	0b01101111 //9
				};
	int atualSW0,atualSW1,lastSW0,lastSW1;//flip flop
	int num=0;//number to be displayed
	int order=1;
	int speed=1;

  while (1)
  {
	  atualSW0=(GPIOA->IDR & GPIO_IDR_IDR_6);
	  atualSW1=(GPIOA->IDR & GPIO_IDR_IDR_5);

	  GPIOC->ODR=phases[num];//start with a zero on the display

	  if(num==10)
	  {
		  num=0;
		  GPIOC->ODR=phases[num];
	  }
	  if(num==-1)
	  {
		  num=9;
		  GPIOC->ODR=phases[num];
	  }



	  if(atualSW0 != lastSW0)
	  {
		  lastSW0 = atualSW0;
		  if(atualSW0)
		  {
			  TIM9->ARR=TIM9->ARR+100;
		  }
	  }
	  if(atualSW1 != lastSW1)
	  {
		  lastSW1 = atualSW1;
		  if(atualSW1)
		  {
			  order=order*-1;
		  }
	  }
	  if(order==1)
	  {
		  if (TIM9->SR & TIM_SR_UIF)
		  {
			  TIM9->SR &= ~TIM_SR_UIF;
			  num++;
			  GPIOC->ODR=phases[num];
		  }
	  }
	  if(order==-1)
	  {
		  if (TIM9->SR & TIM_SR_UIF)
		  {
			  TIM9->SR &= ~TIM_SR_UIF;
			  num--;
			  GPIOC->ODR=phases[num];
		  }
	  }
  }
}

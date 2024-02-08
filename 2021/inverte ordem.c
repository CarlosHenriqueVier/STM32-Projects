/*
******************************************************************************
* @author  Augusto Blauth Schneider
  * @date    august,2022
******************************************************************************
*/

#include "stm32f4xx.h"

//VETOR COM OS NUMEROS CRESCENDO
int numeros[10]={0b01111111, //0.
				   0b00001101, //1.
				   0b10110111, //2.
				   0b10011111, //3.
			 	   0b11001101, //4.
				   0b11011011, //5.
				   0b11111011, //6.
				   0b00001111, //7.
				   0b11111111, //8.
				   0b11011111  //9.
      };

int cont=0, ordem=1;
int SWatual, SWantes=0;
int main(void)
{
	//GPIOS
  RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN;//clock GPIOA e GPIOC
  GPIOC->MODER|=0x5555;//PC0 até PC7 como saída
  GPIOA->MODER|=0x0;//GPIOA como entrada
  GPIOA->PUPDR|=GPIO_PUPDR_PUPDR7_0 | GPIO_PUPDR_PUPDR8_0; //pull-up no botão PA7 e PA8 pois liga ao GND (LOW)

    //TIMER
  RCC->APB2ENR|=RCC_APB2ENR_TIM10EN;//clock TIM10
  TIM10->PSC=15999;//PSC = 16k para facilitar as contas
  TIM10->ARR=999;//ARR = 1000 para fechar 1s (1Hz)
  TIM10->CR1|=TIM_CR1_CEN | TIM_CR1_CEN;

  GPIOC->ODR|=0b01111111;//inicia com 0
  while (1)
  {

	  SWatual = GPIOA->IDR & 0x80;//entrada -> PA7

	  //mudança no estado do b
	  		if(SWatual!=SWantes){
	  		     SWantes=SWatual;
	  				if(SWatual){
	  				  ordem = ordem*-1;//inverte ordem (1 para -1, -1 para 1)
	  				  }
	  		}
		if(ordem==1){//crescente
			if(TIM10->SR & TIM_SR_UIF){
				TIM10->SR&= ~TIM_SR_UIF;
				GPIOC->ODR&=~0b11111111;//limpa o display
				GPIOC->ODR|= ~numeros[cont];//mostra o numero
				cont++;//incrementa
				}
		}
		if(ordem==-1){//decrescente
			if(TIM10->SR & TIM_SR_UIF){
				TIM10->SR&= ~TIM_SR_UIF;
				GPIOC->ODR&=~0b11111111;//limpa o display
				GPIOC->ODR|= ~numeros[cont];//mostra o numero
				cont--;//decrementa
				}
		}
	if(cont==-1){cont=9;}
	if(cont==10){cont=0;}
  }
}


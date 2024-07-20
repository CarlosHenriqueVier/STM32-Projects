#include "stm32f4xx.h"

//VETOR COM OS NUMEROS (1 bit a mais para ser o ponto)
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

int cont=0;
int SW;
int main(void)
{
	//GPIOS
  RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN;//clock GPIOA e GPIOC
  GPIOC->MODER|=0x5555;//PC0 até PC7 como saída
  GPIOA->MODER|=0x0;//GPIOA como entrada
  GPIOA->PUPDR|=GPIO_PUPDR_PUPDR7_0; //pull-up no botão PA7 pois liga ao GND (LOW)

    //TIMER
  RCC->APB2ENR|=RCC_APB2ENR_TIM10EN;//clock TIM10
  TIM10->PSC=15999;//PSC = 16k para facilitar as contas
  TIM10->ARR=99;//ARR = 100 para fechar 100ms (10Hz)
  TIM10->CR1|=TIM_CR1_CEN | TIM_CR1_ARPE;//liga o TIM para contar

  GPIOC->ODR|=0b01111111;//inicia com 0
  while (1)
  {
	SW = GPIOA->IDR & 0x80;//entrada -> PA7

	//se o botão foi pressionado
	if(SW){
			TIM10->CR1|=TIM_CR1_CEN; //| TIM_CR1_ARPE;//liga o TIM para contar
			//se contou um ciclo de 100ms
			if(TIM10->SR & TIM_SR_UIF){
				TIM10->SR&= ~TIM_SR_UIF;
				GPIOC->ODR &= ~0b11111111;//limpa o display
				GPIOC->ODR|= ~numeros[cont];//mostra o número
				cont++;//incrementa o contador auxiliar
			}
		if(!SW){
			TIM10->CR1 &= ~TIM_CR1_CEN;	//desliga o timer (pausa)
		}
		//escolha entre parar ou continuar
				if(cont==10){
					cont=0;//reseta o contador auxiliar para continuar infinitamente
					//TIM10->CR1 &= ~TIM_CR1_CEN;	//desliga o timer
				}
	}

  }
}

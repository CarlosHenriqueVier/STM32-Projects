#include "stm32f4xx.h"
#include "main.h"

int main(void)
{
	TIM10->PSC = 15999;
	 TIM10->ARR = 999;
	    TIM10->CR1 |= TIM_CR1_CEN|TIM_CR1_ARPE;

	       GPIOA->MODER &= ~(GPIO_MODER_MODER1|GPIO_MODER_MODER2);
	       GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR1|GPIO_PUPDR_PUPDR2);
	       GPIOA->PUPDR |=  (GPIO_PUPDR_PUPDR1_1|GPIO_PUPDR_PUPDR2_1);

	       GPIOC->MODER &= ~(GPIO_MODER_MODER0|GPIO_MODER_MODER1|GPIO_MODER_MODER2|GPIO_MODER_MODER3|GPIO_MODER_MODER4|GPIO_MODER_MODER5|GPIO_MODER_MODER6|GPIO_MODER_MODER7);
	       GPIOC->MODER |= (GPIO_MODER_MODER0_0|GPIO_MODER_MODER1_0|GPIO_MODER_MODER2_0|GPIO_MODER_MODER3_0|GPIO_MODER_MODER4_0|GPIO_MODER_MODER5_0|GPIO_MODER_MODER6_0|GPIO_MODER_MODER7_0);
	       GPIOC->MODER |= GPIO_MODER_MODER8_0; //ponto
	       GPIOC->ODR |= GPIO_ODR_ODR_8;

	       uint32_t display[10] = {0b0111111, 0b0000110, 0b1011011,0b1001111,0b1100110,0b1101101,0b1111101,0b0000111,0b1111111,0b1101111};

	           GPIOC->ODR |= display[0]; // começa o display no 0. pode ser substitúido por GPIOC->ODR = display[0];

	           int8_t cont = 0; // int8_t -> 8 bytes, com sinal (sem u no inicio). sinal facilita a vida (linha 51)
	           uint8_t estadoAtualPA1 = 0;
	           uint8_t estadoAnteriorPA1 = 0;
	           uint8_t estadoAtualPA2 = 0;
	           uint8_t estadoAnteriorPA2 = 0;

	           while (1) {
	               // troca de estado pode não ser neccesária (caso da troca de ordem da contagem)
	               estadoAtualPA1 = (GPIOA->IDR & GPIO_IDR_IDR_1) >> 1; // troca de estado ("flip-flop")
	               estadoAtualPA2 = (GPIOA->IDR & GPIO_IDR_IDR_2) >> 2; // mesma bosta pro PA5
	               if (estadoAtualPA1 != estadoAnteriorPA1) { // compara e atualiza estado
	                   // se precisar aumentar quando apertar um botão
	                   // estadoAtual é 1 e estadoAnterior é 0 (botão tava aberto e foi fechado)
	                   // inverte pra chave ativa em LOW (atual 0 e anterior 1)
	                   // precisa disso pra não contar 5 num aperto por exemplo, pq vai contar toda vez que o arm processar se não tiver uma leitura de troca de estado
	                   /* if (estadoAtualPA5) {
	                    *     cont++;
	                    * }
	                    *
	                    */
	                   estadoAnteriorPA1 = estadoAtualPA1;
	               }
	               if (estadoAtualPA2 != estadoAnteriorPA2) { // mesma bosta pro PA5
	                   estadoAnteriorPA2 = estadoAtualPA2;
	                   if (estadoAtualPA2) { // troca de velocidade da contagem
	                       TIM10->ARR = 499; // 500ms
	                   } else {
	                       TIM10->ARR = 999; // 1000ms
	                   }
	               }
	               if (TIM10->SR & TIM_SR_UIF) { // Atualiza display
	                   TIM10->SR &= ~(TIM_SR_UIF);
	                   if (cont >= 10) { // faz contar só de 0 a 9
	                       cont = 0;
	                   }
	                   if (cont <= -1) { // mesma coisa, descrescente
	                       cont = 9;
	                   }
	                   GPIOC->ODR &= ~(display[8]); // dá pra substituir por GPIOC->ODR = display[cont];
	                   if (estadoAtualPA1) { // troca de ordem da contagem
	                       cont--; // descrescente
	                   } else {
	                       cont++; // crescente
	                   }
	               }
	           }
	       }

}

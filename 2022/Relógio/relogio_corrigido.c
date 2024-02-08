#include "stm32f4xx.h"

#define PRESCALE_1MS 15999
#define TIMER_1HZ 999
#define TIMER_2HZ 499
#define TIMER_100HZ 9
#define BUTTON_MAIS (GPIOA->IDR & GPIO_IDR_IDR_1)
#define BUTTON_MENOS (GPIOA->IDR & GPIO_IDR_IDR_4)
#define BUTTON_SELECT (GPIOA->IDR & GPIO_IDR_IDR_0)
#define BUTTON_PAUSE (GPIOB->IDR & GPIO_IDR_IDR_0)

int dispSel = 0;
int currentDisp = 0;
int dispTimeArr[6] = {0};
int pisca = 0;
int pause = 0;

const uint16_t dispArr[10] = {
        0b0111111000, // 0
        0b0000110000, // 1
        0b1011011000, // 2
        0b1001111000, // 3
        0b1100110000, // 4
        0b1101101000, // 5
        0b1111101000, // 6
        0b0000111000, // 7
        0b1111111000, // 8
        0b1101111000, // 9
};

void setup() {
    //Clock 1
    RCC->AHB1ENR = 0x87;
    //Clock 2
    RCC->APB2ENR |= RCC_APB2ENR_TIM9EN|RCC_APB2ENR_TIM10EN|RCC_APB2ENR_TIM11EN;

    //Display, GPIOB 3, 4, 5, 6, 7, 8, 9, 10
    GPIOB->MODER &= ~(GPIO_MODER_MODER3|GPIO_MODER_MODER4|GPIO_MODER_MODER5|GPIO_MODER_MODER6|GPIO_MODER_MODER7|GPIO_MODER_MODER8|GPIO_MODER_MODER9|GPIO_MODER_MODER10);
    GPIOB->MODER |= (GPIO_MODER_MODER3_0|GPIO_MODER_MODER4_0|GPIO_MODER_MODER5_0|GPIO_MODER_MODER6_0|GPIO_MODER_MODER7_0|GPIO_MODER_MODER8_0|GPIO_MODER_MODER9_0|GPIO_MODER_MODER10_0);

    //Ponto selecao GPIOB 2
    GPIOB->MODER &= ~(GPIO_MODER_MODER2);
    GPIOB->MODER |= (GPIO_MODER_MODER2_0);

    //Controle displays, GPIOC 0, 1, 2, 3, 4, 5
    GPIOC->MODER &= ~(GPIO_MODER_MODER0|GPIO_MODER_MODER1|GPIO_MODER_MODER2|GPIO_MODER_MODER3|GPIO_MODER_MODER4|GPIO_MODER_MODER5);
    GPIOC->MODER |= (GPIO_MODER_MODER0_0|GPIO_MODER_MODER1_0|GPIO_MODER_MODER2_0|GPIO_MODER_MODER3_0|GPIO_MODER_MODER4_0|GPIO_MODER_MODER5_0);

    //Botoes, GPIOA 0, 1, 4
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0|GPIO_PUPDR_PUPDR1|GPIO_PUPDR_PUPDR4);
    GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR0_1|GPIO_PUPDR_PUPDR1_1|GPIO_PUPDR_PUPDR4_1);
    GPIOA->MODER &= ~(GPIO_MODER_MODER0|GPIO_MODER_MODER1|GPIO_MODER_MODER4);

    // Botao PB0
    GPIOB->PUPDR &= (GPIO_PUPDR_PUPDR0);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR0_1);
    GPIOB->MODER &= ~(GPIO_MODER_MODER0);

    //Timers
    TIM9->PSC = PRESCALE_1MS;
    TIM10->PSC = PRESCALE_1MS;
    TIM11->PSC = PRESCALE_1MS;

    //Contagem geral
    TIM9->ARR = TIMER_1HZ;
    //Ponto selecao
    TIM10->ARR = TIMER_2HZ;
    //Debounce
    TIM11->ARR = TIMER_100HZ;

    //Interrupt
    NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
    NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 0);
    TIM9->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
    NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 2);
    TIM10->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
    NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 1);
    TIM11->DIER |= TIM_DIER_UIE;

    TIM9->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;
    TIM10->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;
    TIM11->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;
}

void correctDisp() {
    for (; currentDisp <= 5; currentDisp++) {
        switch (currentDisp) {
            case 5: {
                if (dispTimeArr[5] >= 3 ||
                    (dispTimeArr[5] >= 2 && (dispTimeArr[4] >= 4))
                ) {
                    dispTimeArr[5] = 0;
                    dispTimeArr[4] = 0;
                }
                break;
            }
            default: {
                if (dispTimeArr[currentDisp] >= (10 - (4 * (currentDisp & 1)))) {
                    dispTimeArr[currentDisp] = 0;
                    dispTimeArr[currentDisp + 1] += 1;
                }
                break;
            }
        }
    }
    currentDisp = 0;
    return;
}

void updateDisp() {
    for (; currentDisp <= 5; currentDisp++) {
        GPIOC->ODR |= 63;
        GPIOB->ODR &= ~dispArr[8];
        GPIOB->ODR |= dispArr[dispTimeArr[currentDisp]];
        if (currentDisp != dispSel) {
            GPIOC->ODR &= ~(1 << currentDisp);
        } else {
            if (pisca) {
                GPIOC->ODR &= ~(1 << currentDisp);
            } else {
                GPIOC->ODR |= (1 << currentDisp);
            }
        }
    }
    currentDisp = 0;
    return;
}

int debounce (int chave, int estado) {
    static uint8_t estados[4] = {0};
    if (estados[chave] == estado) {
        return 0;
    }
    estados[chave] = estado;
    return estado;
}

void TIM1_BRK_TIM9_IRQHandler() {
    TIM9->SR &= ~TIM_SR_UIF;
    if (!pause) {
        dispTimeArr[0] += 1;
    }
    return;
};

void TIM1_UP_TIM10_IRQHandler() {
     TIM10->SR &= ~TIM_SR_UIF;
     pisca = !pisca;
     return;
}

void TIM1_TRG_COM_TIM11_IRQHandler() {
    TIM11->SR &= ~TIM_SR_UIF;
    if (debounce(0, BUTTON_SELECT)) {
        dispSel++;
        if (dispSel >= 6) {
            dispSel = 0;
        }
        return;
    }
    if (debounce(1, BUTTON_MAIS)) {
        dispTimeArr[dispSel] += 1;
        if (!dispSel) { // garantir que unidade de segundo seja sempre 1seg
            TIM9->CNT = TIMER_1HZ;
        }
        return;
    }
    if (debounce(2, BUTTON_MENOS)) {
        dispTimeArr[dispSel] -= 1;
        if (dispTimeArr[dispSel] <= -1) {
            switch (dispSel) {
                case 5: {
                    dispTimeArr[5] = 2;
                    break;
                }
                case 4: {
                    dispTimeArr[4] = 9 - (6 * (dispTimeArr[5] == 2));
                    break;
                }
                default: {
                    dispTimeArr[dispSel] = 9 - (4 * (dispSel & 1));
                    break;
                }
            }
        }
        if (!dispSel) {
            TIM9->CNT = TIMER_1HZ;
        }
        return;
    }
    if (debounce(3, BUTTON_PAUSE)) {
        pause = !pause;
        return;
    }
}

int main() {
    setup();
    while(1) {
        correctDisp();
        updateDisp();
    }
};
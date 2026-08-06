/*
#include "../Headers/stm32f103xe.h"

#define OUTSIDE   (1 << 7)   // SW1, Port C
#define INSIDE    (1 << 6)   // SW0, Port C
#define LED_ENTER (1 << 7)   // PF7
#define LED_EXIT  (1 << 6)   // PF6

enum { IDLE, WAIT_INSIDE, WAIT_OUTSIDE };
#define TIMEOUT_MS 5000

void setup_timer(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 7999;     // 1 ms / tick
    TIM2->EGR |= 0x1;     // load prescaler + reset CNT
    TIM2->CR1 |= 0x1;     // free-running
}

int main(void) {
    int state = IDLE;
    int out_prev = 0, in_prev = 0;

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;   // RCC enable Port C — switches SW0/SW1
    RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;   // RCC enable  Port F — LED6/LED7
    GPIOC->CRL &= 0x00FFFFFF;   //  clear for PC6, PC7
    GPIOC->CRL |= 0x44000000;   //  PC6, PC7: 0100 0100
    GPIOF->CRL &= 0x00FFFFFF;   // clear for PF6, PF7
    GPIOF->CRL |= 0x22000000;   // PF6, PF7: 0010 0010
    GPIOF->BSRR = (LED_ENTER | LED_EXIT) << 16; // Make sure they start off

    setup_timer();

    while (1) {
        int sw = GPIOC->IDR; // Grab the value inside switch right now
        int out_now = (sw & OUTSIDE) ? 1 : 0; // mask for outside to check outside sensor
        int in_now  = (sw & INSIDE)  ? 1 : 0; // mask for inside to check inside sensor

        int out_edge = out_now && !out_prev;   // rising edge
        int in_edge  = in_now  && !in_prev; // rising edge
        out_prev = out_now; //set prev
        in_prev  = in_now; //set prev

        switch (state) {
        case IDLE:
            if (out_edge) {
                state = WAIT_INSIDE;  TIM2->EGR |= 0x1;
            }
            else if (in_edge) {
                state = WAIT_OUTSIDE; TIM2->EGR |= 0x1;
            }
            break;

        case WAIT_INSIDE:   // outside first -> expecting inside -> ENTER
            if (TIM2->CNT > TIMEOUT_MS) {
                state = IDLE;
            }
            else if (in_edge) {
                GPIOF->BSRR = LED_ENTER | (LED_EXIT << 16);
                state = IDLE;
            }
            break;

        case WAIT_OUTSIDE:  // inside first -> expecting outside -> EXIT
            if (TIM2->CNT > TIMEOUT_MS) state = IDLE;
            else if (out_edge) {
                GPIOF->BSRR = LED_EXIT | (LED_ENTER << 16);
                state = IDLE;
            }
            break;
        }
    }
} */
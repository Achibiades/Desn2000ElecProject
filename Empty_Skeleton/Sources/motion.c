#include "../Headers/stm32f103xe.h"
#include "../Headers/motion.h"

#define OUTSIDE   (1 << 7)   // SW1, Port C
#define INSIDE    (1 << 6)   // SW0, Port C
#define LED_ENTER (1 << 7)   // PF7
#define LED_EXIT  (1 << 6)   // PF6

enum { IDLE, WAIT_INSIDE, WAIT_OUTSIDE };
#define TIMEOUT_MS 5000

static void setup_timer(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 7999;
    TIM2->EGR |= 0x1;
    TIM2->CR1 |= 0x1;
}

void motion_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;
    GPIOC->CRL &= 0x00FFFFFF;
    GPIOC->CRL |= 0x44000000;
    GPIOF->CRL &= 0x00FFFFFF;
    GPIOF->CRL |= 0x22000000;
    GPIOF->BSRR = (LED_ENTER | LED_EXIT) << 16;
    setup_timer();
}

void motion_update(int *inside) {
    static int state = IDLE;
    static int out_prev = 0, in_prev = 0;

    int sw = GPIOC->IDR;
    int out_now = (sw & OUTSIDE) ? 1 : 0;
    int in_now  = (sw & INSIDE)  ? 1 : 0;

    int out_edge = out_now && !out_prev;
    int in_edge  = in_now  && !in_prev;
    out_prev = out_now;
    in_prev  = in_now;

    switch (state) {
    case IDLE:
        if (out_edge)      { state = WAIT_INSIDE;  TIM2->EGR |= 0x1; }
        else if (in_edge)  { state = WAIT_OUTSIDE; TIM2->EGR |= 0x1; }
        break;

    case WAIT_INSIDE:   // outside first -> inside -> ENTER
        if (TIM2->CNT > TIMEOUT_MS) state = IDLE;
        else if (in_edge) {
            GPIOF->BSRR = LED_ENTER | (LED_EXIT << 16);
            *inside = 1;
            state = IDLE;
        }
        break;

    case WAIT_OUTSIDE:  // inside first -> outside -> EXIT
        if (TIM2->CNT > TIMEOUT_MS) state = IDLE;
        else if (out_edge) {
            GPIOF->BSRR = LED_EXIT | (LED_ENTER << 16);
            *inside = 0;
            state = IDLE;
        }
        break;
    }
}
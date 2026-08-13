#include "../Headers/stm32f103xe.h"
#include "../Headers/motion.h"

#define OUTSIDE   (1 << 7)   // SW1, Port C (PC7)
#define INSIDE    (1 << 6)   // SW0, Port C (PC6)

// RGB0 on GPIOA[8:10]
#define RGB_BLUE   (1 << 8)   // PA8
#define RGB_RED    (1 << 9)   // PA9
#define RGB_GREEN  (1 << 10)  // PA10

enum { IDLE, WAIT_INSIDE, WAIT_OUTSIDE };
#define TIMEOUT_MS 5000

static void setup_timer(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 7999;
    TIM2->EGR |= 0x1;
    TIM2->CR1 |= 0x1;
}

// Drive the RGB LED: green for entered, red for exited.
// BSRR sets pins in the low half, clears them in the high half (<< 16),
// so one write turns the wanted colour on and the others off atomically.
static void rgb_set_green(void) {
    GPIOA->BSRR = RGB_GREEN | ((RGB_RED | RGB_BLUE) << 16);
}
static void rgb_set_red(void) {
    GPIOA->BSRR = RGB_RED | ((RGB_GREEN | RGB_BLUE) << 16);
}
static void rgb_off(void) {
    GPIOA->BSRR = (RGB_RED | RGB_GREEN | RGB_BLUE) << 16;
}

void motion_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   // Port A for the RGB LED

    GPIOC->CRL &= 0x00FFFFFF;   // PC6, PC7 as inputs (switches)
    GPIOC->CRL |= 0x44000000;

    // PA8, PA9, PA10 are in CRH (pins 8-15). Configure just those three
    // nibbles as output push-pull 2 MHz (0010), leaving the rest of CRH alone.
    GPIOA->CRH &= ~(0x00000FFF);   // clear PA8, PA9, PA10 nibbles
    GPIOA->CRH |=  0x00000222;     // PA8/9/10: output push-pull 2 MHz

    rgb_set_red();
    setup_timer();
}

int motion_update(int *inside) {
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
            rgb_set_green();        // entered -> green
            *inside = 1;
            state = IDLE;
            return MOTION_ENTER;
        }
        break;

    case WAIT_OUTSIDE:  // inside first -> outside -> EXIT
        if (TIM2->CNT > TIMEOUT_MS) state = IDLE;
        else if (out_edge) {
            rgb_set_red();          // exited -> red
            *inside = 0;
            state = IDLE;
            return MOTION_EXIT;
        }
        break;
    }

    return MOTION_NONE;
}

void motion_mark_inside(void) {
    rgb_set_green();
}
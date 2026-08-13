#include "../Headers/stm32f103xe.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"
#include <stdio.h>

#define COFFEE_AUTO   0
#define COFFEE_MANUAL 1
#define COFFEE_OFF    2

static int coffee_mode = COFFEE_AUTO;


void coffee_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;

    GPIOF->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
    GPIOF->CRL |= GPIO_CRL_MODE1_1;

    GPIOF->BSRR = GPIO_BSRR_BR1;
}


void draw_coffee_screen(int hour, int minute)
{
    char time_str[10];

    lcd_fillScreen(WHITE);

    lcd_fillRect(10, 10, 70, 40, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(16, 20, (unsigned char *)"return");

    lcd_fontColor(BLACK, WHITE);
    lcd_putString_scaled(40, 55,
                         (unsigned char *)"COFFEE TIME", 2);

    lcd_fillRect(30, 90, 210, 150, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);

    if (coffee_mode == COFFEE_AUTO)
    {
        lcd_putString_scaled(
            55, 108,
            (unsigned char *)"SUNRISE",
            2
        );
    }
    else if (coffee_mode == COFFEE_OFF)
    {
        lcd_putString_scaled(
            85, 105,
            (unsigned char *)"OFF",
            3
        );
    }
    else
    {
        sprintf(time_str, "%02d : %02d", hour, minute);

        lcd_putString_scaled(
            36, 104,
            (unsigned char *)time_str,
            4
        );
    }

    lcd_fontColor(BLACK, WHITE);
    lcd_putString(40, 160, (unsigned char *)"HOUR");
    lcd_putString(165, 160, (unsigned char *)"MIN");

    lcd_fillRect(20, 170, 105, 220, LIGHT_GRAY);
    lcd_fillRect(20, 230, 105, 280, LIGHT_GRAY);

    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString_scaled(57, 187, (unsigned char *)"+", 2);
    lcd_putString_scaled(57, 247, (unsigned char *)"-", 2);

    lcd_fillRect(135, 170, 220, 220, LIGHT_GRAY);
    lcd_fillRect(135, 230, 220, 280, LIGHT_GRAY);

    lcd_putString_scaled(172, 187, (unsigned char *)"+", 2);
    lcd_putString_scaled(172, 247, (unsigned char *)"-", 2);

    if (coffee_mode == COFFEE_OFF)
    {
        lcd_fillRect(20, 290, 220, 315, RED);
        lcd_fontColor(BLACK, RED);
        lcd_putString(75, 299,
                      (unsigned char *)"COFFEE OFF");
    }
    else
    {
        lcd_fillRect(20, 290, 220, 315, GREEN);
        lcd_fontColor(BLACK, GREEN);
        lcd_putString(78, 299,
                      (unsigned char *)"COFFEE ON");
    }
}


void handle_coffee_touch(
    int tx,
    int ty,
    int *hour,
    int *minute
)
{
    if (touch_rect(20, 170, 105, 220, tx, ty))
    {
        change_hour(hour, 1);
        coffee_mode = COFFEE_MANUAL;
    }

    else if (touch_rect(20, 230, 105, 280, tx, ty))
    {
        change_hour(hour, 0);
        coffee_mode = COFFEE_MANUAL;
    }

    else if (touch_rect(135, 170, 220, 220, tx, ty))
    {
        change_minute(minute, 1);
        coffee_mode = COFFEE_MANUAL;
    }

    else if (touch_rect(135, 230, 220, 280, tx, ty))
    {
        change_minute(minute, 0);
        coffee_mode = COFFEE_MANUAL;
    }

    else if (touch_rect(20, 290, 220, 315, tx, ty))
    {
        if (coffee_mode == COFFEE_OFF)
            coffee_mode = COFFEE_AUTO;
        else
            coffee_mode = COFFEE_OFF;
    }

    draw_coffee_screen(*hour, *minute);
}


void check_coffee(
    char *time_str,
    int hour,
    int minute,
    int sunrise_detected
)
{
    if (coffee_mode == COFFEE_OFF)
    {
        GPIOF->BSRR = GPIO_BSRR_BR1;
    }

    else if (coffee_mode == COFFEE_MANUAL)
    {
        if (check_time(time_str, hour, minute))
            GPIOF->BSRR = GPIO_BSRR_BS1;
        else
            GPIOF->BSRR = GPIO_BSRR_BR1;
    }

    else
    {
        if (wakeup_sunrise)
            GPIOF->BSRR = GPIO_BSRR_BS1;
        else
            GPIOF->BSRR = GPIO_BSRR_BR1;
    }
}

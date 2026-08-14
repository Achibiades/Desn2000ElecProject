#include "../Headers/stm32f103xe.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/SystemInit.h"
#include "../Headers/draw_screens.h"
#include <stdio.h>

#define COFFEE_RUN_TIME 1800

static int coffee_enable = 1;
static int coffee_running = 0;
static unsigned int coffee_start_time = 0;


static void draw_coffee_time(int hour, int minute, int wakeup_sunrise)
{
    char time_str[6];

    lcd_fillRect(30, 90, 210, 150, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);

    if (wakeup_sunrise)
    {
        lcd_putString_scaled(
            43,
            108,
            (unsigned char *)"SUNRISE MODE",
            2
        );
    }
    else
    {
        snprintf(time_str, 6, "%02d:%02d", hour, minute);

        lcd_putString_scaled(
            60,
            104,
            (unsigned char *)time_str,
            4
        );
    }
}


static void draw_coffee_state(void)
{
    lcd_fillRect(205, 15, 235, 30, WHITE);
    lcd_fontColor(BLACK, WHITE);

    if (coffee_enable)
    {
        lcd_putString(
            205,
            21,
            (unsigned char *)"ON"
        );
    }
    else
    {
        lcd_putString(
            205,
            21,
            (unsigned char *)"OFF"
        );
    }
}


void coffee_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;

    GPIOF->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
    GPIOF->CRL |= GPIO_CRL_MODE1_1;

    GPIOF->BSRR = GPIO_BSRR_BR1;
}


void draw_coffee_screen(int hour, int minute, int wakeup_sunrise)
{
    lcd_fillScreen(WHITE);

    /* Return button */
    lcd_fillRect(10, 10, 70, 40, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(16, 21, (unsigned char *)"return");

    /* Coffee state */
    lcd_fontColor(BLACK, WHITE);
    lcd_putString(
        110,
        21,
        (unsigned char *)"Coffee State:"
    );

    draw_coffee_state();

    /* Title */
    lcd_putString_scaled(
        60,
        55,
        (unsigned char *)"COFFEE TIME",
        2
    );

    /* Time / sunrise display */
    draw_coffee_time(
        hour,
        minute,
        wakeup_sunrise
    );

    /* Labels */
    lcd_fontColor(BLACK, WHITE);

    lcd_putString(
        49,
        161,
        (unsigned char *)"HOUR"
    );

    lcd_putString(
        163,
        161,
        (unsigned char *)"MIN"
    );

    /* Hour + / - */
    lcd_fillRect(20, 180, 60, 220, LIGHT_GRAY);
    lcd_fillRect(70, 180, 110, 220, LIGHT_GRAY);

    lcd_fontColor(BLACK, LIGHT_GRAY);

    lcd_putString_scaled(
        32,
        192,
        (unsigned char *)"+",
        2
    );

    lcd_putString_scaled(
        82,
        192,
        (unsigned char *)"-",
        2
    );

    /* Minute + / - */
    lcd_fillRect(130, 180, 170, 220, LIGHT_GRAY);
    lcd_fillRect(180, 180, 220, 220, LIGHT_GRAY);

    lcd_putString_scaled(
        142,
        192,
        (unsigned char *)"+",
        2
    );

    lcd_putString_scaled(
        192,
        192,
        (unsigned char *)"-",
        2
    );

    /* Sunrise mode */
    lcd_fillRect(20, 235, 220, 260, LIGHT_GRAY);

    lcd_putString(
        44,
        243,
        (unsigned char *)"WAKE UP TO SUNRISE"
    );

    /* Coffee ON */
    lcd_fillRect(20, 275, 110, 305, LIGHT_GRAY);

    lcd_putString(
        33,
        285,
        (unsigned char *)"Coffee ON"
    );

    /* Coffee OFF */
    lcd_fillRect(130, 275, 220, 305, LIGHT_GRAY);

    lcd_putString(
        139,
        285,
        (unsigned char *)"Coffee OFF"
    );
}


void handle_coffee_touch(int tx, int ty,
                         int *hour,
                         int *minute,
                         int *wakeup_sunrise)
{
    /* Hour + */
    if (touch_rect(20, 180, 60, 220, tx, ty))
    {
        *wakeup_sunrise = 0;

        change_hour(hour, 1);

        draw_coffee_time(
            *hour,
            *minute,
            *wakeup_sunrise
        );
    }

    /* Hour - */
    else if (touch_rect(70, 180, 110, 220, tx, ty))
    {
        *wakeup_sunrise = 0;

        change_hour(hour, 0);

        draw_coffee_time(
            *hour,
            *minute,
            *wakeup_sunrise
        );
    }

    /* Minute + */
    else if (touch_rect(130, 180, 170, 220, tx, ty))
    {
        *wakeup_sunrise = 0;

        change_minute(minute, 1);

        draw_coffee_time(
            *hour,
            *minute,
            *wakeup_sunrise
        );
    }

    /* Minute - */
    else if (touch_rect(180, 180, 220, 220, tx, ty))
    {
        *wakeup_sunrise = 0;

        change_minute(minute, 0);

        draw_coffee_time(
            *hour,
            *minute,
            *wakeup_sunrise
        );
    }

    /* Sunrise mode */
    else if (touch_rect(20, 235, 220, 260, tx, ty))
    {
        *wakeup_sunrise = 1;

        draw_coffee_time(
            *hour,
            *minute,
            *wakeup_sunrise
        );
    }

    /* Coffee ON */
    else if (touch_rect(20, 275, 110, 305, tx, ty))
    {
        coffee_enable = 1;

        draw_coffee_state();
    }

    /* Coffee OFF */
    else if (touch_rect(130, 275, 220, 305, tx, ty))
    {
        coffee_enable = 0;
        coffee_running = 0;

        GPIOF->BSRR = GPIO_BSRR_BR1;

        draw_coffee_state();
    }
}


void check_coffee(char *time_str,
                  int hour,
                  int minute,
                  int wakeup_sunrise,
                  unsigned int now)
{
    if (!coffee_enable)
    {
        coffee_running = 0;
        GPIOF->BSRR = GPIO_BSRR_BR1;
        return;
    }

    if (coffee_running)
    {
        if ((now - coffee_start_time) >= COFFEE_RUN_TIME)
        {
            coffee_running = 0;
            GPIOF->BSRR = GPIO_BSRR_BR1;
        }
        else
        {
            GPIOF->BSRR = GPIO_BSRR_BS1;
        }

        return;
    }

    if (wakeup_sunrise == 0)
    {
        if (check_time(time_str, hour, minute))
        {
            coffee_running = 1;
            coffee_start_time = now;

            GPIOF->BSRR = GPIO_BSRR_BS1;
        }
        else
        {
            GPIOF->BSRR = GPIO_BSRR_BR1;
        }
    }
    else
    {
        GPIOF->BSRR = GPIO_BSRR_BR1;
    }
}

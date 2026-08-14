#include "../Headers/stm32f103xe.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"
#include <stdio.h>

#define COFFEE_RUN_TIME 1800

static int coffee_enable = 1;
static int coffee_running = 0;
static unsigned int coffee_start_time = 0;


/* Redraw only the time box */
static void draw_coffee_time(int hour, int minute, int wakeup_sunrise)
{
    char time_str[10];

    lcd_fillRect(30, 90, 210, 150, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);

    if (wakeup_sunrise)
    {
        lcd_putString_scaled(
            55,
            108,
            (unsigned char *)"SUNRISE",
            2
        );
    }
    else
    {
        sprintf(time_str, "%02d : %02d", hour, minute);

        lcd_putString_scaled(
            36,
            104,
            (unsigned char *)time_str,
            4
        );
    }
}


/* Redraw only the sunrise button */
static void draw_sunrise_button(int wakeup_sunrise)
{
    if (wakeup_sunrise)
    {
        lcd_fillRect(150, 10, 230, 40, GREEN);
        lcd_fontColor(BLACK, GREEN);
    }
    else
    {
        lcd_fillRect(150, 10, 230, 40, LIGHT_GRAY);
        lcd_fontColor(BLACK, LIGHT_GRAY);
    }

    lcd_putString(
        166,
        20,
        (unsigned char *)"SUNRISE"
    );
}


/* Redraw only the Coffee ON/OFF button */
static void draw_coffee_button(void)
{
    if (coffee_enable)
    {
        lcd_fillRect(20, 290, 220, 315, GREEN);
        lcd_fontColor(BLACK, GREEN);

        lcd_putString(
            75,
            299,
            (unsigned char *)"COFFEE ON"
        );
    }
    else
    {
        lcd_fillRect(20, 290, 220, 315, RED);
        lcd_fontColor(BLACK, RED);

        lcd_putString(
            72,
            299,
            (unsigned char *)"COFFEE OFF"
        );
    }
}


/* LED1 = PF1 */
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

    /* Return */
    lcd_fillRect(10, 10, 70, 40, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(16, 20, (unsigned char *)"return");

    /* Sunrise mode button */
    draw_sunrise_button(wakeup_sunrise);

    /* Title */
    lcd_fontColor(BLACK, WHITE);

    lcd_putString_scaled(
        40,
        55,
        (unsigned char *)"COFFEE TIME",
        2
    );

    /* Time */
    draw_coffee_time(
        hour,
        minute,
        wakeup_sunrise
    );

    /* Labels */
    lcd_fontColor(BLACK, WHITE);

    lcd_putString(
        40,
        160,
        (unsigned char *)"HOUR"
    );

    lcd_putString(
        165,
        160,
        (unsigned char *)"MIN"
    );

    /* Hour + / - */
    lcd_fillRect(20, 170, 105, 220, LIGHT_GRAY);
    lcd_fillRect(20, 230, 105, 280, LIGHT_GRAY);

    lcd_fontColor(BLACK, LIGHT_GRAY);

    lcd_putString_scaled(
        57,
        187,
        (unsigned char *)"+",
        2
    );

    lcd_putString_scaled(
        57,
        247,
        (unsigned char *)"-",
        2
    );

    /* Minute + / - */
    lcd_fillRect(135, 170, 220, 220, LIGHT_GRAY);
    lcd_fillRect(135, 230, 220, 280, LIGHT_GRAY);

    lcd_putString_scaled(
        172,
        187,
        (unsigned char *)"+",
        2
    );

    lcd_putString_scaled(
        172,
        247,
        (unsigned char *)"-",
        2
    );

    draw_coffee_button();
}


void handle_coffee_touch(int tx, int ty,
                         int *hour, int *minute,
                         int *wakeup_sunrise)
{
    /* Sunrise mode */
    if (touch_rect(150, 10, 230, 40, tx, ty))
    {
        *wakeup_sunrise = !(*wakeup_sunrise);

        draw_sunrise_button(
            *wakeup_sunrise
        );

        draw_coffee_time(
            *hour,
            *minute,
            *wakeup_sunrise
        );
    }

    /* Hour + */
    else if (touch_rect(20, 170, 105, 220, tx, ty))
    {
        change_hour(hour, 1);

        *wakeup_sunrise = 0;

        draw_sunrise_button(
            *wakeup_sunrise
        );

        draw_coffee_time(
            *hour,
            *minute,
            *wakeup_sunrise
        );
    }

    /* Hour - */
    else if (touch_rect(20, 230, 105, 280, tx, ty))
    {
        change_hour(hour, 0);

        *wakeup_sunrise = 0;

        draw_sunrise_button(
            *wakeup_sunrise
        );

        draw_coffee_time(
            *hour,
            *minute,
            *wakeup_sunrise
        );
    }

    /* Minute + */
    else if (touch_rect(135, 170, 220, 220, tx, ty))
    {
        change_minute(minute, 1);

        *wakeup_sunrise = 0;

        draw_sunrise_button(
            *wakeup_sunrise
        );

        draw_coffee_time(
            *hour,
            *minute,
            *wakeup_sunrise
        );
    }

    /* Minute - */
    else if (touch_rect(135, 230, 220, 280, tx, ty))
    {
        change_minute(minute, 0);

        *wakeup_sunrise = 0;

        draw_sunrise_button(
            *wakeup_sunrise
        );

        draw_coffee_time(
            *hour,
            *minute,
            *wakeup_sunrise
        );
    }

    /* Coffee ON/OFF */
    else if (touch_rect(20, 290, 220, 315, tx, ty))
    {
        coffee_enable = !coffee_enable;

        if (!coffee_enable)
        {
            coffee_running = 0;
            GPIOF->BSRR = GPIO_BSRR_BR1;
        }

        draw_coffee_button();
    }
}


void check_coffee(char *time_str,
                  int hour,
                  int minute,
                  int wakeup_sunrise,
                  unsigned int now)
{
    /* Coffee disabled */
    if (!coffee_enable)
    {
        coffee_running = 0;
        GPIOF->BSRR = GPIO_BSRR_BR1;
        return;
    }

    /* Coffee currently running */
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

    /* Manual wake-up time */
    if (!wakeup_sunrise)
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

    /* Sunrise mode */
    else
    {
        /*
         * TODO:
         * Actual sunlight detection will go here.
         *
         * When sunrise is detected:
         *
         * coffee_running = 1;
         * coffee_start_time = now;
         * GPIOF->BSRR = GPIO_BSRR_BS1;
         */

        GPIOF->BSRR = GPIO_BSRR_BR1;
    }
}

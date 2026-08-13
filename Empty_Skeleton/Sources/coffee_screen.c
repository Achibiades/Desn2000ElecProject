#include "../Headers/stm32f103xe.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"
#include <stdio.h>

static int coffee_enable = 1;


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
    char time_str[10];

    lcd_fillScreen(WHITE);

    /* Return button */
    lcd_fillRect(10, 10, 70, 40, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(16, 20, (unsigned char *)"return");

    /* Title */
    lcd_fontColor(BLACK, WHITE);
    lcd_putString_scaled(40, 55,
                         (unsigned char *)"COFFEE TIME", 2);

    /* Time display */
    lcd_fillRect(30, 90, 210, 150, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);

    if (wakeup_sunrise)
    {
        lcd_putString_scaled(
            55, 108,
            (unsigned char *)"SUNRISE",
            2
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

    /* Labels */
    lcd_fontColor(BLACK, WHITE);
    lcd_putString(40, 160, (unsigned char *)"HOUR");
    lcd_putString(165, 160, (unsigned char *)"MIN");

    /* Hour +/- */
    lcd_fillRect(20, 170, 105, 220, LIGHT_GRAY);
    lcd_fillRect(20, 230, 105, 280, LIGHT_GRAY);

    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString_scaled(57, 187, (unsigned char *)"+", 2);
    lcd_putString_scaled(57, 247, (unsigned char *)"-", 2);

    /* Minute +/- */
    lcd_fillRect(135, 170, 220, 220, LIGHT_GRAY);
    lcd_fillRect(135, 230, 220, 280, LIGHT_GRAY);

    lcd_putString_scaled(172, 187, (unsigned char *)"+", 2);
    lcd_putString_scaled(172, 247, (unsigned char *)"-", 2);

    /* Coffee ON/OFF */
    if (coffee_enable)
    {
        lcd_fillRect(20, 290, 220, 315, GREEN);
        lcd_fontColor(BLACK, GREEN);
        lcd_putString(75, 299,
                      (unsigned char *)"COFFEE ON");
    }
    else
    {
        lcd_fillRect(20, 290, 220, 315, RED);
        lcd_fontColor(BLACK, RED);
        lcd_putString(72, 299,
                      (unsigned char *)"COFFEE OFF");
    }
}


void handle_coffee_touch(int tx, int ty,
                         int *hour, int *minute,
                         int *wakeup_sunrise)
{
    if (touch_rect(20, 170, 105, 220, tx, ty))
    {
        change_hour(hour, 1);
        *wakeup_sunrise = 0;
    }

    else if (touch_rect(20, 230, 105, 280, tx, ty))
    {
        change_hour(hour, 0);
        *wakeup_sunrise = 0;
    }

    else if (touch_rect(135, 170, 220, 220, tx, ty))
    {
        change_minute(minute, 1);
        *wakeup_sunrise = 0;
    }

    else if (touch_rect(135, 230, 220, 280, tx, ty))
    {
        change_minute(minute, 0);
        *wakeup_sunrise = 0;
    }

    else if (touch_rect(20, 290, 220, 315, tx, ty))
    {
        coffee_enable = !coffee_enable;

        if (!coffee_enable)
            GPIOF->BSRR = GPIO_BSRR_BR1;
    }

    draw_coffee_screen(
        *hour,
        *minute,
        *wakeup_sunrise
    );
}


void check_coffee(char *time_str,
                  int hour,
                  int minute,
                  int wakeup_sunrise)
{
    if (!coffee_enable)
    {
        GPIOF->BSRR = GPIO_BSRR_BR1;
        return;
    }

    if (!wakeup_sunrise)
    {
        if (check_time(time_str, hour, minute))
            GPIOF->BSRR = GPIO_BSRR_BS1;
        else
            GPIOF->BSRR = GPIO_BSRR_BR1;
    }
    else
    {
        /*
         * TODO:
         * Add sunlight / sunrise detection here later.
         */

        GPIOF->BSRR = GPIO_BSRR_BR1;
    }
}

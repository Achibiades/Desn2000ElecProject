#include "../Headers/stm32f103xe.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/SystemInit.h"
#include "../Headers/draw_screens.h"
#include "../Headers/rtc.h"
#include <stdio.h>
#include <string.h>

#define COFFEE_RUN_TIME 1800

static int coffee_enable = 1;
static int coffee_running = 0;
static unsigned int coffee_start_time = 0;



static const char coffee_sunrise[][6] = {
  "05:50","05:51","05:52","05:53","05:54","05:54","05:55","05:56","05:57","05:58",
  "05:59","06:00","06:01","06:02","06:03","06:04","06:05","06:06","06:07","06:08",
  "06:09","06:10","06:11","06:12","06:14","06:15","06:16","06:17","06:18","06:19",
  "06:20",

  "06:21","06:22","06:23","06:24","06:25","06:27","06:28","06:29","06:30","06:31",
  "06:32","06:33","06:34","06:35","06:36","06:37","06:38","06:39","06:40","06:41",
  "06:42","06:43","06:44","06:45","06:46","06:47","06:48","06:49",

  "06:50","06:51","06:52","06:53","06:54","06:55","06:56","06:57","06:58","06:59",
  "07:00","07:00","07:01","07:02","07:03","07:04","07:05","07:06","07:07","07:08",
  "07:08","07:09","07:10","07:11","07:11","07:12","07:13","07:14","07:15","07:15",
  "07:16",

  "07:18","07:19","07:20","07:20","06:21","06:22","06:23","06:24","06:25","06:26",
  "06:26","06:27","06:28","06:29","06:30","06:31","06:31","06:32","06:33","06:34",
  "06:35","06:36","06:37","06:37","06:38","06:39","06:40","06:41","06:42","06:42",

  "06:43","06:44","06:45","06:46","06:47","06:47","06:48","06:49","06:50","06:51",
  "06:52","06:52","06:53","06:54","06:55","06:56","06:56","06:57","06:58","06:59",
  "07:00","07:00","07:01","07:02","07:03","07:03","07:04","07:05","07:05","07:06",
  "07:07",

  "07:08","07:08","07:09","07:09","07:10","07:11","07:11","07:12","07:12","07:13",
  "07:13","07:14","07:14","07:14","07:15","07:15","07:15","07:16","07:16","07:17",
  "07:17","07:17","07:17","07:17","07:17","07:17","07:17","07:18","07:18","07:18",

  "07:18","07:17","07:17","07:17","07:17","07:17","07:17","07:17","07:16","07:16",
  "07:16","07:15","07:15","07:15","07:14","07:14","07:13","07:13","07:12","07:12",
  "07:11","07:10","07:10","07:09","07:08","07:08","07:07","07:06","07:05","07:04",
  "07:04",

  "07:03","07:02","07:01","07:00","06:59","06:58","06:57","06:56","06:55","06:54",
  "06:53","06:52","06:51","06:49","06:48","06:47","06:46","06:45","06:44","06:43",
  "06:42","06:41","06:40","06:38","06:37","06:36","06:34","06:33","06:32","06:30",
  "06:27",

  "06:26","06:25","06:23","06:22","06:20","06:19","06:18","06:16","06:15","06:14",
  "06:12","06:11","06:09","06:07","06:06","06:04","06:03","06:02","06:00","05:58",
  "05:57","05:55","05:54","05:52","05:51","05:49","05:48","05:46","05:45","05:43",

  "05:42","05:40","05:39","06:37","06:36","06:34","06:33","06:31","06:30","06:28",
  "06:27","06:26","06:24","06:23","06:21","06:20","06:19","06:17","06:16","06:15",
  "06:14","06:12","06:11","06:10","06:09","06:07","06:06","06:05","06:04","06:03",
  "06:02",

  "06:01","06:00","05:59","05:58","05:57","05:56","05:55","05:54","05:53","05:52",
  "05:51","05:51","05:50","05:49","05:48","05:48","05:47","05:46","05:46","05:45",
  "05:45","05:44","05:43","05:43","05:42","05:42","05:41","05:41","05:41","05:41",

  "05:41","05:40","05:40","05:40","05:40","05:40","05:40","05:40","05:40","05:40",
  "05:40","05:41","05:41","05:41","05:41","05:42","05:42","05:42","05:43","05:43",
  "05:44","05:44","05:44","05:45","05:45","05:46","05:46","05:47","05:47","05:48",
  "05:48"
};



static void draw_coffee_time(int hour, int minute, int wakeup_sunrise)
{
    char time_str[6];

    lcd_fillRect(30, 90, 210, 150, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);

    if (wakeup_sunrise)
    {
        lcd_putString_scaled(43, 108, (unsigned char *)"SUNRISE MODE", 2);
    }
    else
    {
        snprintf(time_str, 6, "%02d:%02d", hour, minute);
        lcd_putString_scaled(60, 104, (unsigned char *)time_str, 4);
      
    }
}


static void draw_coffee_state(void)
{
    lcd_fillRect(205, 15, 235, 30, WHITE);
    lcd_fontColor(BLACK, WHITE);

    if (coffee_enable)
    {
        lcd_putString(205, 21, (unsigned char *)"ON");
    }
    else
    {
        lcd_putString(205, 21, (unsigned char *)"OFF");
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

    //Return button 
    lcd_fillRect(10, 10, 70, 40, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(16, 21, (unsigned char *)"return");

    //Coffee state
    lcd_fontColor(BLACK, WHITE);
    lcd_putString(110, 21, (unsigned char *)"Coffee State:");

    draw_coffee_state();

    //Title 
    lcd_putString_scaled(60, 55, (unsigned char *)"COFFEE TIME", 2);

    //Time / sunrise display 
    draw_coffee_time(hour, minute, wakeup_sunrise);

    // Labels 
    lcd_fontColor(BLACK, WHITE);
    lcd_putString(49, 161, (unsigned char *)"HOUR");
    lcd_putString(163, 161, (unsigned char *)"MIN");

    // Hour + / - 
    lcd_fillRect(20, 180, 60, 220, LIGHT_GRAY);
    lcd_fillRect(70, 180, 110, 220, LIGHT_GRAY);

    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString_scaled(32, 192, (unsigned char *)"+", 2);

    lcd_putString_scaled(82, 192, (unsigned char *)"-", 2);

    // Minute + / - 
    lcd_fillRect(130, 180, 170, 220, LIGHT_GRAY);
    lcd_fillRect(180, 180, 220, 220, LIGHT_GRAY);

    lcd_putString_scaled(142, 192, (unsigned char *)"+", 2);

    lcd_putString_scaled( 192, 192, (unsigned char *)"-", 2);

    // Sunrise mode 
    lcd_fillRect(20, 235, 220, 260, LIGHT_GRAY);

    lcd_putString(44, 243, (unsigned char *)"WAKE UP TO SUNRISE");

    // Coffee ON 
    lcd_fillRect(20, 275, 110, 305, LIGHT_GRAY);

    lcd_putString(33, 285, (unsigned char *)"Coffee ON");

    // Coffee OFF 
    lcd_fillRect(130, 275, 220, 305, LIGHT_GRAY);

    lcd_putString(139, 285, (unsigned char *)"Coffee OFF");
}


void handle_coffee_touch(int tx, int ty, int *hour, int *minute, int *wakeup_sunrise)
{
    // Hour + 
    if (touch_rect(20, 180, 60, 220, tx, ty)) {
        *wakeup_sunrise = 0;
        change_hour(hour, 1);
        draw_coffee_time(*hour, *minute, *wakeup_sunrise);
    }

    // Hour - 
    else if (touch_rect(70, 180, 110, 220, tx, ty)) 
    {
        *wakeup_sunrise = 0;
        change_hour(hour, 0);
        draw_coffee_time(*hour, *minute, *wakeup_sunrise);
    }

    // Minute + 
    else if (touch_rect(130, 180, 170, 220, tx, ty))
    {
        *wakeup_sunrise = 0;
        change_minute(minute, 1);
        draw_coffee_time(*hour, *minute, *wakeup_sunrise);
    }

    // Minute - 
    else if (touch_rect(180, 180, 220, 220, tx, ty))
    {
        *wakeup_sunrise = 0;
        change_minute(minute, 0);
        draw_coffee_time(*hour, *minute, *wakeup_sunrise);
    }

    // Sunrise mode ON/OFF 
    else if (touch_rect(20, 235, 220, 260, tx, ty))
    {
        if (*wakeup_sunrise == 0)
        {
            *wakeup_sunrise = 1;
        }
        else
        {
            *wakeup_sunrise = 0;
        }

        draw_coffee_time(*hour, *minute, *wakeup_sunrise);
    }

    // Coffee ON 
    else if (touch_rect(20, 275, 110, 305, tx, ty))
    {
        coffee_enable = 1;
        draw_coffee_state();
    }

    // Coffee OFF 
    else if (touch_rect(130, 275, 220, 305, tx, ty))
    {
        coffee_enable = 0;
        coffee_running = 0;
        GPIOF->BSRR = GPIO_BSRR_BR1;
        draw_coffee_state();
    }
}


void check_coffee(char *time_str, char *date_str, int hour, int minute, int wakeup_sunrise, unsigned int now)
{
    unsigned int day_of_year;
    unsigned int sunrise_index;
    unsigned int sunrise_count;

    // Coffee completely disabled 
    if (!coffee_enable)
    {
        coffee_running = 0;
        GPIOF->BSRR = GPIO_BSRR_BR1;
        return;
    }

    // Coffee already running 
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

    // Manual wake-up mode 
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

    // Sunrise mode 
    else
    {
        day_of_year = day_of_year_from_str(date_str);
        sunrise_count = sizeof(coffee_sunrise) / sizeof(coffee_sunrise[0]);

        if (day_of_year >= 1 && day_of_year <= sunrise_count)
        {
            sunrise_index = day_of_year - 1;
            if (!strcmp(time_str, coffee_sunrise[sunrise_index]))
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
            // Invalid date index: keep the coffee machine off 
            GPIOF->BSRR = GPIO_BSRR_BR1;
        }
    }
}

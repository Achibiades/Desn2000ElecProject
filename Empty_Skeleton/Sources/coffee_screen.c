#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"
#include <stdio.h>
#include "../Headers/SystemInit.h"

/* Coffee mode state */
typedef enum {
    COFFEE_MANUAL = 0,   /* uses coffee_hour / coffee_min set below */
    COFFEE_SYNC_WAKEUP,  /* ignores manual time, uses wake up time */
    COFFEE_OFF            /* no coffee at all */
} coffee_mode_t;

static coffee_mode_t coffee_mode = COFFEE_MANUAL;
static int coffee_hour = 7;
static int coffee_min  = 0;

void draw_coffee_screen() {
    lcd_fillScreen(WHITE);

    /* Return button */
    lcd_fillRect(10, 10, 70, 40, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(16, 20, (unsigned char *)"return");

    /* Title -- 2x scale */
    lcd_fontColor(BLACK, WHITE);
    lcd_putString_scaled(40, 55, (unsigned char *)"COFFEE TIME", 2);

    /* Time readout -- 4x scale, grayed if not in manual mode */
    lcd_fillRect(30, 90, 210, 150, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    {
        char time_str[8];
        sprintf(time_str, "%02d : %02d", coffee_hour, coffee_min);
        lcd_putString_scaled(36, 104, (unsigned char *)time_str, 4);
    }

    /* Column labels */
    lcd_fontColor(BLACK, WHITE);
    lcd_putString(40, 160, (unsigned char *)"HOUR");
    lcd_putString(165, 160, (unsigned char *)"MIN");

    /* Hour +/- tiles */
    lcd_fillRect(20, 170, 105, 220, LIGHT_GRAY);
    lcd_fillRect(20, 230, 105, 280, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString_scaled(57, 187, (unsigned char *)"+", 2);
    lcd_putString_scaled(57, 247, (unsigned char *)"-", 2);

    /* Minute +/- tiles */
    lcd_fillRect(135, 170, 220, 220, LIGHT_GRAY);
    lcd_fillRect(135, 230, 220, 280, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString_scaled(172, 187, (unsigned char *)"+", 2);
    lcd_putString_scaled(172, 247, (unsigned char *)"-", 2);

    /* Sync with wake up time toggle */
    lcd_fillRect(20, 290, 220, 330, (coffee_mode == COFFEE_SYNC_WAKEUP) ? RED : LIGHT_GRAY);
    lcd_fontColor(BLACK, (coffee_mode == COFFEE_SYNC_WAKEUP) ? RED : LIGHT_GRAY);
    lcd_putString(43, 305, (unsigned char *)"SYNC WITH WAKE UP");

    /* No coffee toggle */
    lcd_fillRect(20, 340, 220, 380, (coffee_mode == COFFEE_OFF) ? RED : LIGHT_GRAY);
    lcd_fontColor(BLACK, (coffee_mode == COFFEE_OFF) ? RED : LIGHT_GRAY);
    lcd_putString(70, 355, (unsigned char *)"NO COFFEE TODAY");
}

void handle_coffee_touch(int tx, int ty)
{
    /* Return button */
    if (touch_rect(10, 10, 70, 40, tx, ty)) {
        /* curr_ui = previous screen; */
        return;
    }

    /* Hour + */
    else if (touch_rect(20, 170, 105, 220, tx, ty)) {
        coffee_mode = COFFEE_MANUAL;
        coffee_hour = (coffee_hour + 1) % 24;
        draw_coffee_screen();
        return;
    }

    /* Hour - */
    else if (touch_rect(20, 230, 105, 280, tx, ty)) {
        coffee_mode = COFFEE_MANUAL;
        coffee_hour = (coffee_hour + 23) % 24;
        draw_coffee_screen();
        return;
    }

    /* Minute + */
    else if (touch_rect(135, 170, 220, 220, tx, ty)) {
        coffee_mode = COFFEE_MANUAL;
        coffee_min = (coffee_min + 5) % 60;
        draw_coffee_screen();
        return;
    }

    /* Minute - */
    else if (touch_rect(135, 230, 220, 280, tx, ty)) {
        coffee_mode = COFFEE_MANUAL;
        coffee_min = (coffee_min + 55) % 60;
        draw_coffee_screen();
        return;
    }

    /* Sync with wake-up toggle */
    else if (touch_rect(20, 290, 220, 330, tx, ty)) {
        coffee_mode =
            (coffee_mode == COFFEE_SYNC_WAKEUP)
            ? COFFEE_MANUAL
            : COFFEE_SYNC_WAKEUP;

        draw_coffee_screen();
        return;
    }

    /* No coffee toggle */
    else if (touch_rect(20, 340, 220, 380, tx, ty)) {
        coffee_mode =
            (coffee_mode == COFFEE_OFF)
            ? COFFEE_MANUAL
            : COFFEE_OFF;

        draw_coffee_screen();
        return;
    }
}
#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"
#include <stdio.h>

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

void handle_coffee_touch(int tx, int ty) {
    /* Hour + */
    if (tx >= 20 && tx <= 105 && ty >= 170 && ty <= 220) {
        coffee_mode = COFFEE_MANUAL;
        coffee_hour = (coffee_hour + 1) % 24;
        draw_coffee_screen();
        return;
    }

    /* Hour - */
    if (tx >= 20 && tx <= 105 && ty >= 230 && ty <= 280) {
        coffee_mode = COFFEE_MANUAL;
        coffee_hour = (coffee_hour + 23) % 24;
        draw_coffee_screen();
        return;
    }

    /* Min + */
    if (tx >= 135 && tx <= 220 && ty >= 170 && ty <= 220) {
        coffee_mode = COFFEE_MANUAL;
        coffee_min = (coffee_min + 5) % 60;
        draw_coffee_screen();
        return;
    }

    /* Min - */
    if (tx >= 135 && tx <= 220 && ty >= 230 && ty <= 280) {
        coffee_mode = COFFEE_MANUAL;
        coffee_min = (coffee_min + 55) % 60;
        draw_coffee_screen();
        return;
    }

    /* Sync with wake up toggle */
    if (tx >= 20 && tx <= 220 && ty >= 290 && ty <= 330) {
        coffee_mode = (coffee_mode == COFFEE_SYNC_WAKEUP) ? COFFEE_MANUAL : COFFEE_SYNC_WAKEUP;
        draw_coffee_screen();
        return;
    }

    /* No coffee toggle */
    if (tx >= 20 && tx <= 220 && ty >= 340 && ty <= 380) {
        coffee_mode = (coffee_mode == COFFEE_OFF) ? COFFEE_MANUAL : COFFEE_OFF;
        draw_coffee_screen();
        return;
    }

    /* Return button */
    if (tx >= 10 && tx <= 70 && ty >= 10 && ty <= 40) {
        /* navigate back to previous screen */
    }
}
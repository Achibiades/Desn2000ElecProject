
#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"

/* Lock state flags */
static int main_door_locked = 0;
static int window1_locked   = 0;
static int window2_locked   = 0;
static int window3_locked   = 0;
static int window4_locked   = 0;
static int window5_locked   = 0;

void draw_lock_screen() {
    lcd_fillScreen(WHITE);

    //teaching will git

    /* Return button */
    lcd_fillRect(10, 10, 70, 40, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(16, 20, (unsigned char *)"return");

    /* Title -- 2x scale */
    lcd_fontColor(BLACK, WHITE);
    lcd_putString_scaled(48, 55, (unsigned char *)"CENTRAL LOCK CONTROL", 2);

    /* Subtitle */
    lcd_putString(48, 42, (unsigned char *)"TAP TO LOCK / UNLOCK");

    /* Main Door tile */
    lcd_fillRect(20, 60, 115, 105, main_door_locked ? RED : LIGHT_GRAY);
    lcd_fontColor(BLACK, main_door_locked ? RED : LIGHT_GRAY);
    lcd_putString(28, 76, (unsigned char *)"MAIN DOOR");

    /* Window 1 tile */
    lcd_fillRect(125, 60, 220, 105, window1_locked ? RED : LIGHT_GRAY);
    lcd_fontColor(BLACK, window1_locked ? RED : LIGHT_GRAY);
    lcd_putString(140, 76, (unsigned char *)"WINDOW 1");

    /* Window 2 tile */
    lcd_fillRect(20, 115, 115, 160, window2_locked ? RED : LIGHT_GRAY);
    lcd_fontColor(BLACK, window2_locked ? RED : LIGHT_GRAY);
    lcd_putString(35, 131, (unsigned char *)"WINDOW 2");

    /* Window 3 tile */
    lcd_fillRect(125, 115, 220, 160, window3_locked ? RED : LIGHT_GRAY);
    lcd_fontColor(BLACK, window3_locked ? RED : LIGHT_GRAY);
    lcd_putString(140, 131, (unsigned char *)"WINDOW 3");

    /* Window 4 tile */
    lcd_fillRect(20, 170, 115, 215, window4_locked ? RED : LIGHT_GRAY);
    lcd_fontColor(BLACK, window4_locked ? RED : LIGHT_GRAY);
    lcd_putString(35, 186, (unsigned char *)"WINDOW 4");

    /* Window 5 tile */
    lcd_fillRect(125, 170, 220, 215, window5_locked ? RED : LIGHT_GRAY);
    lcd_fontColor(BLACK, window5_locked ? RED : LIGHT_GRAY);
    lcd_putString(140, 186, (unsigned char *)"WINDOW 5");

    /* Lock all button */
    lcd_fillRect(20, 235, 220, 275, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(95, 251, (unsigned char *)"LOCK ALL");
}

void handle_security_touch(int tx, int ty) {
    /* Main Door tile */
    if (tx >= 20 && tx <= 115 && ty >= 60 && ty <= 105) {
        main_door_locked = !main_door_locked;
        lcd_fillRect(20, 60, 115, 105, main_door_locked ? RED : LIGHT_GRAY);
        lcd_fontColor(BLACK, main_door_locked ? RED : LIGHT_GRAY);
        lcd_putString(28, 76, (unsigned char *)"MAIN DOOR");
        return;
    }

    /* Window 1 tile */
    if (tx >= 125 && tx <= 220 && ty >= 60 && ty <= 105) {
        window1_locked = !window1_locked;
        lcd_fillRect(125, 60, 220, 105, window1_locked ? RED : LIGHT_GRAY);
        lcd_fontColor(BLACK, window1_locked ? RED : LIGHT_GRAY);
        lcd_putString(140, 76, (unsigned char *)"WINDOW 1");
        return;
    }

    /* Window 2 tile */
    if (tx >= 20 && tx <= 115 && ty >= 115 && ty <= 160) {
        window2_locked = !window2_locked;
        lcd_fillRect(20, 115, 115, 160, window2_locked ? RED : LIGHT_GRAY);
        lcd_fontColor(BLACK, window2_locked ? RED : LIGHT_GRAY);
        lcd_putString(35, 131, (unsigned char *)"WINDOW 2");
        return;
    }

    /* Window 3 tile */
    if (tx >= 125 && tx <= 220 && ty >= 115 && ty <= 160) {
        window3_locked = !window3_locked;
        lcd_fillRect(125, 115, 220, 160, window3_locked ? RED : LIGHT_GRAY);
        lcd_fontColor(BLACK, window3_locked ? RED : LIGHT_GRAY);
        lcd_putString(140, 131, (unsigned char *)"WINDOW 3");
        return;
    }

    /* Window 4 tile */
    if (tx >= 20 && tx <= 115 && ty >= 170 && ty <= 215) {
        window4_locked = !window4_locked;
        lcd_fillRect(20, 170, 115, 215, window4_locked ? RED : LIGHT_GRAY);
        lcd_fontColor(BLACK, window4_locked ? RED : LIGHT_GRAY);
        lcd_putString(35, 186, (unsigned char *)"WINDOW 4");
        return;
    }

    /* Window 5 tile */
    if (tx >= 125 && tx <= 220 && ty >= 170 && ty <= 215) {
        window5_locked = !window5_locked;
        lcd_fillRect(125, 170, 220, 215, window5_locked ? RED : LIGHT_GRAY);
        lcd_fontColor(BLACK, window5_locked ? RED : LIGHT_GRAY);
        lcd_putString(140, 186, (unsigned char *)"WINDOW 5");
        return;
    }

    /* Lock all button */
    if (tx >= 20 && tx <= 220 && ty >= 235 && ty <= 275) {
        main_door_locked = 1;
        window1_locked = 1;
        window2_locked = 1;
        window3_locked = 1;
        window4_locked = 1;
        window5_locked = 1;
        draw_lock_screen();  /* simplest to just redraw everything here */
        return;
    }

    /* Return button */
    if (tx >= 10 && tx <= 70 && ty >= 10 && ty <= 40) {
        /* navigate back to previous screen */
    }
}
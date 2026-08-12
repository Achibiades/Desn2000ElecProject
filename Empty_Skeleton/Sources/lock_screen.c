#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"
#include "../LCD/lcd_grph.h"
#include <stdio.h>
#include <string.h>
#include "../Headers/SystemInit.h"

/* Lock state flags */
static int main_door_locked = 0;
static int window1_locked   = 0;
static int window2_locked   = 0;
static int window3_locked   = 0;
static int window4_locked   = 0;
static int window5_locked   = 0;

void draw_lock_screen() {
    lcd_fillScreen(WHITE);

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

static void draw_lock_tile(
    int x1,
    int y1,
    int x2,
    int y2,
    int text_x,
    int text_y,
    unsigned char *label,
    int locked)
{
    unsigned int colour = locked ? RED : LIGHT_GRAY;

    lcd_fillRect(x1, y1, x2, y2, colour);
    lcd_fontColor(BLACK, colour);
    lcd_putString(text_x, text_y, label);
}

void handle_lock_touch(int tx, int ty)
{
    /* Return button */
    if (touch_rect(10, 10, 70, 40, tx, ty)) {
        /* curr_ui = previous screen; */
        return;
    }

    /* Main door */
    else if (touch_rect(20, 60, 115, 105, tx, ty)) {
        main_door_locked = !main_door_locked;

        draw_lock_tile(
            20, 60, 115, 105,
            28, 76,
            (unsigned char *)"MAIN DOOR",
            main_door_locked
        );

        return;
    }

    /* Window 1 */
    else if (touch_rect(125, 60, 220, 105, tx, ty)) {
        window1_locked = !window1_locked;

        draw_lock_tile(
            125, 60, 220, 105,
            140, 76,
            (unsigned char *)"WINDOW 1",
            window1_locked
        );

        return;
    }

    /* Window 2 */
    else if (touch_rect(20, 115, 115, 160, tx, ty)) {
        window2_locked = !window2_locked;

        draw_lock_tile(
            20, 115, 115, 160,
            35, 131,
            (unsigned char *)"WINDOW 2",
            window2_locked
        );

        return;
    }

    /* Window 3 */
    else if (touch_rect(125, 115, 220, 160, tx, ty)) {
        window3_locked = !window3_locked;

        draw_lock_tile(
            125, 115, 220, 160,
            140, 131,
            (unsigned char *)"WINDOW 3",
            window3_locked
        );

        return;
    }

    /* Window 4 */
    else if (touch_rect(20, 170, 115, 215, tx, ty)) {
        window4_locked = !window4_locked;

        draw_lock_tile(
            20, 170, 115, 215,
            35, 186,
            (unsigned char *)"WINDOW 4",
            window4_locked
        );

        return;
    }

    /* Window 5 */
    else if (touch_rect(125, 170, 220, 215, tx, ty)) {
        window5_locked = !window5_locked;

        draw_lock_tile(
            125, 170, 220, 215,
            140, 186,
            (unsigned char *)"WINDOW 5",
            window5_locked
        );

        return;
    }

    /* Lock all */
    else if (touch_rect(20, 235, 220, 275, tx, ty)) {
        main_door_locked = 1;
        window1_locked = 1;
        window2_locked = 1;
        window3_locked = 1;
        window4_locked = 1;
        window5_locked = 1;

        draw_lock_tile(
            20, 60, 115, 105, 28, 76,
            (unsigned char *)"MAIN DOOR",
            main_door_locked
        );

        draw_lock_tile(
            125, 60, 220, 105, 140, 76,
            (unsigned char *)"WINDOW 1",
            window1_locked
        );

        draw_lock_tile(
            20, 115, 115, 160, 35, 131,
            (unsigned char *)"WINDOW 2",
            window2_locked
        );

        draw_lock_tile(
            125, 115, 220, 160, 140, 131,
            (unsigned char *)"WINDOW 3",
            window3_locked
        );

        draw_lock_tile(
            20, 170, 115, 215, 35, 186,
            (unsigned char *)"WINDOW 4",
            window4_locked
        );

        draw_lock_tile(
            125, 170, 220, 215, 140, 186,
            (unsigned char *)"WINDOW 5",
            window5_locked
        );

        return;
    }
}

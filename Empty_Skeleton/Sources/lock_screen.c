#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"
#include "../LCD/lcd_grph.h"
#include <stdio.h>
#include <string.h>
#include "../Headers/SystemInit.h"
#include "../Headers/stm32f103xe.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"

/* APE-board LED5 is connected to PF5 */
#define LOCK_LED_PIN 5U

/* Lock states: 0 = unlocked, 1 = locked */
static int main_door_locked = 0;
static int window1_locked   = 0;
static int window2_locked   = 0;
static int window3_locked   = 0;
static int window4_locked   = 0;
static int window5_locked   = 0;


/*
 * Configure APE-board LED5 (PF5).
 * Call this once during program initialization.
 */
void lock_led_init(void)
{
    /* Enable the GPIOF peripheral clock */
    RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;

    /*
     * Configure PF5 as:
     * MODE5 = 10: output at 2 MHz
     * CNF5  = 00: general-purpose push-pull
     */
    GPIOF->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5);
    GPIOF->CRL |= GPIO_CRL_MODE5_1;

    /* Start with LED5 off */
    GPIOF->BRR = (1U << LOCK_LED_PIN);
}


/*
 * Return 1 only when the main door and all windows are locked.
 */
static int all_locks_are_locked(void)
{
    return main_door_locked &&
           window1_locked &&
           window2_locked &&
           window3_locked &&
           window4_locked &&
           window5_locked;
}


/*
 * LED5 indicates whether everything is locked.
 */
static void update_lock_led(void)
{
    if (all_locks_are_locked()) {
        /* Turn LED5 on */
        GPIOF->BSRR = (1U << LOCK_LED_PIN);
    }
    else {
        /* Turn LED5 off */
        GPIOF->BRR = (1U << LOCK_LED_PIN);
    }
}


/*
 * Draw or update one lock tile.
 */
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
    unsigned int colour;

    if (locked) {
        colour = RED;
    }
    else {
        colour = LIGHT_GRAY;
    }

    /*
     * Filling the tile erases its old text, so the text must also
     * be drawn again.
     */
    lcd_fillRect(x1, y1, x2, y2, colour);
    lcd_fontColor(BLACK, colour);
    lcd_putString(text_x, text_y, label);
}


/*
 * Draw all six lock tiles.
 */
static void draw_all_lock_tiles(void)
{
    draw_lock_tile(
        20, 60, 115, 105,
        28, 76,
        (unsigned char *)"MAIN DOOR",
        main_door_locked
    );

    draw_lock_tile(
        125, 60, 220, 105,
        140, 76,
        (unsigned char *)"WINDOW 1",
        window1_locked
    );

    draw_lock_tile(
        20, 115, 115, 160,
        35, 131,
        (unsigned char *)"WINDOW 2",
        window2_locked
    );

    draw_lock_tile(
        125, 115, 220, 160,
        140, 131,
        (unsigned char *)"WINDOW 3",
        window3_locked
    );

    draw_lock_tile(
        20, 170, 115, 215,
        35, 186,
        (unsigned char *)"WINDOW 4",
        window4_locked
    );

    draw_lock_tile(
        125, 170, 220, 215,
        140, 186,
        (unsigned char *)"WINDOW 5",
        window5_locked
    );
}


/*
 * Draw the complete lock-control screen.
 *
 * Call this once when entering the lock screen.
 * Do not repeatedly call it from the main loop.
 */
void draw_lock_screen(void)
{
    lcd_fillScreen(WHITE);

    /* Return button */
    lcd_fillRect(10, 10, 70, 40, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(
        16,
        20,
        (unsigned char *)"return"
    );

    /* Screen heading */
    lcd_fontColor(BLACK, WHITE);
    lcd_putString(
        82,
        20,
        (unsigned char *)"LOCK CONTROL"
    );

    lcd_putString(
        48,
        45,
        (unsigned char *)"TAP TO LOCK / UNLOCK"
    );

    /* Draw the six lock controls */
    draw_all_lock_tiles();

    /* Lock-all button */
    lcd_fillRect(20, 235, 220, 275, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(
        95,
        251,
        (unsigned char *)"LOCK ALL"
    );

    /* Ensure LED5 matches the current states */
    update_lock_led();
}


/*
 * Handle touchscreen presses on the lock-control screen.
 */
void handle_lock_touch(int tx, int ty)
{
    /* Return button */
    if (touch_rect(10, 10, 70, 40, tx, ty)) {
        /*
         * If navigation is handled here, use something like:
         *
         * curr_ui = 0;
         * draw_main_screen();
         *
         * Otherwise, handle this button in your main UI code.
         */
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

        update_lock_led();
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

        update_lock_led();
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

        update_lock_led();
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

        update_lock_led();
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

        update_lock_led();
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

        update_lock_led();
        return;
    }

    /* Lock everything */
    else if (touch_rect(20, 235, 220, 275, tx, ty)) {
        main_door_locked = 1;
        window1_locked = 1;
        window2_locked = 1;
        window3_locked = 1;
        window4_locked = 1;
        window5_locked = 1;

        /*
         * All six states changed, so redraw all six tiles.
         * The rest of the screen is not redrawn.
         */
        draw_all_lock_tiles();

        update_lock_led();
        return;
    }
}

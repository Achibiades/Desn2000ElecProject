#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"
#include "../Headers/SystemInit.h"
#include "../Headers/stm32f103xe.h"
#include <stdio.h>
#include <string.h>

/* APE-board LED2 is connected to PF2 */
#define LIGHT_LED_PIN 2U

/* Light states: 0 = off, 1 = on */
static int bedroom1_light = 0;
static int bedroom2_light = 0;
static int bedroom3_light = 0;
static int bathroom_light = 0;
static int kitchen_light = 0;
static int living_room_light = 0;


/*
 * Configure APE-board LED2 (PF2).
 * Call this once during program initialization.
 */
void light_led_init(void)
{
    /* Enable the GPIOF peripheral clock */
    RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;

    /*
     * Configure PF2 as:
     * MODE2 = 10: output at 2 MHz
     * CNF2  = 00: general-purpose push-pull
     */
    GPIOF->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_MODE2);
    GPIOF->CRL |= GPIO_CRL_MODE2_1;

    /* Start with LED2 off */
    GPIOF->BRR = (1U << LIGHT_LED_PIN);
}


/*
 * Return 1 only when all six lights are on.
 */
static int all_lights_are_on(void)
{
    return bedroom1_light &&
           bedroom2_light &&
           bedroom3_light &&
           bathroom_light &&
           kitchen_light &&
           living_room_light;
}


/*
 * LED2 turns on only when all six lights are on.
 */
static void update_light_led(void)
{
    if (all_lights_are_on()) {
        /* Turn LED2 on */
        GPIOF->BSRR = (1U << LIGHT_LED_PIN);
    }
    else {
        /* Turn LED2 off */
        GPIOF->BRR = (1U << LIGHT_LED_PIN);
    }
}


/*
 * Draw or update one light tile.
 *
 * A red tile means the light is on.
 * A grey tile means the light is off.
 */
static void draw_light_tile(
    int x1,
    int y1,
    int x2,
    int y2,
    int text_x,
    int text_y,
    unsigned char *label,
    int light_on)
{
    unsigned int colour;

    if (light_on) {
        colour = RED;
    }
    else {
        colour = LIGHT_GRAY;
    }

    /*
     * Filling the tile removes its old label, so redraw
     * the label after changing the background colour.
     */
    lcd_fillRect(x1, y1, x2, y2, colour);
    lcd_fontColor(BLACK, colour);
    lcd_putString(text_x, text_y, label);
}


/*
 * Draw all six light tiles.
 *
 * This is used when initially drawing the screen and
 * when the TURN ALL ON button is pressed.
 */
static void draw_all_light_tiles(void)
{
    /* Bedroom 1 */
    draw_light_tile(
        20, 60, 115, 105,
        28, 76,
        (unsigned char *)"BEDROOM 1",
        bedroom1_light
    );

    /* Bedroom 2 */
    draw_light_tile(
        125, 60, 220, 105,
        133, 76,
        (unsigned char *)"BEDROOM 2",
        bedroom2_light
    );

    /* Bedroom 3 */
    draw_light_tile(
        20, 115, 115, 160,
        28, 131,
        (unsigned char *)"BEDROOM 3",
        bedroom3_light
    );

    /* Bathroom */
    draw_light_tile(
        125, 115, 220, 160,
        138, 131,
        (unsigned char *)"BATHROOM",
        bathroom_light
    );

    /* Kitchen */
    draw_light_tile(
        20, 170, 115, 215,
        38, 186,
        (unsigned char *)"KITCHEN",
        kitchen_light
    );

    /* Living room */
    draw_light_tile(
        125, 170, 220, 215,
        131, 186,
        (unsigned char *)"LIVING ROOM",
        living_room_light
    );
}


/*
 * Draw the complete light-control screen.
 *
 * Call this once when entering the light screen.
 */
void draw_light_screen(void)
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
        (unsigned char *)"LIGHT CONTROL"
    );

    lcd_putString(
        55,
        45,
        (unsigned char *)"TAP TO TURN ON / OFF"
    );

    /* Draw the six light controls */
    draw_all_light_tiles();

    /* Turn-all-on button */
    lcd_fillRect(20, 235, 220, 275, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(
        78,
        251,
        (unsigned char *)"TURN ALL ON"
    );

    /* Ensure LED2 matches the current states */
    update_light_led();
}


/*
 * Handle touchscreen presses on the light-control screen.
 *
 * The return button is handled by main(), because curr_ui
 * is a local variable inside main().
 */
void handle_light_touch(int tx, int ty)
{
    /* Return button */
    if (touch_rect(10, 10, 70, 40, tx, ty)) {
        /*
         * Navigation is handled in main().
         */
        return;
    }

    /* Bedroom 1 */
    else if (touch_rect(20, 60, 115, 105, tx, ty)) {
        bedroom1_light = !bedroom1_light;

        draw_light_tile(
            20, 60, 115, 105,
            28, 76,
            (unsigned char *)"BEDROOM 1",
            bedroom1_light
        );

        update_light_led();
        return;
    }

    /* Bedroom 2 */
    else if (touch_rect(125, 60, 220, 105, tx, ty)) {
        bedroom2_light = !bedroom2_light;

        draw_light_tile(
            125, 60, 220, 105,
            133, 76,
            (unsigned char *)"BEDROOM 2",
            bedroom2_light
        );

        update_light_led();
        return;
    }

    /* Bedroom 3 */
    else if (touch_rect(20, 115, 115, 160, tx, ty)) {
        bedroom3_light = !bedroom3_light;

        draw_light_tile(
            20, 115, 115, 160,
            28, 131,
            (unsigned char *)"BEDROOM 3",
            bedroom3_light
        );

        update_light_led();
        return;
    }

    /* Bathroom */
    else if (touch_rect(125, 115, 220, 160, tx, ty)) {
        bathroom_light = !bathroom_light;

        draw_light_tile(
            125, 115, 220, 160,
            138, 131,
            (unsigned char *)"BATHROOM",
            bathroom_light
        );

        update_light_led();
        return;
    }

    /* Kitchen */
    else if (touch_rect(20, 170, 115, 215, tx, ty)) {
        kitchen_light = !kitchen_light;

        draw_light_tile(
            20, 170, 115, 215,
            38, 186,
            (unsigned char *)"KITCHEN",
            kitchen_light
        );

        update_light_led();
        return;
    }

    /* Living room */
    else if (touch_rect(125, 170, 220, 215, tx, ty)) {
        living_room_light = !living_room_light;

        draw_light_tile(
            125, 170, 220, 215,
            131, 186,
            (unsigned char *)"LIVING ROOM",
            living_room_light
        );

        update_light_led();
        return;
    }

    /* Turn all lights on */
    else if (touch_rect(20, 235, 220, 275, tx, ty)) {
        bedroom1_light = 1;
        bedroom2_light = 1;
        bedroom3_light = 1;
        bathroom_light = 1;
        kitchen_light = 1;
        living_room_light = 1;

        /*
         * All six states may have changed, so redraw all
         * six tiles without clearing the whole screen.
         */
        draw_all_light_tiles();

        update_light_led();
        return;
    }
}

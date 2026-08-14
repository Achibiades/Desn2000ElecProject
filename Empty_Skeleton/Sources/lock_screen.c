#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"
#include "../Headers/SystemInit.h"
#include "../Headers/stm32f103xe.h"
#include <stdio.h>
#include <string.h>

//led is port f pin 5
#define LOCK_LED_PIN 5U

//0 = unlock, 1 = locked
static int main_door_locked = 0;
static int window1_locked = 0;
static int window2_locked = 0;
static int window3_locked = 0;
static int window4_locked = 0;
static int window5_locked = 0;


//return 1 when all doors and windows are locked
static int all_locks_are_locked(void) {
    return main_door_locked && window1_locked && window2_locked && window3_locked && window4_locked && window5_locked;
}


//led5 on when all doors and windows are locked
static void update_lock_led(void) {
    if (all_locks_are_locked()) {
        //LED5 on
        GPIOF->BSRR = (1U << LOCK_LED_PIN);
    }
    else {
        //LED5 off
        GPIOF->BRR = (1U << LOCK_LED_PIN);
    }
}


//updating one tile; grey is unlocked, red is locked
static void draw_lock_tile(int x1, int y1, int x2, int y2, int text_x, int text_y, unsigned char *label, int locked) {
    unsigned int colour;

    if (locked) {
        colour = RED;
    }
    else {
        colour = LIGHT_GRAY;
    }

    //add label back after colouring
    lcd_fillRect(x1, y1, x2, y2, colour);
    lcd_fontColor(BLACK, colour);
    lcd_putString(text_x, text_y, label);
}


//update all tiles
static void draw_all_lock_tiles(void) {
    //main door
    draw_lock_tile(20, 60, 115, 105, 28, 76, (unsigned char *)"MAIN DOOR", main_door_locked);

    //window 1
    draw_lock_tile(125, 60, 220, 105, 140, 76, (unsigned char *)"WINDOW 1", window1_locked);

    //window 2
    draw_lock_tile(20, 115, 115, 160, 35, 131, (unsigned char *)"WINDOW 2", window2_locked);

    //window 3
    draw_lock_tile(125, 115, 220, 160, 140, 131, (unsigned char *)"WINDOW 3", window3_locked);

    //window 4
    draw_lock_tile(20, 170, 115, 215, 35, 186, (unsigned char *)"WINDOW 4", window4_locked);

    //window 5
    draw_lock_tile(125, 170, 220, 215, 140, 186, (unsigned char *)"WINDOW 5", window5_locked);
}


//unlock everything
void unlock_all(int curr_ui) {
    main_door_locked = 0;
    window1_locked = 0;
    window2_locked = 0;
    window3_locked = 0;
    window4_locked = 0;
    window5_locked = 0;

    update_lock_led();

    //only redraw if lock screen is visible
    if (curr_ui == 5) {
        draw_all_lock_tiles();
    }
}


//lock everything
void lock_all(int curr_ui) {
    main_door_locked = 1;
    window1_locked = 1;
    window2_locked = 1;
    window3_locked = 1;
    window4_locked = 1;
    window5_locked = 1;

    update_lock_led();

    //only redraw if lock screen is visible
    if (curr_ui == 5) {
        draw_all_lock_tiles();
    }
}


//draw full screen
void draw_lock_screen(void) {
    lcd_fillScreen(WHITE);

    //return button
    lcd_fillRect(10, 10, 70, 40, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(16, 20, (unsigned char *)"return");

    //title
    lcd_fontColor(BLACK, WHITE);
    lcd_putString(82, 20, (unsigned char *)"LOCK CONTROL");
    lcd_putString(48, 45, (unsigned char *)"TAP TO LOCK / UNLOCK");

    //draw all lock tiles
    draw_all_lock_tiles();

    //lock all button
    lcd_fillRect(20, 235, 220, 275, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(95, 251, (unsigned char *)"LOCK ALL");

    update_lock_led();
}


//handle touchscreen input
void handle_lock_touch(int tx, int ty) {
    //return
    if (touch_rect(10, 10, 70, 40, tx, ty)) {
        //handled in main
        return;
    }

    //main door
    else if (touch_rect(20, 60, 115, 105, tx, ty)) {
        main_door_locked = !main_door_locked;

        draw_lock_tile(20, 60, 115, 105, 28, 76, (unsigned char *)"MAIN DOOR", main_door_locked);

        update_lock_led();
        return;
    }

    //window 1
    else if (touch_rect(125, 60, 220, 105, tx, ty)) {
        window1_locked = !window1_locked;

        draw_lock_tile(125, 60, 220, 105, 140, 76, (unsigned char *)"WINDOW 1", window1_locked);

        update_lock_led();
        return;
    }

    //window 2
    else if (touch_rect(20, 115, 115, 160, tx, ty)) {
        window2_locked = !window2_locked;

        draw_lock_tile(20, 115, 115, 160, 35, 131, (unsigned char *)"WINDOW 2", window2_locked);

        update_lock_led();
        return;
    }

    //window 3
    else if (touch_rect(125, 115, 220, 160, tx, ty)) {
        window3_locked = !window3_locked;

        draw_lock_tile(125, 115, 220, 160, 140, 131, (unsigned char *)"WINDOW 3", window3_locked);

        update_lock_led();
        return;
    }

    //window 4
    else if (touch_rect(20, 170, 115, 215, tx, ty)) {
        window4_locked = !window4_locked;

        draw_lock_tile(20, 170, 115, 215, 35, 186, (unsigned char *)"WINDOW 4", window4_locked);

        update_lock_led();
        return;
    }

    //window 5
    else if (touch_rect(125, 170, 220, 215, tx, ty)) {
        window5_locked = !window5_locked;

        draw_lock_tile(125, 170, 220, 215, 140, 186, (unsigned char *)"WINDOW 5", window5_locked);

        update_lock_led();
        return;
    }

    //lock everything
    else if (touch_rect(20, 235, 220, 275, tx, ty)) {
        lock_all(5);
        return;
    }
}

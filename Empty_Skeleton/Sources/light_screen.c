#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"
#include "../Headers/SystemInit.h"
#include "../Headers/stm32f103xe.h"
#include <stdio.h>
#include <string.h>

//led2 is connected to port f pin2 */
#define LIGHT_LED_PIN 2U

// 0 = off, 1 = on
static int bedroom1_light = 0;
static int bedroom2_light = 0;
static int bedroom3_light = 0;
static int bathroom_light = 0;
static int kitchen_light = 0;
static int living_room_light = 0;


//return 1 when all lights on
static int all_lights_are_on(void) {
    return bedroom1_light && bedroom2_light && bedroom3_light && bathroom_light && kitchen_light && living_room_light;
}


//led2 on when all lights on
static void update_light_led(void) {
    if (all_lights_are_on()) {
        //led2 on
        GPIOF->BSRR = (1U << LIGHT_LED_PIN);
    }
    else {
        //turn led2 off
        GPIOF->BRR = (1U << LIGHT_LED_PIN);
    }
}


/*
 * Draw or update one light tile.
 *
 * A red tile means the light is on.
 * A grey tile means the light is off.
 */

//updating one tile. grey is off, blue is on
static void draw_light_tile(int x1, int y1, int x2, int y2, int text_x, int text_y, unsigned char *label, int light_on) {
    unsigned int colour;

    if (light_on) {
        colour = BLUE;
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
static void draw_all_light_tiles(void) {
    //bed1
    draw_light_tile( 20, 60, 115, 105, 28, 76, (unsigned char *)"BEDROOM 1", bedroom1_light);

    //bed2
    draw_light_tile(125, 60, 220, 105, 133, 76, (unsigned char *)"BEDROOM 2", bedroom2_light);

   //bed3
    draw_light_tile(20, 115, 115, 160, 28, 131, (unsigned char *)"BEDROOM 3", bedroom3_light);

    //bath
    draw_light_tile(125, 115, 220, 160, 138, 131, (unsigned char *)"BATHROOM", bathroom_light);

    //kitchen
    draw_light_tile(20, 170, 115, 215, 38, 186, (unsigned char *)"KITCHEN", kitchen_light);

    //living
    draw_light_tile(125, 170, 220, 215, 131, 186, (unsigned char *)"LIVING ROOM", living_room_light);
}


//draw full screen
void draw_light_screen(void) {
    lcd_fillScreen(WHITE);

    //return
    lcd_fillRect(10, 10, 70, 40, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(16, 20, (unsigned char *)"return");

    //title
    lcd_fontColor(BLACK, WHITE);
    lcd_putString(82, 20, (unsigned char *)"LIGHT CONTROL");

    lcd_putString(55, 45, (unsigned char *)"TAP TO TURN ON / OFF");

    draw_all_light_tiles();

    //turn on all button
    lcd_fillRect(20, 235, 220, 275, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(78, 251,(unsigned char *)"TURN ALL ON");

    update_light_led();
}



void handle_light_touch(int tx, int ty)
{
    //return
    if (touch_rect(10, 10, 70, 40, tx, ty)) {
        //handled in main
        return;
    }

    //bed1
    else if (touch_rect(20, 60, 115, 105, tx, ty)) {
        bedroom1_light = !bedroom1_light;
        draw_light_tile(20, 60, 115, 105, 28, 76, (unsigned char *)"BEDROOM 1", bedroom1_light);

        update_light_led();
        return;
    }

    //bed2
    else if (touch_rect(125, 60, 220, 105, tx, ty)) {
        bedroom2_light = !bedroom2_light;

        draw_light_tile(125, 60, 220, 105, 133, 76, (unsigned char *)"BEDROOM 2",bedroom2_light);

        update_light_led();
        return;
    }

    //bed3
    else if (touch_rect(20, 115, 115, 160, tx, ty)) {
        bedroom3_light = !bedroom3_light;

        draw_light_tile(20, 115, 115, 160, 28, 131, (unsigned char *)"BEDROOM 3", bedroom3_light);

        update_light_led();
        return;
    }

    //bath
    else if (touch_rect(125, 115, 220, 160, tx, ty)) {
        bathroom_light = !bathroom_light;

        draw_light_tile(125, 115, 220, 160, 138, 131, (unsigned char *)"BATHROOM", bathroom_light);

        update_light_led();
        return;
    }

    //kitchen
    else if (touch_rect(20, 170, 115, 215, tx, ty)) {
        kitchen_light = !kitchen_light;

        draw_light_tile(20, 170, 115, 215, 38, 186, (unsigned char *)"KITCHEN", kitchen_light);

        update_light_led();
        return;
    }

    // living room
    else if (touch_rect(125, 170, 220, 215, tx, ty)) {
        living_room_light = !living_room_light;

        draw_light_tile(125, 170, 220, 215, 131, 186, (unsigned char *)"LIVING ROOM", living_room_light);

        update_light_led();
        return;
    }

    //turn all on buton
    else if (touch_rect(20, 235, 220, 275, tx, ty)) {
        bedroom1_light = 1;
        bedroom2_light = 1;
        bedroom3_light = 1;
        bathroom_light = 1;
        kitchen_light = 1;
        living_room_light = 1;

        draw_all_light_tiles();

        update_light_led();
        return;
    }
}


void lights_off(int curr_ui) {
    bedroom1_light = 0;
    bedroom2_light = 0;
    bedroom3_light = 0;
    bathroom_light = 0;
    kitchen_light = 0;
    living_room_light = 0;

    update_light_led();
    
    //all tiles could have changed
		if (curr_ui == 2) {
			draw_all_light_tiles();
		}
}

void lights_on(int curr_ui) {
    bedroom1_light = 1;
    bedroom2_light = 1;
    bedroom3_light = 1;
    bathroom_light = 1;
    kitchen_light = 1;
    living_room_light = 1;

    update_light_led();

    //all tiles could have changed 
		if (curr_ui == 2) {
			draw_all_light_tiles();
		}
}

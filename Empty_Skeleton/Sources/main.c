#include "../Headers/stm32f103xe.h"
#include "../LCD/ili9341.h"
#include "../LCD/lcd_hw.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/SystemInit.h"
#include <stdio.h>

int main() {

    int x_coord = 0;
    int y_coord = 0;

    int curr_ui = 0;
    int prev_ui = -1; // Initialize to -1 so the home screen draws on the very first loop
    int was_touched = 0;
	char str[32];

	lcd_init();
    spi_init();

    while (1) {

        // RENDERING LOGIC

        if (curr_ui != prev_ui) {

            // 0=Home, 1=Coffee, 2=Light, 3=Temp, 4=Alarm, 5=Security, 6=Sound
            if (curr_ui == 0) {
                draw_home_screen();
            } else if (curr_ui == 1) {
                draw_coffee_screen();
            } else if (curr_ui == 2) {
                draw_light_screen();
            } else if (curr_ui == 3) {
                draw_temp_screen();
            } else if (curr_ui == 4) {
                draw_alarm_screen();
            } else if (curr_ui == 5) {
                draw_lock_screen();
            } else if (curr_ui == 6) {
                draw_sound_screen(); 
            }

            // Update prev_ui so it doesn't draw again until curr_ui changes
            prev_ui = curr_ui;
        }


        // TOUCH INTERACTION LOGIC


        if (touch_read_xy(&x_coord, &y_coord) == 1) {

            // Only process the touch once per press
            if (was_touched == 0) {
                was_touched = 1;

                // --- HOME PAGE TOUCH LOGIC (curr_ui == 0)
                if (curr_ui == 0) {

                    // Box 1 Check (Coffee)
                    if (touch_rect(20, 50, 110, 120, x_coord, y_coord)) {
                        curr_ui = 1;
                    }
                    // Box 2 Check (Light)
                    else if (touch_rect(20, 140, 110, 210, x_coord, y_coord)) {
                        curr_ui = 2;
                    }
                    // Box 3 Check (Temp)
                    else if (touch_rect(20, 230, 110, 300, x_coord, y_coord)) {
                        curr_ui = 3;
                    }
                    // Box 4 Check (Alarm)
                    else if (touch_rect(130, 50, 220, 120, x_coord, y_coord)) {
                        curr_ui = 4;
                    }
                    // Box 5 Check (Security)
                    else if (touch_rect(130, 140, 220, 210, x_coord, y_coord)) {
                        curr_ui = 5;
                    }
                    // Box 6 Check (Sound)
                    else if (touch_rect(130, 230, 220, 300, x_coord, y_coord)) {
                        curr_ui = 6;
                    }


					sprintf(str, "%d", curr_ui);
					lcd_fontColor(WHITE,BLACK);
					lcd_putString(10,10, (unsigned char *)str);

                }

				/*
                // --- OTHER PAGES TOUCH LOGIC ---
                // Assigning the return value to curr_ui so these functions can navigate back to home (0)
                else if (curr_ui == 1) {
                    curr_ui = coffee_logic();
                } else if (curr_ui == 2) {
                    curr_ui = light_logic();
                } else if (curr_ui == 3) {
                    curr_ui = temp_logic();
                } else if (curr_ui == 4) {
                    curr_ui = alarm_logic();
                } else if (curr_ui == 5) {
                    curr_ui = security_logic();
                } else if (curr_ui == 6) {
                    curr_ui = sound_logic();
                } */

            }

        } else {
            // Screen is no longer being touched, reset the flag
            was_touched = 0;
        }
    }

    return 0;
}
#include "../Headers/stm32f103xe.h"
#include "../LCD/ili9341.h"
#include "../LCD/lcd_hw.h"
#include "../LCD/lcd_grph.h"

int main() {

    int x_coord = 0;
    int y_coord = 0;

    int curr_ui = 0;
    int prev_ui = -1; // Initialize to -1 so the home screen draws on the very first loop
    int was_touched = 0;

	lcd_setup();
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
                draw_security_screen();
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
                    if (touch_rect(BOX1_X0, BOX1_Y0, BOX1_X1, BOX1_Y1, x_coord, y_coord)) {
                        curr_ui = 1;
                    }
                    // Box 2 Check (Light)
                    else if (touch_rect(BOX2_X0, BOX2_Y0, BOX2_X1, BOX2_Y1, x_coord, y_coord)) {
                        curr_ui = 2;
                    }
                    // Box 3 Check (Temp)
                    else if (touch_rect(10, 80, 100, 120, x_coord, y_coord)) {
                        curr_ui = 3;
                    }
                    // Box 4 Check (Alarm)
                    else if (touch_rect(120, 80, 210, 120, x_coord, y_coord)) {
                        curr_ui = 4;
                    }
                    // Box 5 Check (Security)
                    else if (touch_rect(10, 150, 100, 190, x_coord, y_coord)) {
                        curr_ui = 5;
                    }
                    // Box 6 Check (Sound)
                    else if (touch_rect(120, 150, 210, 190, x_coord, y_coord)) {
                        curr_ui = 6;
                    }
                }

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
                }

            }
        } else {
            // Screen is no longer being touched, reset the flag
            was_touched = 0;
        }
    }

    return 0;
}
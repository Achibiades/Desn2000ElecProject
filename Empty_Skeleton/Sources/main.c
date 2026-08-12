#include "../Headers/stm32f103xe.h"
#include "../LCD/ili9341.h"
#include "../LCD/lcd_hw.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/SystemInit.h"
#include "../Headers/draw_screens.h"
#include "../Headers/rtc.h"
#include <stdio.h>

int main() {

    int x_coord = 0;
    int y_coord = 0;

    int curr_ui = 0;
    int prev_ui = -1; // Initialize to -1 so the home screen draws on the very first loop
    int was_touched = 0;
    char str[32];

    // Alarm state, owned by main and passed down into the alarm screen logic
    int alarm_hour = 7;
    int alarm_minute = 30;
    int alarm_enable = 0;
    int wakeup_sunrise = 0;

    // Live clock, refreshed every loop from the RTC
    char time_str[6];
    char date_str[6];

    lcd_init();
    spi_init();
    rtc_init();

    while (1) {

        // Keep the live clock current every loop -- this is what check_time()
        // compares the alarm setting against
        get_real_time(time_str, date_str);

        // Alarm trigger check runs every loop regardless of which screen is
        // showing or whether the screen is being touched, since the alarm
        // needs to go off at the right time even if the user isn't looking
        // at the alarm page
        if (alarm_enable) {
            if (check_time(time_str, alarm_hour, alarm_minute)) {
                // play_alarm_sound();
                alarm_enable = 0;

                // Only safe to touch the LCD with the alarm-screen layout
                // if that's actually what's on screen right now
                if (curr_ui == 4) {
                    draw_alarm_state(alarm_enable);
                }
            }
        }


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
                // Populate the dynamic parts of the alarm screen now that
                // the static layout has just been drawn
                draw_alarm_time(alarm_hour, alarm_minute, 60, 104);
                draw_alarm_state(alarm_enable);
            } else if (curr_ui == 5) {
                draw_lock_screen();
            } else if (curr_ui == 6) {
                // draw_sound_screen();
            }

            // Update prev_ui so it doesn't draw again until curr_ui changes
            prev_ui = curr_ui;
        }


        // TOUCH INTERACTION LOGIC


        if (touch_read_xy(&x_coord, &y_coord) == 1) {

            // Raw controller readings need mapping into screen-pixel space
            // before they can be compared against touch_rect() boxes
            //x_coord = map_pixel(x_coord, 10, 234, 0, 240);
            //y_coord = map_pixel(y_coord, 0, 250, 0, 320);

            // Only process the touch once per press
            if (was_touched == 0) {
                was_touched = 1;

                if (curr_ui == 0) {

                    // --- HOME PAGE TOUCH LOGIC ---

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

                    sprintf(str, "%d, %d", x_coord, y_coord);
                    lcd_fontColor(WHITE, BLACK);
                    lcd_putString(10, 10, (unsigned char *)str);

                } else if (curr_ui == 1) {
                    if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
                        curr_ui = 0;
                    } else {
                       handle_coffee_touch(x_coord, y_coord);
                    }
                } else if (curr_ui == 2) {
                    if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
                        curr_ui = 0;
                    } else {
                        //curr_ui = light_logic();
                    }
                } else if (curr_ui == 3) {
                    if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
                        curr_ui = 0;
                    } else {
                        //curr_ui = temp_logic();
                    }
                } else if (curr_ui == 4) {

                    // --- ALARM PAGE TOUCH LOGIC ---

                    // Return button
                    if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
                        curr_ui = 0;
                    }
                    // Hour +
                    else if (touch_rect(20, 180, 60, 220, x_coord, y_coord)) {
                        change_hour(&alarm_hour, 1);
                        draw_alarm_time(alarm_hour, alarm_minute, 60, 104);
                    }
                    // Hour -
                    else if (touch_rect(70, 180, 110, 220, x_coord, y_coord)) {
                        change_hour(&alarm_hour, 0);
                        draw_alarm_time(alarm_hour, alarm_minute, 60, 104);
                    }
                    // Minute +
                    else if (touch_rect(130, 180, 170, 220, x_coord, y_coord)) {
                        change_minute(&alarm_minute, 1);
                        draw_alarm_time(alarm_hour, alarm_minute, 60, 104);
                    }
                    // Minute -
                    else if (touch_rect(180, 180, 220, 220, x_coord, y_coord)) {
                        change_minute(&alarm_minute, 0);
                        draw_alarm_time(alarm_hour, alarm_minute, 60, 104);
                    }
                    // Wake up to sunrise toggle
                    else if (touch_rect(20, 235, 220, 260, x_coord, y_coord)) {
                        if (wakeup_sunrise == 0) {
                            wakeup_sunrise = 1;
                        } else {
                            wakeup_sunrise = 0;
                        }
                    }
                    // Alarm ON
                    else if (touch_rect(20, 275, 110, 305, x_coord, y_coord)) {
                        alarm_enable = 1;
                        draw_alarm_state(alarm_enable);
                    }
                    // Alarm OFF
                    else if (touch_rect(130, 275, 220, 305, x_coord, y_coord)) {
                        alarm_enable = 0;
                        draw_alarm_state(alarm_enable);
                    }

                } else if (curr_ui == 5) {
                    if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
                        curr_ui = 0;
                    } else {
                        handle_lock_touch(x_coord, y_coord);
                    }
                } else if (curr_ui == 6) {
                    if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
                        curr_ui = 0;
                    } else {
                        //curr_ui = sound_logic();
                    }
                }

            }

        } else {
            // Screen is no longer being touched, reset the flag
            was_touched = 0;
        }
    }

    return 0;
}



void touch_logic(int x_coord, int y_coord, int *curr_ui) {
	
}


void handle_homescreent_touch (x_coord, y_coord) {

}
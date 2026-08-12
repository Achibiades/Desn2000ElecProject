#include "../Headers/stm32f103xe.h"
#include "../LCD/ili9341.h"
#include "../LCD/lcd_hw.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/motion.h"
#include "../Headers/SystemInit.h"
#include "../Headers/draw_screens.h"
#include "../Headers/rtc.h"
#include <stdio.h>

// Forward declarations -- definitions live underneath main
void check_alarm(char *time_str, int alarm_hour, int alarm_minute,
                 int *alarm_enable, int curr_ui);
void render_screen(int curr_ui, int alarm_hour, int alarm_minute, int alarm_enable);
void handle_touch(int x_coord, int y_coord, int *curr_ui,
                  int *alarm_hour, int *alarm_minute,
                  int *alarm_enable, int *wakeup_sunrise);
void handle_home_touch(int x_coord, int y_coord, int *curr_ui);
void handle_alarm_touch(int x_coord, int y_coord, int *curr_ui,
                        int *alarm_hour, int *alarm_minute,
                        int *alarm_enable, int *wakeup_sunrise);

int main() {

    int x_coord = 0;
    int y_coord = 0;

    int curr_ui = 0;
    int prev_ui = -1; // -1 so the home screen draws on the first loop
    int was_touched = 0;

    int alarm_hour = 7;
    int alarm_minute = 30;
    int alarm_enable = 0;
    int wakeup_sunrise = 0;

    int inside = 0;   // 0 = outside, 1 = inside

    char time_str[6];
    char date_str[6];

    lcd_init();
    spi_init();
    rtc_init();
    lock_led_init();
    motion_init();

    while (1) {

        get_real_time(time_str, date_str);
        motion_update(&inside);
        check_alarm(time_str, alarm_hour, alarm_minute, &alarm_enable, curr_ui);

        if (curr_ui != prev_ui) {
            render_screen(curr_ui, alarm_hour, alarm_minute, alarm_enable);
            prev_ui = curr_ui;
        }

        if (touch_read_xy(&x_coord, &y_coord) == 1) {
            //x_coord = map_pixel(x_coord, 10, 234, 0, 240);
            //y_coord = map_pixel(y_coord, 0, 250, 0, 320);

            // Only process the touch once per press
            if (was_touched == 0) {
                was_touched = 1;
                handle_touch(x_coord, y_coord, &curr_ui,
                             &alarm_hour, &alarm_minute,
                             &alarm_enable, &wakeup_sunrise);
            }
        } else {
            was_touched = 0;
        }
    }

    return 0;
}


void check_alarm(char *time_str, int alarm_hour, int alarm_minute,
                 int *alarm_enable, int curr_ui) {
    if (*alarm_enable) {
        if (check_time(time_str, alarm_hour, alarm_minute)) {
            // play_alarm_sound();
            *alarm_enable = 0;
            if (curr_ui == 4) {
                draw_alarm_state(*alarm_enable);
            }
        }
    }
}


// 0=Home, 1=Coffee, 2=Light, 3=Temp, 4=Alarm, 5=Security, 6=Sound
void render_screen(int curr_ui, int alarm_hour, int alarm_minute, int alarm_enable) {
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
        draw_alarm_time(alarm_hour, alarm_minute, 60, 104);
        draw_alarm_state(alarm_enable);
    } else if (curr_ui == 5) {
        draw_lock_screen();
    } else if (curr_ui == 6) {
        // draw_sound_screen();
    }
}


void handle_touch(int x_coord, int y_coord, int *curr_ui,
                  int *alarm_hour, int *alarm_minute,
                  int *alarm_enable, int *wakeup_sunrise) {

    if (*curr_ui == 0) {
        handle_home_touch(x_coord, y_coord, curr_ui);

    } else if (*curr_ui == 1) {
        if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
            *curr_ui = 0;
        } else {
            handle_coffee_touch(x_coord, y_coord);
        }

    } else if (*curr_ui == 2) {
        if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
            *curr_ui = 0;
        } else {
            //*curr_ui = light_logic();
        }

    } else if (*curr_ui == 3) {
        if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
            *curr_ui = 0;
        } else {
            //*curr_ui = temp_logic();
        }

    } else if (*curr_ui == 4) {
        handle_alarm_touch(x_coord, y_coord, curr_ui,
                           alarm_hour, alarm_minute,
                           alarm_enable, wakeup_sunrise);

    } else if (*curr_ui == 5) {
        if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
            *curr_ui = 0;
        } else {
            handle_lock_touch(x_coord, y_coord);
        }

    } else if (*curr_ui == 6) {
        if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
            *curr_ui = 0;
        } else {
            //*curr_ui = sound_logic();
        }
    }
}


void handle_home_touch(int x_coord, int y_coord, int *curr_ui) {
    char str[32];

    if (touch_rect(20, 50, 110, 120, x_coord, y_coord)) {          // Coffee
        *curr_ui = 1;
    } else if (touch_rect(20, 140, 110, 210, x_coord, y_coord)) {  // Light
        *curr_ui = 2;
    } else if (touch_rect(20, 230, 110, 300, x_coord, y_coord)) {  // Temp
        *curr_ui = 3;
    } else if (touch_rect(130, 50, 220, 120, x_coord, y_coord)) {  // Alarm
        *curr_ui = 4;
    } else if (touch_rect(130, 140, 220, 210, x_coord, y_coord)) { // Security
        *curr_ui = 5;
    } else if (touch_rect(130, 230, 220, 300, x_coord, y_coord)) { // Sound
        *curr_ui = 6;
    }

    sprintf(str, "%d, %d", x_coord, y_coord);
    lcd_fontColor(WHITE, BLACK);
    lcd_putString(10, 10, (unsigned char *)str);
}


void handle_alarm_touch(int x_coord, int y_coord, int *curr_ui,
                        int *alarm_hour, int *alarm_minute,
                        int *alarm_enable, int *wakeup_sunrise) {
    // Return button
    if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
        *curr_ui = 0;
    }
    // Hour +
    else if (touch_rect(20, 180, 60, 220, x_coord, y_coord)) {
        change_hour(alarm_hour, 1);
        draw_alarm_time(*alarm_hour, *alarm_minute, 60, 104);
    }
    // Hour -
    else if (touch_rect(70, 180, 110, 220, x_coord, y_coord)) {
        change_hour(alarm_hour, 0);
        draw_alarm_time(*alarm_hour, *alarm_minute, 60, 104);
    }
    // Minute +
    else if (touch_rect(130, 180, 170, 220, x_coord, y_coord)) {
        change_minute(alarm_minute, 1);
        draw_alarm_time(*alarm_hour, *alarm_minute, 60, 104);
    }
    // Minute -
    else if (touch_rect(180, 180, 220, 220, x_coord, y_coord)) {
        change_minute(alarm_minute, 0);
        draw_alarm_time(*alarm_hour, *alarm_minute, 60, 104);
    }
    // Wake up to sunrise toggle
    else if (touch_rect(20, 235, 220, 260, x_coord, y_coord)) {
        if (*wakeup_sunrise == 0) {
            *wakeup_sunrise = 1;
        } else {
            *wakeup_sunrise = 0;
        }
    }
    // Alarm ON
    else if (touch_rect(20, 275, 110, 305, x_coord, y_coord)) {
        *alarm_enable = 1;
        draw_alarm_state(*alarm_enable);
    }
}

#include "../Headers/stm32f103xe.h"
#include "../LCD/ili9341.h"
#include "../LCD/lcd_hw.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/motion.h"
#include "../Headers/heating.h"
#include "../Headers/SystemInit.h"
#include "../Headers/draw_screens.h"
#include "../Headers/rtc.h"
#include "../Headers/songs.h"

#include <stdio.h>

#define SHUTDOWN_TIMEOUT_S 10

// Forward declarations -- definitions live underneath main
void check_alarm(char *time_str, int alarm_hour, int alarm_minute,
                 int *alarm_enable, int curr_ui);
void render_screen(int curr_ui, int alarm_hour, int alarm_minute, int alarm_enable, int desired_temp, int wakeup_sunrise);
void handle_touch(int x_coord, int y_coord, int *curr_ui,
                  int *alarm_hour, int *alarm_minute,
                  int *alarm_enable, int *wakeup_sunrise, int *desired_temp);
void handle_home_touch(int x_coord, int y_coord, int *curr_ui);
void handle_alarm_touch(int x_coord, int y_coord, int *curr_ui,
                        int *alarm_hour, int *alarm_minute,
                        int *alarm_enable, int *wakeup_sunrise);
void shutdown_all(int *systems_active, int curr_ui);
void check_auto_shutdown(int ev, int touched, int inside_active, unsigned int now,
                         int *shutdown_armed, unsigned int *arm_time,
                         int *systems_active, int curr_ui);
void startup_all(int *systems_active, int curr_ui);

int main() {
    //char debug_str[32];
    //UI
    int x_coord = 0;
    int y_coord = 0;
	
		int time_counter = 9999; // allows 1 cycle between homescreen loading and timer appearing

    int curr_ui = 0;
    int prev_ui = -1; // -1 so the home screen draws on the first loop
    int was_touched = 0;

    // Alarm
    int alarm_hour = 7;
    int alarm_minute = 30;
    int alarm_enable = 0;

    // Alarm/Coffee
    int wakeup_sunrise = 1;

    // Occupancy
    int inside = 0;   // 0 = outside, 1 = inside
    int shutdown_armed = 0;
    unsigned int arm_time = 0;
    int systems_active = 0;

    // Climate control state
    temp_state_t temp_prev = TEMP_IDLE;
    unsigned int temp_since = 0;
    int desired_temp = 25; // Default to 25

    char time_str[6];
    char date_str[6];

    // Coffee 
    coffee_init();
    // Peripherals are brought up by SystemInit() before main runs

    while (1) {
        unsigned int now = rtc_get_epoch();

        int ev = motion_update(&inside);

        int touched = (touch_read_xy(&x_coord, &y_coord) == 1);
        //x_coord = map_pixel(x_coord, 10, 234, 0, 240);
        //y_coord = map_pixel(y_coord, 0, 250, 0, 320);

        get_real_time(time_str, date_str);

        check_auto_shutdown(ev, touched, inside, now,
                            &shutdown_armed, &arm_time, &systems_active, curr_ui);

        check_climate(now, systems_active, &temp_prev, &temp_since, desired_temp);

        check_alarm(time_str, alarm_hour, alarm_minute, &alarm_enable, curr_ui);
				ring_doorbell();

        if (curr_ui == 0 && time_counter >= 10000) {
            draw_current_time(time_str, date_str);
						time_counter = 0;
        } else if (curr_ui != 0) {
						time_counter = 9999;
				} else {
						time_counter = time_counter + 1;
				}

        check_coffee(time_str, alarm_hour, alarm_minute, wakeup_sunrise);
      
        if (curr_ui != prev_ui) {
            render_screen(curr_ui, alarm_hour, alarm_minute, alarm_enable, desired_temp, wakeup_sunrise);
            prev_ui = curr_ui;
        }

        if (touched) {
            // Only process the touch once per press
            if (was_touched == 0) {
                was_touched = 1;
                inside = 1;
                motion_mark_inside();
                handle_touch(x_coord, y_coord, &curr_ui,
                             &alarm_hour, &alarm_minute,
                             &alarm_enable, &wakeup_sunrise, &desired_temp);
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


// 0=Home, 1=Coffee, 2=Light, 3=Temp, 4=Alarm, 5=Security, 
void render_screen(int curr_ui, int alarm_hour, int alarm_minute, int alarm_enable, int desired_temp, int wakeup_sunrise) {
    if (curr_ui == 0) {
        draw_home_screen();
    } else if (curr_ui == 1) {
        draw_coffee_screen(alarm_hour, alarm_minute, wakeup_sunrise);
    } else if (curr_ui == 2) {
        draw_light_screen();
    } else if (curr_ui == 3) {
        draw_temp_screen();
        draw_desired_temp(desired_temp, 40, 98);
    } else if (curr_ui == 4) {
        draw_alarm_screen();
        draw_alarm_time(alarm_hour, alarm_minute, 60, 104);
        draw_alarm_state(alarm_enable);
    } else if (curr_ui == 5) {
        draw_lock_screen();
    }
}

// 0=Home, 1=Coffee, 2=Light, 3=Temp, 4=Alarm, 5=Security, 6=Sound
void handle_touch(int x_coord, int y_coord, int *curr_ui,
                  int *alarm_hour, int *alarm_minute,
                  int *alarm_enable, int *wakeup_sunrise, int *desired_temp) {

    if (*curr_ui == 0) {
        handle_home_touch(x_coord, y_coord, curr_ui);

    } else if (*curr_ui == 1) {
        if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
            *curr_ui = 0;
        } else {
            handle_coffee_touch(x_coord, y_coord, alarm_hour, alarm_minute, wakeup_sunrise);
        }

    } else if (*curr_ui == 2) {
        if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
            *curr_ui = 0;
        } else {
            handle_light_touch(x_coord, y_coord);
        }

    } else if (*curr_ui == 3) {
        if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {
            *curr_ui = 0;
        } else {
            handle_temp_touch(x_coord, y_coord, curr_ui, desired_temp);
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
    } 

    sprintf(str, "%d, %d", x_coord, y_coord);
    lcd_fontColor(WHITE, BLACK);
    lcd_putString(10, 10, (unsigned char *)str);
}




void shutdown_all(int *systems_active, int curr_ui) {
    // TODO:
    lock_all(curr_ui);
    heating_off();
    // lights_off();
    *systems_active = 0;
}


// Arms a 5-min countdown when an exit is registered. Any inside activity
// (a touch or the inside sensor being active) cancels it outright, so the
// countdown only completes if nobody is detected inside until it elapses.
void check_auto_shutdown(int ev, int touched, int inside, unsigned int now,
    int *shutdown_armed, unsigned int *arm_time,
    int *systems_active, int curr_ui) {

    // A fresh exit arms the countdown and wins outright this pass
    if (ev == MOTION_EXIT) {
        *shutdown_armed = 1;
        *arm_time = now;
    }
    // Otherwise, inside activity cancels a pending countdown
    else if (touched || inside) {
        *shutdown_armed = 0;
        if (*systems_active == 0) {
            startup_all(systems_active, curr_ui);
        }
    }

    if (*shutdown_armed && (now - *arm_time >= SHUTDOWN_TIMEOUT_S)) {
        shutdown_all(systems_active, curr_ui);
        *shutdown_armed = 0;
    }
}

void startup_all(int *systems_active, int curr_ui) {
    // TODO:
    unlock_all(curr_ui);
    heating_on();
    // lights_on();
    *systems_active = 1;
}

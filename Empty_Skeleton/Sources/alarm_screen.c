#include "../Headers/stm32f103xe.h"
#include "../LCD/lcd_grph.h"
#include "../LCD/lcd_hw.h"
#include <stdio.h>
#include <string.h>
#include "../Headers/draw_screens.h"

void draw_alarm_screen() {
    lcd_fillScreen(WHITE);

    // Return button
    lcd_fillRect(10, 10, 70, 40, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(16, 21, (unsigned char *)"return");

    // Alarm state readout, top-right
    lcd_fontColor(BLACK, WHITE);
    lcd_putString(110, 21, (unsigned char *)"Alarm State: ");

    // Title
    lcd_putString_scaled(60, 55, (unsigned char *)"WAKE UP TIME", 2);

    // Time readout
    lcd_fillRect(30, 90, 210, 150, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    //lcd_putString_scaled(36, 104, (unsigned char *)"07 : 30", 4);

    // Hour annd Min labels
    lcd_fontColor(BLACK, WHITE);
    lcd_putString(49, 161, (unsigned char *)"HOUR");
    lcd_putString(163, 161, (unsigned char *)"MIN");

    // Hour block
    lcd_fillRect(20, 180, 60, 220, LIGHT_GRAY);   /* hour + */
    lcd_fillRect(70, 180, 110, 220, LIGHT_GRAY);  /* hour - */
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString_scaled(32, 192, (unsigned char *)"+", 2);
    lcd_putString_scaled(82, 192, (unsigned char *)"-", 2);

    // Minute block
    lcd_fillRect(130, 180, 170, 220, LIGHT_GRAY);  /* min + */
    lcd_fillRect(180, 180, 220, 220, LIGHT_GRAY);  /* min - */
    lcd_putString_scaled(142, 192, (unsigned char *)"+", 2);
    lcd_putString_scaled(192, 192, (unsigned char *)"-", 2);

    // Wake up to sunrise
    lcd_fillRect(20, 235, 220, 260, LIGHT_GRAY);
    lcd_putString(60, 243, (unsigned char *)"WAKE UP TO SUNRISE");

    // Alarm ON / OFF buttons
    lcd_fillRect(20, 275, 110, 305, LIGHT_GRAY);
    lcd_putString(40, 285, (unsigned char *)"Alarm ON");

    lcd_fillRect(130, 275, 220, 305, LIGHT_GRAY);
    lcd_putString(146, 285, (unsigned char *)"Alarm OFF");
}


void change_minute(int *alarm_minute, int change)
{
    if (change == 1) {
        // +5 logic
        *alarm_minute += 5;

        if (*alarm_minute > 55) {
            *alarm_minute = 55;
            // prevent go to 60
        }

    } else {
        // -5 logic
        *alarm_minute -= 5;

        if (*alarm_minute < 0) {
            *alarm_minute = 0;
            // prevent go below 0
        }
    }
}

void change_hour(int *alarm_hour, int change) {
    if (change == 1) {
        *alarm_hour += 1;

        if (*alarm_hour > 23) {
            *alarm_hour = 23;
        }
    } else {
        *alarm_hour -= 1;
        if (*alarm_hour < 0) {
            *alarm_hour = 0;
        }
    }
}

void draw_alarm_time(int hour, int minute, int x, int y) {
    char alarm_time[6];
    snprintf(alarm_time, 6, "%02d:%02d", hour, minute);

    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString_scaled(x, y, (unsigned char *)alarm_time, 4);
}

// Redraws the ON/OFF value next to the "Alarm State:" label.
void draw_alarm_state(int alarm_enable) {
    lcd_fillRect(205, 15, 235, 30, WHITE);

    lcd_fontColor(BLACK, WHITE);

    if (alarm_enable) {
        lcd_putString(205, 21, (unsigned char *)"ON");

    } else {
        lcd_putString(205, 21, (unsigned char *)"OFF");

    }
}

// Compares the current time to the set alarm time
int check_time(char *time_str, int hour, int minute) {
    char alarm_time[6];

    snprintf(alarm_time, 6, "%02d:%02d", hour, minute);
    // get time into string format so we can compare it.

    return !strcmp(time_str, alarm_time); // compare the string and send it back
}

// handles alarm touch logic
void handle_alarm_touch(int x_coord, int y_coord, int *curr_ui,
                        int *alarm_hour, int *alarm_minute,
                        int *alarm_enable, int *wakeup_sunrise) {

    if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {             // Return
        *curr_ui = 0;
    } else if (touch_rect(20, 180, 60, 220, x_coord, y_coord)) {    // Hour +
        change_hour(alarm_hour, 1);
        draw_alarm_time(*alarm_hour, *alarm_minute, 60, 104);
    } else if (touch_rect(70, 180, 110, 220, x_coord, y_coord)) {   // Hour -
        change_hour(alarm_hour, 0);
        draw_alarm_time(*alarm_hour, *alarm_minute, 60, 104);
    } else if (touch_rect(130, 180, 170, 220, x_coord, y_coord)) {  // Minute +
        change_minute(alarm_minute, 1);
        draw_alarm_time(*alarm_hour, *alarm_minute, 60, 104);
    } else if (touch_rect(180, 180, 220, 220, x_coord, y_coord)) {  // Minute -
        change_minute(alarm_minute, 0);
        draw_alarm_time(*alarm_hour, *alarm_minute, 60, 104);
    } else if (touch_rect(20, 235, 220, 260, x_coord, y_coord)) {   // Sunrise toggle
        if (*wakeup_sunrise == 0) {
            *wakeup_sunrise = 1;
        } else {
            *wakeup_sunrise = 0;
        }
    } else if (touch_rect(20, 275, 110, 305, x_coord, y_coord)) {   // Alarm ON
        *alarm_enable = 1;
        draw_alarm_state(*alarm_enable);
    } else if (touch_rect(130, 275, 220, 305, x_coord, y_coord)) {  // Alarm OFF
        *alarm_enable = 0;
        draw_alarm_state(*alarm_enable);
    }
}
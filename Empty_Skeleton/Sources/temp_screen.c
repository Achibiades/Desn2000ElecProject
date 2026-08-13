#include "../Headers/stm32f103xe.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"

// Static layout for the temperature adjustment screen
void draw_temp_screen(void) {
    lcd_fillScreen(WHITE);

    // Return button (top-left)
    lcd_fillRect(10, 10, 70, 40, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(16, 21, (unsigned char *)"return");

    lcd_fontColor(BLACK, WHITE);
    lcd_putString_scaled(16, 55, (unsigned char *)"SET TARGET TEMP", 2);

    lcd_fillRect(30, 90, 210, 160, LIGHT_GRAY);

    // Plus (+) button
    lcd_fillRect(30, 180, 110, 230, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString_scaled(62, 195, (unsigned char *)"+", 3);

    // Minus (-) button
    lcd_fillRect(130, 180, 210, 230, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString_scaled(162, 195, (unsigned char *)"-", 3);
}

// Dynamically draws/updates the desired temperature value inside the box
void draw_desired_temp(int temp, int x, int y) {
    char temp_str[8];

    snprintf(temp_str, sizeof(temp_str), "%2d C", temp);

    // Clear inner box and redraw value
    lcd_fillRect(x, y, x + 160, y + 55, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString_scaled(x + 20, y + 12, (unsigned char *)temp_str, 4);
}

// Modifies the desired temperature value within a safe range (10C - 30C)
void change_temp(int *desired_temp, int change) {
    if (change == 1) {
        *desired_temp += 1;
        if (*desired_temp > 30) {
            *desired_temp = 30;
        }
    } else {
        *desired_temp -= 1;
        if (*desired_temp < 10) {
            *desired_temp = 10;
        }
    }
}

// Processes touch events specific to the temperature screen
void handle_temp_touch(int x_coord, int y_coord, int *curr_ui, int *desired_temp) {
    if (touch_rect(10, 10, 70, 40, x_coord, y_coord)) {             // Return
        *curr_ui = 0;
    } else if (touch_rect(30, 180, 110, 230, x_coord, y_coord)) {    // Temp +
        change_temp(desired_temp, 1);
        draw_desired_temp(*desired_temp, 40, 98);
    } else if (touch_rect(130, 180, 210, 230, x_coord, y_coord)) {   // Temp -
        change_temp(desired_temp, 0);
        draw_desired_temp(*desired_temp, 40, 98);
    }
}
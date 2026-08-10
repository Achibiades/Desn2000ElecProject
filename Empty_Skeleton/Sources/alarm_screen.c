#include "../Headers/stm32f103xe.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"

void draw_alarm_screen() {
    lcd_fillScreen(WHITE);

    /* Return button */
    lcd_fillRect(10, 10, 70, 40, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(16, 20, (unsigned char *)"return");

    /* Title -- 2x scale */
    lcd_fontColor(BLACK, WHITE);
    lcd_putString_scaled(48, 55, (unsigned char *)"WAKE UP TIME", 2);

    /* Time readout -- 4x scale */
    lcd_fillRect(30, 90, 210, 150, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString_scaled(36, 104, (unsigned char *)"07 : 30", 4);

    /* Column labels */
    lcd_fontColor(BLACK, WHITE);
    lcd_putString(40, 160, (unsigned char *)"HOUR");
    lcd_putString(165, 160, (unsigned char *)"MIN");

    /* Hour +/- tiles -- 2x scale symbols */
    lcd_fillRect(20, 170, 105, 220, LIGHT_GRAY);
    lcd_fillRect(20, 230, 105, 280, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString_scaled(57, 187, (unsigned char *)"+", 2);
    lcd_putString_scaled(57, 247, (unsigned char *)"-", 2);

    /* Minute +/- tiles -- 2x scale symbols */
    lcd_fillRect(135, 170, 220, 220, LIGHT_GRAY);
    lcd_fillRect(135, 230, 220, 280, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString_scaled(172, 187, (unsigned char *)"+", 2);
    lcd_putString_scaled(172, 247, (unsigned char *)"-", 2);

    /* Wake up to sunrise toggle */
    lcd_fillRect(20, 290, 220, 318, LIGHT_GRAY);
    lcd_fontColor(BLACK, LIGHT_GRAY);
    lcd_putString(63, 300, (unsigned char *)"WAKE UP TO SUNRISE");
}

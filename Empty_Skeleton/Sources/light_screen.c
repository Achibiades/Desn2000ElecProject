#include "../Headers/stm32f103xe.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"

void draw_light_screen() {
    lcd_fillScreen(WHITE);
    lcd_fillRect(30, 20, 50,40, CYAN);
    lcd_fontColor(BLACK, CYAN);
    lcd_putString(30, 30, (unsigned char *)"Return");

    lcd_fillRect(20, 50, 110, 120, LIGHT_GRAY);
    lcd_fillRect(20, 140, 110, 210, LIGHT_GRAY);
    lcd_fillRect(20, 230, 110, 300, LIGHT_GRAY);
    lcd_fillRect(130, 50, 220, 120, LIGHT_GRAY);
    lcd_fillRect(130, 140, 220, 210, LIGHT_GRAY);
    lcd_fillRect(130, 230, 220, 300, LIGHT_GRAY);

    lcd_fontColor(BLACK, WHITE);
    lcd_putString(30, 30, (unsigned char *)"Light");

    lcd_fontColor(BLACK, LIGHT_GRAY);
	lcd_putString(20, 50, (unsigned char *)"COFFEE");
    lcd_putString(20, 140, (unsigned char *)"LIGHT");
    lcd_putString(20, 230, (unsigned char *)"TEMP");
    lcd_putString(130, 50, (unsigned char *)"ALARM");
    lcd_putString(130, 140, (unsigned char *)"LOCK");
    lcd_putString(130, 230, (unsigned char *)"SOUND");

}
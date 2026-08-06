#include "../Headers/stm32f103xe.h"
#include "../LCD/lcd_grph.h"
#include "../Headers/draw_screens.h"

void draw_home_screen() {
    lcd_fillScreen(WHITE);
    lcd_fillRect(20, 50, 110, 120, LIGHT_GRAY);
    lcd_fillRect(20, 140, 110, 210, LIGHT_GRAY);
    lcd_fillRect(20, 230, 110, 300, LIGHT_GRAY);
    lcd_fillRect(130, 50, 220, 120, LIGHT_GRAY);
    lcd_fillRect(130, 140, 220, 210, LIGHT_GRAY);
    lcd_fillRect(130, 230, 220, 300, LIGHT_GRAY);

    lcd_fontColor(BLACK, WHITE);
    lcd_putString(90, 30, (unsigned char *)"Welcome GUEST!");

    lcd_fontColor(BLACK, LIGHT_GRAY);
	lcd_putString(45, 80, (unsigned char *)"COFFEE");
    lcd_putString(45, 170, (unsigned char *)"LIGHT");
    lcd_putString(45, 260, (unsigned char *)"TEMP");
    lcd_putString(155, 80, (unsigned char *)"ALARM");
    lcd_putString(155, 170, (unsigned char *)"LOCK");
    lcd_putString(155, 260, (unsigned char *)"SOUND");

}
#include "../Headers/stm32f103xe.h"
#include "../LCD/lcd_grph.h"

void draw_home_screen() {
    lcd_fillScreen(WHITE);
    lcd_fillRect(20, 50, 110, 120, LIGHT_GRAY);
    lcd_fillRect(20, 140, 110, 210, LIGHT_GRAY);
    lcd_fillRect(20, 230, 110, 300, LIGHT_GRAY);
    lcd_fillRect(130, 50, 220, 120, LIGHT_GRAY);
    lcd_fillRect(130, 140, 220, 210, LIGHT_GRAY);
    lcd_fillRect(130, 230, 220, 300, LIGHT_GRAY);

    lcd_putString(30, 30, (unsigned char *)"Hello ACHI!");

	lcd_putString(20, 50, (unsigned char *)"Hello ACHI!");
    lcd_putString(20, 140, (unsigned char *)"Hello ACHI!");
    lcd_putString(20, 230, (unsigned char *)"Hello ACHI!");
    lcd_putString(130, 50, (unsigned char *)"Hello ACHI!");
    lcd_putString(130, 140, (unsigned char *)"Hello ACHI!");
    lcd_putString(130, 230, (unsigned char *)"Hello ACHI!");

}
#include "../Headers/stm32f103xe.h"
#include "../LCD/ili9341.h"
#include "../LCD/lcd_hw.h"
#include "../LCD/lcd_grph.h"
#include <stdio.h> // Required for sprintf()
#include "../Headers/SystemInit.h"

// ==========================================
// FUNCTION PROTOTYPES
// ==========================================


// ==========================================
// MAIN FUNCTION
// ==========================================
/* int main(void) {
    // 1. Hardware Initialization
    lcd_init();
    spi_init();

    // Clear the screen to a solid background so text is readable
    lcd_fillScreen(BLACK);

    // 2. State Variables
    int x_coord = 0;
    int y_coord = 0;

    // 3. Infinite Polling Loop
    while (1) {

        // Read raw coordinates first
        if (touch_read_xy(&x_coord, &y_coord) == 1) {

            // Map the coordinates ONLY when a new touch is detected
            x_coord = map_pixel(x_coord, 10, 234, 0, 320);
            y_coord = map_pixel(y_coord, 15, 240, 0, 240);

            // Print the coordinates to the screen in real-time
            display_touch_coordinates(x_coord, y_coord);
        }
    }

    return 0;
}
*/
void spi_init() {
	// Enable SPI1 clock and the GPIO ports it uses
	// Port A: SCK, MISO, MOSI. Port C: CS0..5. Port E: PE5 = touch_IRQ
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;

	// Clear pre existing GPIO config
	GPIOA->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5);
	GPIOA->CRL &= ~(GPIO_CRL_CNF6 | GPIO_CRL_MODE6);
	GPIOA->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_MODE7);

	// Set GPIO pins
	// SCLK: alternate function push pull 10 11
	// MISO: input floating / input pull up - 01 00
	// MOSI: alternate function push pull - 10 11
	GPIOA->CRL |= GPIO_CRL_CNF5_1 | GPIO_CRL_MODE5;
	GPIOA->CRL |= GPIO_CRL_CNF6_0;
	GPIOA->CRL |= GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7;

	// Clear PC0..5 config
	GPIOC->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0 |
                GPIO_CRL_CNF1 | GPIO_CRL_MODE1 |
                GPIO_CRL_CNF2 | GPIO_CRL_MODE2 |
                GPIO_CRL_CNF3 | GPIO_CRL_MODE3 |
                GPIO_CRL_CNF4 | GPIO_CRL_MODE4 |
                GPIO_CRL_CNF5 | GPIO_CRL_MODE5);

	// Set PC0..5 as general purpose outputs 00 11
	GPIOC->CRL |= (GPIO_CRL_MODE0_0 | GPIO_CRL_MODE0_1) |
              (GPIO_CRL_MODE1_0 | GPIO_CRL_MODE1_1) |
              (GPIO_CRL_MODE2_0 | GPIO_CRL_MODE2_1) |
              (GPIO_CRL_MODE3_0 | GPIO_CRL_MODE3_1) |
              (GPIO_CRL_MODE4_0 | GPIO_CRL_MODE4_1) |
              (GPIO_CRL_MODE5_0 | GPIO_CRL_MODE5_1);

	// Set all CS to high (active low)
	GPIOC->BSRR = GPIO_BSRR_BS0 | GPIO_BSRR_BS1 | GPIO_BSRR_BS2 | GPIO_BSRR_BS3 | GPIO_BSRR_BS4 | GPIO_BSRR_BS5;

	// Clear PE5 config
	GPIOE->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5);

	// Set PE5 to floating input 01 00
	GPIOE->CRL |= GPIO_CRL_CNF5_0;

	// Disable SPI CR1
	SPI1->CR1 &= ~SPI_CR1_SPE;

	// Set baud rate to f_pclk / 256
	SPI1->CR1 |= SPI_CR1_BR;

	// Select CPOL and CPHA bits to define rising edge, idle low SCK
	// Leave as 0

	// Set data frame format (DFF) bit to define 8 bit
	// Leave as 0

	// configure LSBFIRST to define the frame format as MSB first
	// Leave as 0

	// Disable software management
	// Leave as 0

	// Set MSTR
	SPI1->CR1 |= SPI_CR1_MSTR;

	// Set software slave management and internal slave select
	SPI1->CR1 |= SPI_CR1_SSM;
	SPI1->CR1 |= SPI_CR1_SSI;

	// Enable SPI CR1
	SPI1->CR1 |= SPI_CR1_SPE;
}


unsigned char touch_read(unsigned char command) {
    unsigned char result;

    // Enable CS5 for TSC2046
    GPIOC->BSRR = GPIO_BSRR_BR5;

    // Send command byte and wait, then read dummy result
    SPI1->DR = command;
    while (!(SPI1->SR & SPI_SR_RXNE));
    result = SPI1->DR;

    // Send dummy byte and read true result
    SPI1->DR = 0x00;
    while (!(SPI1->SR & SPI_SR_RXNE));
    result = SPI1->DR;

    // Wait until SPI is free
    while (SPI1->SR & SPI_SR_BSY);

    // Disable CS5
    GPIOC->BSRR = GPIO_BSRR_BS5;

    return (result << 1);
}

unsigned short map_pixel(unsigned short x, short in_min, short in_max, short out_min, short out_max) {
    unsigned short result = (x - in_min) * (out_max - out_min) / (in_max - in_min);
    if (result < out_min) return out_min;
    if (result > out_max) return out_max;
    return result;
}

int touch_rect(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short x_num, unsigned short y_num) {
        return (x_num >= x0 && x_num <= x1 && y_num >= y0 && y_num <= y1);
}

int touch_read_xy(int *x, int *y) {

    if (GPIOE->IDR & GPIO_IDR_IDR5) { // essentially no touch.
        return 0;
    }

    //Read X co-ordinate from the touch screen controller
    *x = touch_read(0xD8);
    //Read Y co-ordinate from the touch screen controller
    *y = touch_read(0x98);

    return 1;
}

void display_touch_coordinates(int touch_x, int touch_y) {
    char coord_str[32];

    // Pad with zeros (e.g., "045") to ensure old text is completely overwritten
    sprintf(coord_str, "X:%03d Y:%03d", touch_x, touch_y);

    // Set font colors (Foreground: White, Background: Black)
    lcd_fontColor(WHITE, BLACK);

    // Print the string at a fixed location (x=10, y=10)
    lcd_putString(10, 10, (unsigned char *)coord_str);
}
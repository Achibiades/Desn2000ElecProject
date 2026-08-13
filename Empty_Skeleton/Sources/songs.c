#include "../Headers/stm32f103xe.h"
#include "../Headers/songs.h"
#include "../LCD/ili9341.h"
#include "../LCD/lcd_hw.h"
#include "../LCD/lcd_grph.h"




int song_duration[] = {10, 20, 33};



struct tone song_data[] = { 
	{2, G5, 0x3A0}, // DOORBELL BEGIN
	{1, G5, 0x1F0},
	{2, OFF, 0},
	{2, FS5, 0x3A0},
	{1, FS5, 0x1F0},
	{2, OFF, 0},
	{2, B5, 0x3A0},
	{1, B5, 0x1F0},
	{2, OFF, 0},	
	{3, A5, 0x3A0},
	{2, A5, 0x1F0},
	{10, OFF, 0}, // DOORBELL END
	{2, CS6, 0x3F0}, // song 1 start (3rd Sanctuary)
	{2, GS5, 0x3F0},
	{2, FS5, 0x3F0},
	{2, GS5, 0x3F0},
	{2, CS6, 0x3F0},
	{1, OFF, 0x3F0},
	{2, CS6, 0x3F0},
	{2, GS5, 0x3F0},
	{2, FS5, 0x3F0},
	{2, GS5, 0x3F0},
	{6, OFF, 0x3F0},
	{2, CS6, 0x3F0},
	{2, GS5, 0x3F0},
	{2, FS5, 0x3F0},
	{2, GS5, 0x3F0},
	{2, CS6, 0x3F0},
	{1, OFF, 0x3F0},
	{2, CS6, 0x3F0},
	{2, GS5, 0x3F0},
	{2, FS5, 0x3F0},
	{2, GS5, 0x3F0},
	{10, OFF, 0}, // song 1 end
	{1, DS5, 0x300}, //song 2 start (nyan cat)
	{1, DS5, 0x1A0},	
	{1, E5, 0x300},
	{1, E5, 0x1A0},
	{1, FS5, 0x300},
	{1, FS5, 0x1A0},
	{2, OFF, 0},
	{1, B5, 0x300},
	{1, B5, 0x1A0},
	{2, OFF, 0},
	{1, DS5, 0x300},
	{1, DS5, 0x1A0},
	{1, E5, 0x300},
	{1, E5, 0x1A0},
	{1, FS5, 0x300},
	{1, FS5, 0x1A0},
	{1, B5, 0x300},
	{1, B5, 0x1A0},
	{1, CS6, 0x300},
	{1, CS6, 0x1A0},
	{1, DS6, 0x300},
	{1, DS6, 0x1A0},
	{1, CS6, 0x300},
	{1, CS6, 0x1A0},
	{1, AS5, 0x300},
	{1, AS5, 0x1A0},
	{1, B5, 0x300},
	{1, B5, 0x1A0},
	{2, OFF, 0},
	{1, FS5, 0x300},
	{1, FS5, 0x1A0},
	{2, OFF, 0},
	{1, DS5, 0x300},
	{1, DS5, 0x1A0}
};


void udelay(unsigned int delay_in_us) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
			// Reset Timer 0
			
	TIM2->EGR |= 0x1;
			
			
			// Set Prescaler to increment T0TC every 1 microsecond
	TIM2->PSC = 0x7;
			
			// Start Timer 0
	TIM2->CR1 |= 0x1; // bit 0 is control on or off
			
			// Wait until target microsecond count is reached
 	while (TIM2->CNT < delay_in_us);
			
			// Stop Timer 0
  TIM2->CR1 &= 0xFFFFFFFE;
}
		
void play_tone(unsigned int duration, int period, int vol) {
		unsigned int elapsed = 0;
		unsigned int low = 0;

    while (elapsed < duration) {
        // High phase
			if (period == 100) {
 //       period = 1000;
				vol = 0x1FF;
				low = 0x7FF;
			}
        DAC->DHR12R1 = vol << 2;
        udelay(period/2);

        // Low phase
        DAC->DHR12R1 = low;
        udelay(period/2);

        // Accumulate time spent
        elapsed += period;
    }

}

void play_song(int song) {
	int rate;
	int i = 0;
	int j = 0;
	
	if (song == 0) {
		j = 0;
		rate = 52000; 
		
	} else if (song == 1) {
		j = 12;
		rate = 70000;
	} else if (song == 2) {
		j = 34;
		rate = 52000;
	}
	while (i <= song_duration[song]) {
		play_tone(rate * song_data[i+j].duration, song_data[i+j].pitch, song_data[i+j].volume);
		i++;
	}
};


void ring_doorbell(void) {
	if (GPIOE->IDR & GPIO_IDR_IDR1) {
		play_song(0);
	}
}

void standard_wakeup_alarm() {
	play_song(1);
}

void dawn_wakeup_alarm() {
	play_song(2);
}

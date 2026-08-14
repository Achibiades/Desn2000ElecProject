#include "../Headers/stm32f103xe.h"
#include "../Headers/adc.h"
#include "../LCD/ili9341.h"
#include "../LCD/lcd_hw.h"
#include "../LCD/lcd_grph.h"

#define LIGHT_SENSOR 1
#define TEMP_SENSOR 0
#define SUN_THRESHOLD 2048


void setupADC() {
    // clock enable at RCC_APB2ENR for ADC1 and GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN;
    GPIOA->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1);

    // ADC pre-scaler RCC_CFGR (PCLK2 / 6 = 12MHz)
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->CFGR |=  RCC_CFGR_ADCPRE_DIV6;

    ADC1->CR2 |= ADC_CR2_ADON;

    // Reset and start calibration
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while (ADC1->CR2 & ADC_CR2_RSTCAL);

    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL);

    // sampling rate Channel 1 ADC_SMPR2
    ADC1->SMPR2 |= ADC_SMPR2_SMP1_1 | ADC_SMPR2_SMP1_0;

    ADC1->CR2 &= ~ADC_CR2_ALIGN;

    // number of conversions L = 0 (1 conversion) in ADC_SQR1
    ADC1->SQR1 &= ~ADC_SQR1_L;

    // set 1st conversion sequence to Channel 1 (PA1) in ADC_SQR3
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
}


int readADC(int ADC_channel) {
	ADC1->SQR3 = 0x0;
	ADC1->SQR3 |= (1 << ADC_channel);
    ADC1->CR2 |= ADC_CR2_ADON;

    // wait for conversion to complete
    while ((ADC1->SR & ADC_SR_EOC) == 0);

    return (int)(ADC1->DR & 0xFFF);
}

int sun_is_out(void) {
	int adc_val;
	adc_val = readADC(LIGHT_SENSOR);
	if (adc_val + 1 > SUN_THRESHOLD) {
		return 1;
	}	else {
		return 0;
	}
}

temp_state_t temp_get_state(int target_temp) {
    int adc_val;
    int temp_c;
    int diff;
	adc_val = readADC(TEMP_SENSOR);
    temp_c  = (adc_val + 1) / 82 - 15;   // maps 0..4095 to -15..35 C

    diff = temp_c - target_temp;
    if (diff > 1)  return TEMP_COOL;    // too hot
    if (diff < -1) return TEMP_HEAT;   // too cold
    return TEMP_IDLE;                   // within 1 degree -> in range
}

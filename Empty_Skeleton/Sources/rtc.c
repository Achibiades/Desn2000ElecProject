#include "../Headers/stm32f103xe.h"
#include "../Headers/rtc.h"
#include "../LCD/ili9341.h"
#include "../LCD/lcd_hw.h"
#include "../LCD/lcd_grph.h"
#include "stdio.h"
#include "time.h"

void rtc_set_time(unsigned int epoch_seconds) {
    // Make sure no earlier write is still in progress
    while (!(RTC->CRL & RTC_CRL_RTOFF))
    {
    }
    // Unlock the counter registers for writing
    RTC->CRL |= RTC_CRL_CNF;

    // Load the epoch value
    RTC->CNTH = (unsigned short)(epoch_seconds >> 16);

    RTC->CNTL = (unsigned short)(epoch_seconds & 0xFFFF);

    RTC->CRL &= ~RTC_CRL_CNF;

    while (!(RTC->CRL & RTC_CRL_RTOFF))
    {
    }
}

void rtc_init() {
    // Power and Backup domain clocks must be on before touching BKP/RTC
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    RCC->APB1ENR |= RCC_APB1ENR_BKPEN;

    PWR->CR |= PWR_CR_DBP;

    // A known DR1 survives soft reset, don't run init again.
    if (BKP->DR1 != 0xA5A5) {
        RCC->BDCR |= RCC_BDCR_LSEON;
        while (!(RCC->BDCR & RCC_BDCR_LSERDY))
        {
        }

        // Point at the LSE source
        RCC->BDCR &= ~RCC_BDCR_RTCSEL;
        RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;

        // Turn peripheral clock on
        RCC->BDCR |= RCC_BDCR_RTCEN;

        RTC->CRL &= ~RTC_CRL_RSF;
        while (!(RTC->CRL & RTC_CRL_RSF))
        {
        }

        while (!(RTC->CRL & RTC_CRL_RTOFF))
        {
        }

        // Open the configuration window
        RTC->CRL |= RTC_CRL_CNF;

        // set pre scaler for a 1 Hz tick
        RTC->PRLH = (unsigned short)0x0000;
        RTC->PRLL = (unsigned short)0x7FFF;

        // wait for it to apply
        RTC->CRL &= ~RTC_CRL_CNF;
        while (!(RTC->CRL & RTC_CRL_RTOFF))
        {
        }

        // current epoch value from online.
        rtc_set_time(1786674652);

        // Record that setup has run so future resets skip this branch
        BKP->DR1 = 0xA5A5;
    } else {
        // Already configured
        RTC->CRL &= ~RTC_CRL_RSF;
        while (!(RTC->CRL & RTC_CRL_RSF))
        {
        }

        while (!(RTC->CRL & RTC_CRL_RTOFF))
        {
        }
    }
}

void epoch_to_strings(unsigned int epoch, char *time_out, char *date_out) {
    unsigned int day;
    unsigned int month;
    unsigned int doy;
    unsigned int mp;
    unsigned int y;
    unsigned int yoe;
    unsigned int doe;
    unsigned int era;
    unsigned int days;
    unsigned int seconds;
    unsigned int minutes;
    unsigned int hours;
    unsigned int seconds_in_day;

    // Split the epoch into a day count, hours min and secs
    seconds_in_day = epoch % 86400;
    hours   = seconds_in_day / 3600;
    minutes = (seconds_in_day % 3600) / 60;
    seconds = seconds_in_day % 60;

    days = epoch / 86400;

    // convert algorithm
    days += 719468;

    era = days / 146097;

    doe = days - era * 146097;

    yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;

    y   = yoe + era * 400;

    doy = doe - (365 * yoe + yoe / 4 - yoe / 100);

    mp  = (5 * doy + 2) / 153;

    day = doy - (153 * mp + 2) / 5 + 1;

    // mp have offset so +3 or - 9
    if (mp < 10)
    {
        month = mp + 3;
    }
    else
    {
        month = mp - 9;
    }

    // write the string
    sprintf(time_out, "%02u:%02u", hours, minutes);
    sprintf(date_out, "%02u/%02u", day, month);
}

void get_real_time(char *time_str, char *date_str) {
    unsigned int real_time;

    // Force a resync
    RTC->CRL &= ~RTC_CRL_RSF;

    while (!(RTC->CRL & RTC_CRL_RSF))
    {
    }

    real_time = ((unsigned int)RTC->CNTH << 16) | (unsigned int)(RTC->CNTL & 0xFFFF);

    epoch_to_strings(real_time, time_str, date_str);
}

unsigned int rtc_get_epoch(void) {
    RTC->CRL &= ~RTC_CRL_RSF;

    while (!(RTC->CRL & RTC_CRL_RSF)) {}


    return ((unsigned int)RTC->CNTH << 16) | (RTC->CNTL & 0xFFFF);
}

void draw_current_time(char *time_str, char *date_str) {
    lcd_fillRect(130, 230, 220, 300, LIGHT_GRAY);

    lcd_fontColor(BLACK, LIGHT_GRAY);

    lcd_putString_scaled(145, 245, (unsigned char *)time_str, 2);

    lcd_putString(150, 275, (unsigned char *)date_str);
}
#include "../Headers/stm32f103xe.h"
#include "../Headers/rtc.h"
#include "../LCD/ili9341.h"
#include "../LCD/lcd_hw.h"
#include "../LCD/lcd_grph.h"
#include "stdio.h"
#include "time.h"

void rtc_set_time(unsigned int epoch_seconds)
{
    // Make sure no earlier write is still in progress
    while (!(RTC->CRL & RTC_CRL_RTOFF))
    {
    }

    // Unlock the counter registers for writing
    RTC->CRL |= RTC_CRL_CNF;

    // Load the 32-bit epoch value across the two 16-bit halves
    RTC->CNTH = (unsigned short)(epoch_seconds >> 16);
    RTC->CNTL = (unsigned short)(epoch_seconds & 0xFFFF);

    // Lock the registers again to commit the new value
    RTC->CRL &= ~RTC_CRL_CNF;

    // Block until the counter has taken the new value
    while (!(RTC->CRL & RTC_CRL_RTOFF))
    {
    }
}

void rtc_init()
{
    // Power and Backup domain clocks must be on before touching BKP/RTC
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    RCC->APB1ENR |= RCC_APB1ENR_BKPEN;

    // Lift write protection on the backup domain registers
    PWR->CR |= PWR_CR_DBP;

    // A known magic value in DR1 means the RTC survived a soft reset
    // and doesn't need to be re-initialised from scratch
    if (BKP->DR1 != 0xA5A5)
    {
        // Start the 32.768 kHz external crystal and wait for lock
        RCC->BDCR |= RCC_BDCR_LSEON;
        while (!(RCC->BDCR & RCC_BDCR_LSERDY))
        {
        }

        // Point the RTC clock mux at the LSE source
        RCC->BDCR &= ~RCC_BDCR_RTCSEL;
        RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;

        // Turn the RTC peripheral clock on
        RCC->BDCR |= RCC_BDCR_RTCEN;

        // Force a resync of the shadow registers before reading/writing
        RTC->CRL &= ~RTC_CRL_RSF;
        while (!(RTC->CRL & RTC_CRL_RSF))
        {
        }

        // Wait for the peripheral to be idle before reconfiguring it
        while (!(RTC->CRL & RTC_CRL_RTOFF))
        {
        }

        // Open the configuration window
        RTC->CRL |= RTC_CRL_CNF;

        // 20-bit prescaler split across PRLH/PRLL, set for a 1 Hz tick
        RTC->PRLH = (unsigned short)0x0000;
        RTC->PRLL = (unsigned short)0x7FFF;

        // Initial counter value is set separately below via rtc_set_time()
        // RTC->CNTH = (unsigned short)(1785034140UL >> 16);
        // RTC->CNTL = (unsigned short)(1785034140UL & 0xFFFF);

        // Close the configuration window and wait for it to apply
        RTC->CRL &= ~RTC_CRL_CNF;
        while (!(RTC->CRL & RTC_CRL_RTOFF))
        {
        }

        // Seed the counter with a known epoch timestamp
        rtc_set_time(1785034140);

        // Record that setup has run so future resets skip this branch
        BKP->DR1 = 0xA5A5;
    }
    else
    {
        // Already configured -- just make sure the shadow registers
        // are back in sync before anything reads from them
        RTC->CRL &= ~RTC_CRL_RSF;
        while (!(RTC->CRL & RTC_CRL_RSF))
        {
        }

        while (!(RTC->CRL & RTC_CRL_RTOFF))
        {
        }
    }
}

void epoch_to_strings(unsigned int epoch, char *time_out, char *date_out)
{
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

    // Split the epoch into a day count and a within-day remainder
    seconds_in_day = epoch % 86400;
    hours   = seconds_in_day / 3600;
    minutes = (seconds_in_day % 3600) / 60;
    seconds = seconds_in_day % 60;

    days = epoch / 86400;

    // Howard Hinnant's civil_from_days algorithm converts a day count
    // (relative to the Unix epoch) into a Gregorian day/month/year
    days += 719468;
    era = days / 146097;
    doe = days - era * 146097;
    yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    y   = yoe + era * 400;
    doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    mp  = (5 * doy + 2) / 153;

    day = doy - (153 * mp + 2) / 5 + 1;

    // mp is a "shifted" month index (0 = March ... 11 = February);
    // convert it back to a normal 1-12 January-based month number
    if (mp < 10)
    {
        month = mp + 3;
    }
    else
    {
        month = mp - 9;
    }

    // Format for display -- seconds are tracked internally but not shown
    sprintf(time_out, "%02u:%02u", hours, minutes);
    sprintf(date_out, "%02u/%02u", day, month);
}

void get_real_time(char *time_str, char *date_str)
{
    unsigned int real_time;

    // Force a resync so CNTH/CNTL reflect the live counter, not stale values
    RTC->CRL &= ~RTC_CRL_RSF;
    while (!(RTC->CRL & RTC_CRL_RSF))
    {
    }

    real_time = ((unsigned int)RTC->CNTH << 16) | (unsigned int)(RTC->CNTL & 0xFFFF);
    epoch_to_strings(real_time, time_str, date_str);
}
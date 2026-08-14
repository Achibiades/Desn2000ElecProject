# Desn2000ElecProject
A Smart Home system hub built in C and arm architecture
Melissa test

hello does this work 

RTC functions (rtc.c):
rtc_init - sets up the LSE clock and configures the RTC prescaler for a 1Hz tick. On first boot it seeds the counter with an epoch timestamp. After a soft reset it just checks the magic value in BKP DR1 and skips re-initializing since the RTC already has the right time.

rtc_set_time - writes a raw epoch value directly into the CNTH and CNTL registers.

rtc_get_epoch - resyncs the shadow registers and returns the current epoch as an unsigned int.

epoch_to_strings - converts an epoch value into HH:MM and DD/MM strings using Howard Hinnant's civil date algorithm.

get_real_time - reads the live RTC counter and formats it into those two strings.

draw_current_time - redraws the clock box on the home screen with the updated time and date.



Alarm functions (alarm_screen.c):
draw_alarm_screen - draws the static parts of the alarm screen like the buttons, labels and boxes.

draw_alarm_time - formats the alarm time as HH:MM and draws it scaled up by 4.

draw_alarm_state - clears the old ON or OFF text first, then draws the current state so nothing overlaps.

change_hour and change_minute - take a pointer and a direction, then increase or decrease the value with clamping so hour stays between 0 and 23 and minute stays between 0 and 55 in steps of 5.

check_time - compares the current time string to the formatted alarm time to see if they match.

handle_alarm_touch - checks where the screen was touched and calls the right function depending on which button was pressed.


How the screen should behave: (for alarm)
When you tap the return button it takes you back to the home screen.

When you tap hour plus or minus it changes the hour up or down by one and redraws the time box, staying within 0 to 23.

When you tap minute plus or minus it changes the minute up or down by five and redraws the time box, staying within 0 to 55.

When you tap the sunrise box it toggles wakeup_sunrise on or off but right now there's no visual change to show it happened.

When you tap alarm ON it sets alarm_enable to 1 and updates the state text to show ON.

When you tap alarm OFF it sets alarm_enable to 0 and updates the state text to show OFF.

Whenever the current time matches the alarm time, regardless of which screen you're on, the song plays and alarm_enable resets to 0. If you happen to be on the alarm screen when that happens the state text updates to OFF right away instead of waiting for you to leave and come back.


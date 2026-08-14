# Desn2000ElecProject
A Smart Home system hub built in C and arm architecture

# LIGHTING CONTROL
- Controls six rooms through touchscreen tiles.
- Blue indicates on; grey indicates off.
- TURN ALL ON activates every light.
- LED2 turns on when all six lights are active.
- Only changed tiles are redrawn.

# LOCK CONTROL
- Controls the main door and five windows.
- Red indicates locked; grey indicates unlocked.
- LOCK ALL secures every entry point.
- LED5 turns on when all six locks are engaged.
- Only changed tiles are redrawn.

# RTC functions (`rtc.c`):
rtc_init - sets up the LSE clock and configures the RTC prescaler for a 1Hz tick. On first boot it seeds the counter with an epoch timestamp. After a soft reset it just checks the magic value in BKP DR1 and skips re-initializing since the RTC already has the right time.

rtc_set_time - writes a raw epoch value directly into the CNTH and CNTL registers.

rtc_get_epoch - resyncs the shadow registers and returns the current epoch as an unsigned int.

epoch_to_strings - converts an epoch value into HH:MM and DD/MM strings using Howard Hinnant's civil date algorithm.

get_real_time - reads the live RTC counter and formats it into those two strings.

draw_current_time - redraws the clock box on the home screen with the updated time and date.



# Alarm functions (`alarm_screen.c`):
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

# Coffee control ('coffee_screen.c') 

The coffee system uses LED1 on PF1 to represent the coffee machine. 
coffee_init - configures PF1 to become an output, initially setting the coffee machine to off. This system tracks whether the coffee machine is enabled, currently running and the RTC time at which the current coffee cycle turned on.

draw_coffee_screen - draws the coffee interface, including a shared wake-up time, controls for hour and minute, WAKE UP TO SUNRISE button, Coffee ON/OFF controls and current coffee state. The variables alarm_hour, alarm_minute and wakeup_sunrise have been shared with the alarm system to simplify the overall logic.

draw_coffee_time - updates the displayed wake-up setting. When wakeup_sunrise == 0, the selected wake-up time is displayed on the board in Hour: Minute format. When wakeup_sunrise is enabled, the display shows SUNRISE.

draw_coffee_state - displays whether the coffee system is currently ON or OF. 

handle_coffee_touch - handles touchscreen controls using the touch_rect function. The hour and minute controls reuse the alarms change_hour and change_minute functions. The WAKE UP TO SUNRISE button toggles wakeup_sunrise between 0 and 1.

check_coffee - called continuously from the main loop. If coffee is disabled, LED1 remains off. In the manual wake-up mode, the check_time function compares the RTC value with the shared wake-up time. When the times match, the coffee machine LED1 turns on and the current RTC epoch is stored as coffee_start_time.

Once turned ON, the coffee machine remains running for 'COFFEE_RUN_TIME', which is set to 1800 seconds or 30 minutes. The current RTC epoch is compared with 'coffee_start_time' on each loop. Once 1800 seconds pass, the coffee machine automatically turns off, and LED1 turns off.

## Coffee screen behaviour 
- Hour +/- changes the shared wake-up hour.
- Minute +/- changes the shared wake-up minute in 5-minute steps.
- WAKE UP TO SUNRISE toggles the shared `wakeup_sunrise` setting used by both the alarm and coffee systems.
- Coffee ON enables the coffee system.
- Coffee OFF disables the coffee system and immediately turns the coffee machine off.
- At the selected manual wake-up time, LED1 turns on to represent the coffee machine running.
- The coffee machine automatically turns off 30 minutes after it starts.
- The return button takes the user back to the home screen.

## Occupancy (`motion.c`)

Two Port C switches act as motion sensors: `OUTSIDE` = PC7, `INSIDE` = PC6.
`motion_update()` checks the order they are triggered:
- **outside→inside within 5 s = ENTER**,
- **inside→outside = EXIT**

 It returns `MOTION_NONE/ENTER/EXIT` and sets the `inside` flag (1 on enter, 0 on exit).

`inside` is the occupancy. It's set to 1 by an ENTER *or any LCD touch* (touch = proof someone's home), and 0 by an EXIT.

RGB0 shows:
- **red = outside**
- **green = inside**.

## Auto-shutdown (`check_auto_shutdown`, `main.c`)

- **EXIT** arms a countdown (`shutdown_armed = 1`, `arm_time = now`).
- `touched` or `inside` **cancels** the countdown. If systems were off, runs `startup_all()`, `systems_active = 1, unlock_all() + heating_on()  + lights_on()`.
- After `SHUTDOWN_TIMEOUT_S` seconds armed, `shutdown_all()` fires, `systems_active = 0, lock_all()  + heating_off() + lights_off()`.

`SHUTDOWN_TIMEOUT_S` is currently **10 s**. `systems_active` boots at **0**, so the house starts asleep until the first inside activity.

## Climate control (`heating.c`)

**Temperature screen (UI 3):** adjusts `desired_temp` (default 25 °C) via +/- controls, which `check_climate()` uses as its target. Return button goes home.

`temp_get_state(desired_temp)` (in `adc.c`) returns `TEMP_IDLE` if the current temp is within 1 °C of the desired temp. It returns`TEMP_HEAT` (too cold), or `TEMP_COOL` (too hot).

`check_climate()` each pass:

1. If `!systems_active`: RGB1 off, blinds closed, return.
2. A state change restarts a 5 s settle window (`PASSIVE_SETTLE_S`).
3. **IDLE** → green. **HEAT** → open blinds if sunny, then active heat if still cold after 5 s. **COOL** → close blinds if sunny, then active cool if still hot after 5 s.

The principle is that blinds move first. Active heating/cooling only engages if 5 s of passive correction wasn't enough. RGB1: **red = active heating**, **blue = active cooling**, **green = at target**, **off = shut down or passive-only**.

Blinds = PF6 (high = open), moves only on change.

The `systems_active` gate stops `check_climate` re-enabling heating after a shutdown —
it works with the explicit `heating_off()` in `shutdown_all()`.


## MAIN Loop order (`main.c`)
`SystemInit()` must call `motion_init()`, `heating_init()`, and `setupADC()` before `main()`.
`motion_update()` → `check_auto_shutdown()` → `check_climate()` → touch handling.


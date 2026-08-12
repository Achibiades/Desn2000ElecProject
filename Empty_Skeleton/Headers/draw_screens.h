void draw_alarm_screen();

// Alarm Logic
void change_minute(int *alarm_minute, int change);
void change_hour(int *alarm_hour, int change);
void draw_alarm_time(int hour, int minute, int x, int y);
void draw_alarm_state(int alarm_enable);
int check_time(char *time_str, int hour, int minute);

//lock logic
void lock_led_init(void);
void draw_lock_screen(void);
void handle_lock_touch(int tx, int ty);

void draw_lock_screen();
void draw_coffee_screen();
void draw_home_screen();
void draw_temp_screen();
void draw_light_screen();
void draw_sound_screen();

void draw_alarm_screen();

// Alarm Logic
void change_minute(int *alarm_minute, int change);
void change_hour(int *alarm_hour, int change);
void draw_alarm_time(int hour, int minute, int x, int y);
void draw_alarm_state(int alarm_enable);
int check_time(char *time_str, int hour, int minute);
void draw_current_time(char *time_str, char *date_str);
void handle_alarm_touch(int x_coord, int y_coord, int *curr_ui,
                        int *alarm_hour, int *alarm_minute,
                        int *alarm_enable, int *wakeup_sunrise);

//lock logic
void lock_led_init(void);
void draw_lock_screen(void);
void handle_lock_touch(int tx, int ty);
void unlock_all();
void lock_all();

//light logic
void light_led_init(void);
void draw_light_screen(void);
void handle_light_touch(int tx, int ty);

void draw_lock_screen();
void draw_coffee_screen();
void draw_home_screen();
void draw_temp_screen();
void draw_light_screen();
void draw_sound_screen();

//temp logic
void draw_temp_screen(void);
void draw_desired_temp(int temp, int x, int y);
void change_temp(int *desired_temp, int change);
void handle_temp_touch(int x_coord, int y_coord, int *curr_ui, int *desired_temp);

//coffee logic
void coffee_init(void);
void draw_coffee_screen(int hour, int minute, int wakeup_sunrise);
void handle_coffee_touch(int tx, int ty, int *hour, int *minute, int *wakeup_sunrise);
void check_coffee(char *time_str, int hour, int minute, int wakeup_sunrise);void draw_alarm_screen();

// Alarm Logic
void change_minute(int *alarm_minute, int change);
void change_hour(int *alarm_hour, int change);
void draw_alarm_time(int hour, int minute, int x, int y);
void draw_alarm_state(int alarm_enable);
int check_time(char *time_str, int hour, int minute);
void draw_current_time(char *time_str, char *date_str);
void handle_alarm_touch(int x_coord, int y_coord, int *curr_ui,
                        int *alarm_hour, int *alarm_minute,
                        int *alarm_enable, int *wakeup_sunrise);

//lock logic
void lock_led_init(void);
void draw_lock_screen(void);
void handle_lock_touch(int tx, int ty);
void unlock_all(int curr_ui);
void lock_all(int curr_ui);

//light logic
void light_led_init(void);
void draw_light_screen(void);
void handle_light_touch(int tx, int ty);

void draw_lock_screen();
void draw_coffee_screen();
void draw_home_screen();
void draw_temp_screen();
void draw_light_screen();
void draw_sound_screen();

//temp logic
void draw_temp_screen(void);
void draw_desired_temp(int temp, int x, int y);
void change_temp(int *desired_temp, int change);
void handle_temp_touch(int x_coord, int y_coord, int *curr_ui, int *desired_temp);

//coffee logic
void coffee_init(void);
void draw_coffee_screen(int hour, int minute, int wakeup_sunrise);
void handle_coffee_touch(int tx, int ty, int *hour, int *minute, int *wakeup_sunrise);
void check_coffee(char *time_str, int hour, int minute, int wakeup_sunrise);

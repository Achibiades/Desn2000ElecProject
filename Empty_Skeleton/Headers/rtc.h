void rtc_set_time(unsigned int epoch_seconds);
void rtc_init(void);
void epoch_to_strings(unsigned int epoch, char *time_out, char *date_out);
void get_real_time(char *time_str, char *date_str);
unsigned int rtc_get_epoch(void);

#ifndef HEATING_H
#define HEATING_H

// Temperature control flag. temp_get_state() (owned by the temperature/ADC
// module) returns one of these; check_climate() acts on it.
typedef enum { TEMP_IDLE, TEMP_HEAT, TEMP_COOL } temp_state_t;
temp_state_t temp_get_state(int desired_temp);
int          sun_is_out(void);

// Configure the climate output pins
// Call once at startup, e.g. from SystemInit().
void heating_init(void);

// Prioritises passive heating/cooling (blinds) and escalates to active heating/cooling only if
// still out of range after the settle window. Gated off when systems_active
// is 0 (house shut down).
void check_climate(unsigned int now, int systems_active,
                   temp_state_t *prev_state, unsigned int *state_since, int desired_temp);
void heating_off(void);
void heating_on(void);
#endif
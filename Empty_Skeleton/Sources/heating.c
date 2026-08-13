#include "../Headers/stm32f103xe.h"
#include "../Headers/heating.h"

#define PASSIVE_SETTLE_S 5     // let blinds work 5s before active heating/cooling

//   red   = active heating engaged
//   blue  = active cooling engaged
//   green = at correct temperature
//   off   = system shut down, OR out of range but only passively correcting
#define RGB1_BLUE   (1 << 2)   // PE2
#define RGB1_RED    (1 << 3)   // PE3
#define RGB1_GREEN  (1 << 4)   // PE4

// Blinds: LED6 / PF6 (high = open).
#define BLINDS_PIN  (1 << 6)   // PF6


static void rgb1_red(void) {
    GPIOE->BSRR = RGB1_RED | ((RGB1_BLUE | RGB1_GREEN) << 16);
}
static void rgb1_blue(void) {
    GPIOE->BSRR = RGB1_BLUE | ((RGB1_RED | RGB1_GREEN) << 16);
}
static void rgb1_green(void) {
    GPIOE->BSRR = RGB1_GREEN | ((RGB1_RED | RGB1_BLUE) << 16);
}
static void rgb1_off(void) {
    GPIOE->BSRR = (RGB1_RED | RGB1_GREEN | RGB1_BLUE) << 16;
}

static int blinds_open = 0;

static void open_blinds(void) {
    if (!blinds_open) {
        GPIOF->BSRR = BLINDS_PIN;
        blinds_open = 1;
    }
}

static void close_blinds(void) {
    if (blinds_open) {
        GPIOF->BSRR = BLINDS_PIN << 16;
        blinds_open = 0;
    }
}


void heating_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;   // Port E for RGB1
    RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;   // Port F for the blinds LED

    // RGB1: PE2, PE3, PE4
    GPIOE->CRL &= ~(0x000FFF00);
    GPIOE->CRL |=  (0x00022200);

    // Blinds LED6/PF6 as output
    GPIOF->CRL &= ~(0x0F000000);
    GPIOF->CRL |=  (0x02000000);

    // Start in a known state
    rgb1_off();
    GPIOF->BSRR = BLINDS_PIN << 16;   // blinds closed
    blinds_open = 0;
}


// Prioritises passive heating/cooling (blinds) first, escalating to active
// heating/cooling only if still out of range after PASSIVE_SETTLE_S seconds.
// A state change restarts that settle window.
// IDLE stops active correction but leaves the blinds as they are.
// RGB1 shows red/blue only once active correction is engaged; during the
// passive-only window it stays off.
void check_climate(unsigned int now, int systems_active,
                   temp_state_t *prev_state, unsigned int *state_since) {

    if (!systems_active) {
        rgb1_off();
        return;
    }

    temp_state_t s = temp_get_state();

    // A state change restarts the 5s passive-settle window
    if (s != *prev_state) {
        *prev_state = s;
        *state_since = now;
    }

    if (s == TEMP_IDLE) {
        rgb1_green();
        return;
    }

    if (s == TEMP_HEAT) {
        if (sun_is_out()) {
            open_blinds();
        }

        if (now - *state_since >= PASSIVE_SETTLE_S) {   // still cold after 5s
            rgb1_red();  // active heating
        } else {
            rgb1_off();   // passive only, not engaged
        }
    } else {  // TEMP_COOL
        if (sun_is_out()) {
            close_blinds();
        }
        
        if (now - *state_since >= PASSIVE_SETTLE_S) {   // still hot after 5s
            rgb1_blue();  // active cooling engaged
        } else {
            rgb1_off();   // passive only, not engaged
        }
    }
}


// ===========================================================================
// TEMPORARY input stubs -- OWNED BY THE TEMPERATURE / LIGHT (ADC) MODULES.
// These exist only so this file links on its own. DELETE them once the real
// sensor functions exist, or you'll get duplicate-symbol linker errors.
// ===========================================================================
temp_state_t temp_get_state(void) { return TEMP_IDLE; }
int sun_is_out(void) { return 0; }
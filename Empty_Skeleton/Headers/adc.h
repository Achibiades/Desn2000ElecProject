#ifndef ADC_H
#define ADC_H

#include "../Headers/heating.h"   // for temp_state_t

void setupADC(void);
int  readADC(int channel);

#endif
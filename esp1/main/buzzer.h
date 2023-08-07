#ifndef BUZZER_H
#define BUZZER_H

#include <inttypes.h>
#include <pinosGpio.h>

void setupBuzzer();

void setBuzzerIntensity(uint32_t buzzer);

#endif
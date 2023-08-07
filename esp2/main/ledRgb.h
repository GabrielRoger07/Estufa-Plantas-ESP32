#ifndef LEDRGB_H
#define LEDRGB_H

#include <inttypes.h>
#include <pinosGpio.h>

void setupLED();

void setLEDIntensity(uint32_t red, uint32_t green, uint32_t blue);

#endif
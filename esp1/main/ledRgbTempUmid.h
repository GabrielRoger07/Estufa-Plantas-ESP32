#ifndef LEDRGBTEMPUMID_H
#define LEDRGBTEMPUMID_H

#include <inttypes.h>
#include <pinosGpio.h>

void setupLEDTempUmid();

void setLEDTempUmidIntensity(uint32_t red, uint32_t green, uint32_t blue);

#endif
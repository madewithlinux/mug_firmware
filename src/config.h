#pragma once
#include "secrets.h"

#define HOSTNAME            "mug"

#define NUM_LEDS_INNER_RING   12
#define NUM_LEDS_OUTER_RING   24
#define NUM_LEDS              (NUM_LEDS_INNER_RING + NUM_LEDS_OUTER_RING)
// #define NUM_LEDS              48
// #define NUM_LEDS              11

#define DATA_PIN              16
// #define BRIGHTNESS           255
#define BRIGHTNESS           10
#define FRAMES_PER_SECOND    120
#define HEATER_PIN            15

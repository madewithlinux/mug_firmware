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
#define BRIGHTNESS            10
#define FRAMES_PER_SECOND     60
#define HEATER_PIN            15

// #define TEMP_BITS     (temperature.Resolution_9_bits)
// #define TEMP_INTERVAL 28
#define TEMP_BITS     (temperature.Resolution_10_bits)
#define TEMP_INTERVAL 55
// #define TEMP_BITS     (temperature.Resolution_11_bits)
// #define TEMP_INTERVAL 110
// #define TEMP_BITS     (temperature.Resolution_12_bits)
// #define TEMP_INTERVAL 220
#define PID_INTERVAL 300

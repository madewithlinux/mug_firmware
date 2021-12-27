#define FASTLED_ALLOW_INTERRUPTS 0
// #define FASTLED_INTERRUPT_RETRY_COUNT 1
#include <FastLED.h>
#include "config.h"
#include "led_manager.h"
#include "thermostat.h"
#include "eeprom_manager.h"

uint8_t led_brightness = BRIGHTNESS;

const char* get_led_state_str() {
  switch (current_led_state) {
  case INIT:
    return "INIT";
  case WAY_TOO_COLD:
    return "WAY_TOO_COLD";
  case SLIGHTLY_TOO_COLD:
    return "SLIGHTLY_TOO_COLD";
  case AT_TARGET:
    return "AT_TARGET";
  case SLIGHTLY_TOO_HOT:
    return "SLIGHTLY_TOO_HOT";
  case WAY_TOO_HOT:
    return "WAY_TOO_HOT";
  case UNKNOWN_ERROR:
    return "UNKNOWN_ERROR";
  case OTA_START:
    return "OTA_START";
  case OTA_PROGRESS:
    return "OTA_PROGRESS";
  case OTA_END:
    return "OTA_END";
  case OTA_ERROR:
    return "OTA_ERROR";
  default:
    return "UNKNOWN";
  }
}

CRGB leds[NUM_LEDS];

void led_manager_setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(led_brightness);
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
}

void show_led_color(const Color& color) {
  fill_solid(leds, NUM_LEDS, CRGB(color));
  FastLED.show();
}

uint8_t gHue = 0;

volatile led_state_t current_led_state = INIT;
unsigned long state_change_timeout = 500;
unsigned long last_state_change_time = 0;
uint8_t ota_progress_counter = 0;

void update_led_state() {
  if (millis() < last_state_change_time + state_change_timeout) {
    return;
  }
  switch (current_led_state) {
  case OTA_START:
  case OTA_PROGRESS:
  case OTA_END:
  case OTA_ERROR:
    return;
  default:
    break;
  }

  led_state_t next_led_state = UNKNOWN_ERROR;
  if (false) {
  } else if (abs(current_temp_f - target_temp_f) <= (2 * threshold_temp_f)) {
    // temp within desired range
    next_led_state = AT_TARGET;
  } else if (current_temp_f <= temp_cold) {
    next_led_state = WAY_TOO_COLD;
  } else if (current_temp_f < target_temp_f) {
    next_led_state = SLIGHTLY_TOO_COLD;
  } else if (current_temp_f > target_temp_f && current_temp_f < temp_hot) {
    next_led_state = SLIGHTLY_TOO_HOT;
  } else if (current_temp_f >= temp_hot) {
    next_led_state = WAY_TOO_HOT;
  } else {
    next_led_state = UNKNOWN_ERROR;
  }
  if (next_led_state != current_led_state) {
    current_led_state = next_led_state;
    last_state_change_time = millis();
  }
}

void led_manager_loop() {
  // EVERY_N_MILLISECONDS(20) {
  //   gHue++;
  // }

  EVERY_N_MILLISECONDS(50) {
    update_led_state();
  }

  EVERY_N_MILLISECONDS(1000 / FRAMES_PER_SECOND) {
    gHue += (1000 / FRAMES_PER_SECOND) / 4;
    int pos = beatsin16(40, 0, NUM_LEDS - 1);
    switch (current_led_state) {
    case INIT:
      fill_solid(leds, NUM_LEDS, CRGB::White);
      break;

    case WAY_TOO_COLD:
      fill_solid(leds, NUM_LEDS, CRGB::Blue);
      break;

      // case SLIGHTLY_TOO_COLD:
      //   fill_solid(leds, NUM_LEDS, CRGB::Cyan);
      //   break;

    case SLIGHTLY_TOO_COLD:
    case SLIGHTLY_TOO_HOT:
    case AT_TARGET:
      // fill_rainbow(leds, NUM_LEDS, gHue, 255 / NUM_LEDS * 2);
      fill_rainbow(leds, NUM_LEDS, gHue, 7);
      break;

      // case SLIGHTLY_TOO_HOT:
      //   fill_solid(leds, NUM_LEDS, CRGB::Yellow);
      //   leds[pos] = CRGB::Red;
      //   leds[(pos+1) % NUM_LEDS] = CRGB::Red;
      //   break;

    case WAY_TOO_HOT:
      fill_solid(leds, NUM_LEDS, CRGB::Red);
      leds[pos] = CRGB::Yellow;
      leds[(pos + 1) % NUM_LEDS] = CRGB::Yellow;
      break;

    case OTA_START:
      fill_solid(leds, NUM_LEDS, CRGB::Cyan);
      break;
    case OTA_PROGRESS:
      return;
      break;
    case OTA_END:
      fill_solid(leds, NUM_LEDS, CRGB::White);
      break;
    case OTA_ERROR:
      fill_solid(leds, NUM_LEDS, CRGB::Red);
      break;

    default:
    case UNKNOWN_ERROR:
      fill_solid(leds, NUM_LEDS, CRGB::Purple);
      break;
    }

    fill_solid(leds, NUM_LEDS_INNER_RING, CRGB::Black);
    FastLED.setBrightness(led_brightness);
    FastLED.show();
  }

  // FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void set_led_state_ota_progress(unsigned int progress, unsigned int total) {
  fill_solid(leds, NUM_LEDS, CRGB::Cyan);
  // fill_solid(leds, (progress / (total / (NUM_LEDS))) % NUM_LEDS, CRGB::Pink);
  FastLED.show();
}

#include <FastLED.h>
#include "config.h"
#include "led_manager.h"
#include "thermostat.h"
#include "eeprom_manager.h"

CRGB leds[NUM_LEDS];

void led_manager_setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
}

void show_led_color(const Color& color) {
  fill_solid(leds, NUM_LEDS, CRGB(color));
  FastLED.show();
}

uint8_t gHue = 0;

void addGlitter(fract8 chanceOfGlitter) {
  if (random8() < chanceOfGlitter) {
    leds[random16(NUM_LEDS)] += CRGB::White;
  }
}

void led_manager_loop() {

  // send the 'leds' array out to the actual LED strip
  EVERY_N_MILLISECONDS(1000 / FRAMES_PER_SECOND) {
    FastLED.show();
  }
  // insert a delay to keep the framerate modest
  // FastLED.delay(1000 / FRAMES_PER_SECOND);
  EVERY_N_MILLISECONDS(20) {
    gHue++;
  }

  EVERY_N_MILLISECONDS(500) {
    if (false) {
    } else if (abs(current_temp_f - target_temp_f) <= (2 * threshold_temp_f)) {
      // temp within desired range
      fill_rainbow(leds, NUM_LEDS, gHue, 255 / NUM_LEDS * 2);
    } else if (is_heater_on) {
      fill_solid(leds, NUM_LEDS, CRGB::Yellow);
    } else if (current_temp_f > temp_hot) {
      fill_solid(leds, NUM_LEDS, CRGB::Red);
    } else if (current_temp_f < temp_cold) {
      fill_gradient_RGB(leds, NUM_LEDS, CRGB::Blue, CRGB::Cyan);
    } else {
      // unknown
      fill_solid(leds, NUM_LEDS, CRGB::Purple);
    }
  }
}

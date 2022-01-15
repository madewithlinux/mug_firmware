// loops_per_second.cpp
#include <Arduino.h>

#include "loops_per_second.h"
#include "lib8tion_standalone.h"

unsigned loops_per_second = 0;
unsigned loop_counter = 0;

void loops_per_second_loop() {
  loop_counter++;
  EVERY_N_MILLIS(1000) {
    loops_per_second = loop_counter;
    loop_counter = 0;
  }
}


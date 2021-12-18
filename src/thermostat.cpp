#include <Temperature_LM75_Derived.h>
#include "lib8tion_standalone.h"
#include "thermostat.h"
#include "config.h"

TI_TMP275 temperature(0x48);

volatile float current_temp_f = 125;
volatile float target_temp_f = 125;
volatile float threshold_temp_f = 1;
volatile bool is_heater_on = false;
CircularBuffer<float, 128> buffer_temperature;

void thermostat_setup() {
  Wire.begin();
  pinMode(HEATER_PIN, OUTPUT);
  // make sure heater is initially off
  digitalWrite(HEATER_PIN, LOW);
  is_heater_on = false;
}

void thermostat_loop() {

  EVERY_N_MILLISECONDS(10) {
    current_temp_f = temperature.readTemperatureF();
    if (current_temp_f < (target_temp_f - threshold_temp_f)) {
      is_heater_on = true;
    } else if (current_temp_f > (target_temp_f + threshold_temp_f)) {
      is_heater_on = false;
    }
    digitalWrite(HEATER_PIN, is_heater_on);
  }

  EVERY_N_MILLISECONDS(10) {
    buffer_temperature.push(current_temp_f);
  }

}

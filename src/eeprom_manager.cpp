#include <Arduino.h>
#include <ESP_EEPROM.h>
#include "eeprom_manager.h"
#include "thermostat.h"
#include "led_manager.h"

float temp_hot = 130;
float temp_cold = 120;

// eeprom_data_t eeprom_data;
struct {
  float target_temp_f;
  float threshold_temp_f;
  float temp_hot;
  float temp_cold;
  double Kp;
  double Ki;
  double Kd;
  int max_pid_output;
  uint8_t led_brightness;
} eeprom_data;

void eeprom_manager_setup() {
  EEPROM.begin(sizeof(eeprom_data));

  eeprom_data.target_temp_f = target_temp_f;
  eeprom_data.threshold_temp_f = threshold_temp_f;
  eeprom_data.temp_hot = temp_hot;
  eeprom_data.temp_cold = temp_cold;

  eeprom_data.Kp = Kp;
  eeprom_data.Ki = Ki;
  eeprom_data.Kd = Kd;
  eeprom_data.max_pid_output = max_pid_output;
  eeprom_data.led_brightness = led_brightness;

  // read saved data from eeprom (if there is any there)
  if (EEPROM.percentUsed() >= 0) {
    EEPROM.get(0, eeprom_data);
    Serial.println("read data from eeprom");

    target_temp_f = eeprom_data.target_temp_f;
    threshold_temp_f = eeprom_data.threshold_temp_f;
    temp_hot = eeprom_data.temp_hot;
    temp_cold = eeprom_data.temp_cold;
    Kp = eeprom_data.Kp;
    Ki = eeprom_data.Ki;
    Kd = eeprom_data.Kd;
    max_pid_output = eeprom_data.max_pid_output;
    led_brightness = eeprom_data.led_brightness;

  } else {
    Serial.println("no saved data found in eeprom");
  }
}

bool save_config_to_eeprom() {
  eeprom_data.target_temp_f = target_temp_f;
  eeprom_data.threshold_temp_f = threshold_temp_f;
  eeprom_data.temp_hot = temp_hot;
  eeprom_data.temp_cold = temp_cold;
  eeprom_data.Kp = Kp;
  eeprom_data.Ki = Ki;
  eeprom_data.Kd = Kd;
  eeprom_data.max_pid_output = max_pid_output;
  eeprom_data.led_brightness = led_brightness;
  EEPROM.put(0, eeprom_data);
  bool ok = EEPROM.commit();
  return ok;
}

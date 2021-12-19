#include <Arduino.h>
#include <Temperature_LM75_Derived.h>
#include <PID_v1.h>
#include "lib8tion_standalone.h"
#include "thermostat.h"
#include "config.h"

// #define PWM_MAX 65535
#define PWM_MAX 1023
int pwm_output = 0;

TI_TMP275 temperature(0x48);
volatile bool is_thermostat_enabled = true;

volatile float current_temp_f = 125;
volatile float target_temp_f = 125;
volatile float threshold_temp_f = 1;
volatile bool is_heater_on = false;
CircularBuffer<float, 128> buffer_temperature;

// Define Variables we'll be connecting to
double Setpoint, Input, Output;
// Specify the links and initial tuning parameters
double Kp = 2, Ki = 5, Kd = 1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void set_heater_pwm_level(int pwm_level) {
  if (pwm_level > PWM_MAX) {
    pwm_level = PWM_MAX;
  }
  if (pwm_level < 0) {
    pwm_level = 0;
  }
  analogWrite(HEATER_PIN, pwm_level);
  pwm_output = pwm_level;
  is_heater_on = pwm_level > 10;
}

void thermostat_setup() {
  Wire.begin();
  pinMode(HEATER_PIN, OUTPUT);
  // make sure heater is initially off
  digitalWrite(HEATER_PIN, LOW);
  analogWriteRange(PWM_MAX);
  set_heater_pwm_level(0);

  Input = current_temp_f;
  Setpoint = target_temp_f;
  myPID.SetOutputLimits(0, PWM_MAX);
  myPID.SetMode(AUTOMATIC);

  temperature.setResolution(temperature.Resolution_12_bits);
}

void thermostat_loop() {
  if (!is_thermostat_enabled) {
    return;
  }

  EVERY_N_MILLISECONDS(10) {
    current_temp_f = temperature.readTemperatureF();

    Input = current_temp_f;
    Setpoint = target_temp_f;
    myPID.Compute();

    if (current_temp_f < (target_temp_f - threshold_temp_f)) {
      set_heater_pwm_level(PWM_MAX);
    } else if (current_temp_f > (target_temp_f + threshold_temp_f)) {
      set_heater_pwm_level(0);
    } else {
      // it's close enough that we'll trust the PWM output
      set_heater_pwm_level((int)Output);
    }
  }

  EVERY_N_MILLISECONDS(10) {
    buffer_temperature.push(current_temp_f);
  }
}

void thermostat_disable() {
  is_thermostat_enabled = false;
  is_heater_on = false;
  set_heater_pwm_level(0);
}

void thermostat_enable() {
  is_thermostat_enabled = true;
}

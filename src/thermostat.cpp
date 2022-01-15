#include <Arduino.h>
#include <Temperature_LM75_Derived.h>
#include <PID_v1.h>
#include "lib8tion_standalone.h"
#include "thermostat.h"
#include "config.h"
#include "MovingAverage.h"

CircularBuffer<float, 16> buffer_temperature;
MovingAverage<float, 16> temperature_average2;

// #define PWM_MAX 65535
#define PWM_MAX 1023
int pwm_output = 0;
int max_pid_output = PWM_MAX;

TI_TMP275 temperature(0x48);
volatile bool is_thermostat_enabled = true;

volatile float current_temp_f = 125;
volatile float current_temp_f_avg = 125;
volatile float current_temp_f_avg2 = 125;
volatile float target_temp_f = 125;
volatile float threshold_temp_f = 10;
volatile bool is_heater_on = false;

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
  // myPID.SetOutputLimits(0, PWM_MAX);
  myPID.SetOutputLimits(0, max_pid_output);
  myPID.SetTunings(Kp, Ki, Kd, P_ON_E);
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(PID_INTERVAL);

  // temperature.setResolution(temperature.Resolution_12_bits);
  temperature.setResolution(TEMP_BITS);
}

void thermostat_loop() {
  if (!is_thermostat_enabled) {
    return;
  }

  EVERY_N_MILLISECONDS(TEMP_INTERVAL) {
    current_temp_f = temperature.readTemperatureF();
    buffer_temperature.push(current_temp_f);
    temperature_average2.push(current_temp_f);
    current_temp_f_avg2 = temperature_average2.getAverage();
  }

  EVERY_N_MILLISECONDS(PID_INTERVAL) {
    // current_temp_f = temperature.readTemperatureF();
    // double avg = 0;
    // for (decltype(buffer_temperature)::index_t i = 0; i < buffer_temperature.size(); i++) {
    //   avg += double(buffer_temperature[i]) / buffer_temperature.size();
    // }
    // current_temp_f_avg = avg;
    float avg = current_temp_f_avg2;
    Input = avg;
    Setpoint = target_temp_f;
    myPID.SetOutputLimits(0, max_pid_output);
    myPID.SetTunings(Kp, Ki, Kd, P_ON_E);
    myPID.Compute();

    if (avg < (target_temp_f - threshold_temp_f)) {
      set_heater_pwm_level(PWM_MAX);
    } else if (avg > (target_temp_f + threshold_temp_f)) {
      set_heater_pwm_level(0);
    } else {
      // it's close enough that we'll trust the PWM output
      set_heater_pwm_level((int)Output);
    }
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

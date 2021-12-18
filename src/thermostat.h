#pragma once
#include <CircularBuffer.h>

extern volatile float current_temp_f;
extern volatile float current_temp_f;
extern volatile float target_temp_f;
extern volatile float threshold_temp_f;
extern volatile bool is_heater_on;
extern CircularBuffer<float, 128> buffer_temperature;

void thermostat_setup();
void thermostat_loop();

void thermostat_disable();
void thermostat_enable();

extern int pwm_output;
extern double Setpoint, Input, Output;
extern double Kp, Ki, Kd;

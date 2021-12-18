#pragma once

// TODO move these elsewhere?
extern float temp_hot;
extern float temp_cold;


void eeprom_manager_setup();

bool save_config_to_eeprom();

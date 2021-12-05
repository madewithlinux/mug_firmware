#include <Arduino.h>
#include <Temperature_LM75_Derived.h>

// The Generic_LM75 class will provide 9-bit (±0.5°C) temperature for any
// LM75-derived sensor. More specific classes may provide better resolution.
TI_TMP275 temperature(0x48);

void setup() {
  while(!Serial) {}
  
  Serial.begin(9600);

  Wire.begin();
}

void loop() {
  Serial.print("Temperature = ");
  Serial.print(temperature.readTemperatureF());
  Serial.println(" F");

  delay(250);
}

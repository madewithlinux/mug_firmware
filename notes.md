# notes.md



https://github.com/me-no-dev/ESPAsyncWebServer
- [x] https://docs.platformio.org/en/latest/platforms/espressif8266.html#using-arduino-framework-with-staging-version

https://github.com/jwrw/ESP_EEPROM/blob/master/examples/ESP_EEPROM_Use/ESP_EEPROM_Use.ino



https://github.com/FastLED/FastLED/wiki/ESP8266-notes
https://github.com/FastLED/FastLED/wiki/FastLED-HSV-Colors
https://platformio.org/lib/show/126/FastLED
https://github.com/FastLED/FastLED/blob/master/examples/DemoReel100/DemoReel100.ino
http://fastled.io/docs/3.1



https://docs.platformio.org/en/latest/boards/espressif8266/d1_mini.html
https://docs.platformio.org/en/latest/platforms/espressif8266.html#over-the-air-ota-update
https://docs.platformio.org/en/latest/projectconf/interpolation.html
https://tttapa.github.io/ESP8266/Chap08%20-%20mDNS.html

# TODO
- [x] assign it a static IP in my router
  * had to use the router's own diagnostics to ping it by the IP address, for it to show up in the device list
  * I guess the router didn't know about it since it wasn't connecting to the internet?
- [x] fix mDNS
  * having the ESP do a MDNS query every minute did not fix it
  * it seems that `MDNS.announce();` every minute did fix it, though
- [x] fix OTA space limitation by using the correct board spec in `platformio.ini` (should be `d1_mini`, not `d1_mini_lite`)
- [ ] switch to stock web server? (for OTA space reasons)
- [ ] make the webpage auto-refresh every so often?
  * probably nah, that could lose form content and that would be annoying
- [ ] improve LED animations



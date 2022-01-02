# notes.md

http://192.168.1.172/
http://mug.local/
http://192.168.1.173/


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


# webdev notes
https://github.com/rollup/plugins/tree/master/packages/html



# TODO
- [x] assign it a static IP in my router
  * had to use the router's own diagnostics to ping it by the IP address, for it to show up in the device list
  * I guess the router didn't know about it since it wasn't connecting to the internet?
- [x] fix mDNS
  * having the ESP do a MDNS query every minute did not fix it
  * it seems that `MDNS.announce();` every minute did fix it, though
- [x] fix OTA space limitation by using the correct board spec in `platformio.ini` (should be `d1_mini`, not `d1_mini_lite`)
- [x] get it to work from my phone
  * I finally fixed this! thanks to https://forums.att.com/conversations/att-fiber-equipment/problem-with-att-nokia-bgw320505-and-guest-wifi/606f422c4fdc0a391ada3653
  * the fix is to go to router settings > Home Network > Wi-Fi > Advanced Settings, and change 2.4GHz wifi band to `B/G/N/AX`
    * it was defaulted to `G/N` which actualy is a really stupid value for it to be defaulted to we well so that's weird
- [x] make the webpage auto-refresh every so often
- [x] maybe use some RTOS, like the esp one, or simba
  * nah too complicated
- [x] remove all the ota only stuff
- [x] split code into separate files
- [x] switch to stock web server? (for OTA space reasons)
  * nah async is fine
- [x] improve LED animations
- [x] use PID control for heater
- [ ] use interrupt timer or something for reading the temperature?
  * like maybe `timer1_attachInterrupt` in `Arduino.h`
  * or use a library

- [x] publish the code
  - [x] make sure there's no secrets in the git history (use https://rtyley.github.io/bfg-repo-cleaner/ )
  - [x] make github repo and push it

- [x] change the interval that temperature is read to match the TPM275 tempearture read time
- [x] add an additional neopixel ring
- [x] show uptime on main page

- [x] Server sent events
- [x] headers indicating that content shall be cached
- [x] cache-busting URLs
  - [x] for javascript
  - [x] for CSS (actually nah, just make svelte include it in the js)


- [ ] switch from FastLED to NeoPixelBus? (because it has hardware IO for esp8266)
- [ ] power the TPM275 with 3.3V instead of 5V (should improve accuracy and/or read time?)
- [ ] rebuild with ESP32-based D1 mini pro and APA102 LEDs




# TPM275 temperature read time

| res |   typ |   max |
|----:|------:|------:|
|   9 |  27.5 |  37.5 |
|  10 |  55.0 |  75.0 |
|  11 | 110.0 | 150.0 |
|  12 | 220.0 | 300.0 |

resolution in bits, typ, max in ms


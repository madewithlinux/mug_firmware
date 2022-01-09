#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <FS.h>

#include "secrets.h"
#include "config.h"
#include "thermostat.h"
#include "eeprom_manager.h"
#include "webserver.h"
#include "led_manager.h"
#include "log_to_influxdb.h"
#include "lib8tion_standalone.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

void setup() {
  while (!Serial) {
  }
  Serial.begin(115200);
  Serial.println("begin");

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  thermostat_setup();
  led_manager_setup();
  eeprom_manager_setup();
  show_led_color(Color::Yellow);

#pragma region connect to wifi
  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    show_led_color(Color::Blue);
    delay(1000);
    show_led_color(Color::Red);
    delay(1000);
    show_led_color(Color::Blue);
    delay(1000);
    show_led_color(Color::Red);
    delay(1000);
    show_led_color(Color::Blue);
    delay(1000);
    show_led_color(Color::Red);
    delay(1000);
    return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  show_led_color(Color::Blue);
#pragma endregion connect to wifi

#pragma region mDNS
  if (MDNS.begin(HOSTNAME)) {
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
#pragma endregion mDNS


#pragma region OTA
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.onStart([]() {
    Serial.println("ArduinoOTA.onStart");
    thermostat_disable();
    current_led_state = OTA_START;
    led_manager_loop();
    SPIFFS.end();
    webserver_end();
  });
  ArduinoOTA.onEnd([]() {
    current_led_state = OTA_END;
    led_manager_loop();
    Serial.println("\nArduinoOTA.onEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    current_led_state = OTA_PROGRESS;
    set_led_state_ota_progress(progress, total);
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    current_led_state = OTA_ERROR;
    led_manager_loop();
    Serial.printf("ArduinoOTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
#pragma endregion OTA

  webserver_setup();
  log_to_influxdb_setup();
  show_led_color(Color::Green);

  Serial.println("current_temp_f,target_temp_f,threshold_temp_f,is_heater_on");
}

void loop() {
  ArduinoOTA.handle();
  MDNS.update();
  EVERY_N_SECONDS(60) {
    MDNS.announce();
  }

  thermostat_loop();
  led_manager_loop();
  webserver_loop();
  log_to_influxdb_loop();

  EVERY_N_SECONDS(5) {
    Serial.printf("%f,%f,%f,%s\n", current_temp_f, target_temp_f, threshold_temp_f, is_heater_on ? "ON" : "OFF");
  }

}

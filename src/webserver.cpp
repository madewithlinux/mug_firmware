#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "config.h"
#include "webserver.h"
#include "thermostat.h"
#include "eeprom_manager.h"
#include "led_manager.h"
#include "generated/html_templates.inc"

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}

float refresh_interval = 10;

void webserver_setup() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncResponseStream* response = request->beginResponseStream("text/html", CONTROL_PAGE_SIZE_ESTIMATE);
    write_control_page(response);
    request->send(response);
  });

  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest* request) {
    request->send(200);
    ESP.restart();
  });

  server.on("/", HTTP_POST, [](AsyncWebServerRequest* request) {
#define READ_FLOAT_PARAM(param_name)                                            \
  if (request->hasParam(#param_name, true)) {                             \
    param_name = request->getParam(#param_name, true)->value().toFloat(); \
  } else if (request->hasParam(#param_name)) {                            \
    param_name = request->getParam(#param_name)->value().toFloat();       \
  }
    READ_FLOAT_PARAM(target_temp_f)
    READ_FLOAT_PARAM(threshold_temp_f)
    READ_FLOAT_PARAM(temp_hot)
    READ_FLOAT_PARAM(temp_cold)
    READ_FLOAT_PARAM(refresh_interval)
    READ_FLOAT_PARAM(Kp)
    READ_FLOAT_PARAM(Ki)
    READ_FLOAT_PARAM(Kd)
#undef READ_FLOAT_PARAM
    bool ok = save_config_to_eeprom();
    if (!ok) {
      request->send(500, "text/plain", "failed to save state to EEPROM");
    } else {
      AsyncResponseStream* response = request->beginResponseStream("text/html", CONTROL_PAGE_SIZE_ESTIMATE);
      write_control_page(response);
      request->send(response);
    }
  });

  server.onNotFound(notFound);
  server.begin();
}

void webserver_loop() {
}

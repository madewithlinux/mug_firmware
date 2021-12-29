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
#include "lib8tion_standalone.h"

#include "generated/html_templates.inc"

int uptimeS;
int uptimeM;
int uptimeH;

void setUptime() {
  const long millisecs = millis();
  uptimeS = int((millisecs / (1000)) % 60);
  uptimeM = int((millisecs / (1000 * 60)) % 60);
  uptimeH = int((millisecs / (1000 * 60 * 60)));
}

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}

bool read_and_save_config(AsyncWebServerRequest* request) {

#define READ_PARAM(param_name, str_method)                                   \
  if (request->hasParam(#param_name, true)) {                                \
    param_name = request->getParam(#param_name, true)->value().str_method(); \
  } else if (request->hasParam(#param_name)) {                               \
    param_name = request->getParam(#param_name)->value().str_method();       \
  }
#define READ_FLOAT_PARAM(param_name) READ_PARAM(param_name, toFloat)
#define READ_DOUBLE_PARAM(param_name) READ_PARAM(param_name, toDouble)
#define READ_INT_PARAM(param_name) READ_PARAM(param_name, toInt)

  READ_FLOAT_PARAM(target_temp_f)
  READ_FLOAT_PARAM(threshold_temp_f)
  READ_FLOAT_PARAM(temp_hot)
  READ_FLOAT_PARAM(temp_cold)
  READ_FLOAT_PARAM(refresh_interval)

  READ_DOUBLE_PARAM(Kp)
  READ_DOUBLE_PARAM(Ki)
  READ_DOUBLE_PARAM(Kd)

  READ_INT_PARAM(led_brightness)

#undef READ_FLOAT_PARAM
#undef READ_DOUBLE_PARAM
#undef READ_INT_PARAM
#undef READ_PARAM

  return save_config_to_eeprom();
}

float refresh_interval = 10;

void webserver_setup() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncResponseStream* response = request->beginResponseStream("text/html", CONTROL_PAGE_SIZE_ESTIMATE);
    write_control_page(response);
    request->send(response);
  });

  server.on("/system-info", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncResponseStream* response = request->beginResponseStream("text/html", SYSTEM_INFO_PAGE_SIZE_ESTIMATE);
    write_system_info_page(response);
    request->send(response);
  });

  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest* request) {
    request->send(200);
    ESP.restart();
  });

  server.on("/update-config", HTTP_POST, [](AsyncWebServerRequest* request) {
    bool ok = read_and_save_config(request);
    if (!ok) {
      request->send(500, "text/plain", "failed to save state to EEPROM");
    } else {
      if (request->hasHeader("Referer")) {
        request->redirect(request->getHeader("Referer")->value());
      } else {
        // we don't know where to send them back to, so just send them back to homepage
        request->redirect("/");
      }
      // AsyncResponseStream* response = request->beginResponseStream("text/html", CONTROL_PAGE_SIZE_ESTIMATE);
      // write_control_page(response);
      // request->send(response);
    }
  });

  server.onNotFound(notFound);
  server.begin();
}

void webserver_loop() {
  EVERY_N_SECONDS(1) {
    setUptime();
  }
}

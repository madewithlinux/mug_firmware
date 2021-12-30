#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <FS.h>

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

String get_state_json_str() {
  DynamicJsonDocument doc(1024);
  doc["millis"] = millis();

  doc["is_heater_on"] = is_heater_on;
  doc["pwm_output"] = pwm_output;
  doc["current_temp_f"] = current_temp_f;

  doc["target_temp_f"] = target_temp_f;
  doc["threshold_temp_f"] = threshold_temp_f;
  doc["temp_hot"] = temp_hot;
  doc["temp_cold"] = temp_cold;
  doc["refresh_interval"] = refresh_interval;

  doc["kp"] = Kp;
  doc["ki"] = Ki;
  doc["kd"] = Kd;
  doc["output"] = Output;

  doc["led_brightness"] = led_brightness;

  String output;
  serializeJson(doc, output);
  return output;
}

AsyncWebServer server(80);
AsyncEventSource events("/events");

void notFound(AsyncWebServerRequest* request) {
  if (request->method() == HTTP_OPTIONS) {
    request->send(200);
  } else {
    request->send(404);
  }
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

  // if (SPIFFS.exists("/last-modified")) {
  //   File file = SPIFFS.open("/last-modified", "r");
  File file = SPIFFS.open("/last-modified", "r");
  if (file) {
    String last_modified = file.readString();
    Serial.println("read last modified: " + last_modified);
    server.serveStatic("/", SPIFFS, "/www/").setLastModified(last_modified.c_str());
  } else {
    Serial.println("last modified file not found");
    server.serveStatic("/", SPIFFS, "/www/");
  }
  // server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");

  server.on("/system-info", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncResponseStream* response = request->beginResponseStream("text/html", SYSTEM_INFO_PAGE_SIZE_ESTIMATE);
    write_system_info_page(response);
    request->send(response);
  });

  server.on("/api/state", HTTP_GET, [](AsyncWebServerRequest* request) {
    String state_json_str = get_state_json_str();
    request->send(200, "application/json", state_json_str);
  });

  AsyncCallbackJsonWebHandler* handler =
      new AsyncCallbackJsonWebHandler("/api/state", [](AsyncWebServerRequest* request, JsonVariant& json) {
        const JsonObject& jsonObj = json.as<JsonObject>();

        target_temp_f = jsonObj["target_temp_f"] | target_temp_f;
        threshold_temp_f = jsonObj["threshold_temp_f"] | threshold_temp_f;
        temp_hot = jsonObj["temp_hot"] | temp_hot;
        temp_cold = jsonObj["temp_cold"] | temp_cold;
        refresh_interval = jsonObj["refresh_interval"] | refresh_interval;
        Kp = jsonObj["Kp"] | Kp;
        Ki = jsonObj["Ki"] | Ki;
        Kd = jsonObj["Kd"] | Kd;
        led_brightness = jsonObj["led_brightness"] | led_brightness;

        if (!save_config_to_eeprom()) {
          request->send(500, "text/plain", "failed to save state to EEPROM");
        } else {
          request->send(200);
        }
      });
  server.addHandler(handler);

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

  events.onConnect([](AsyncEventSourceClient* client) {
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
    }
    String state_json_str = get_state_json_str();
    // client->send(state_json_str.c_str(), "state", millis(), uint32_t(refresh_interval * 1000.0));
    client->send(state_json_str.c_str(), "state", millis());
  });
  server.addHandler(&events);

  server.onNotFound(notFound);
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");
  server.begin();
}

void webserver_loop() {
  EVERY_N_SECONDS(1) {
    setUptime();
  }
  EVERY_N_SECONDS(10) {
    String state_json_str = get_state_json_str();
    events.send(state_json_str.c_str(), "state", millis());
  }
}

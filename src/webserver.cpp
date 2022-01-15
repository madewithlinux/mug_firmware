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
#include "loops_per_second.h"
#include "util.h"

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
  doc["max_pid_output"] = max_pid_output;

  doc["led_brightness"] = led_brightness;

  String json_str;
  serializeJson(doc, json_str);
  return json_str;
}

AsyncWebServer server(80);
AsyncEventSource events("/api/events");


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

void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if (!request->_tempObject) {
    // this is the first file in this request, so we start by deleting all the existing files under /www/
    Serial.println("delete all existing files under /www/:");
    Dir www = SPIFFS.openDir("/www/");
    while (www.next()) {
      String filename = www.fileName();
      Serial.print("\t");
      Serial.println(filename);
      SPIFFS.remove(filename);
    }
    request->_tempObject = new bool(true);
  }

  File dest;
  if (!index) {
    dest = SPIFFS.open("/www/"+filename, "w");
  } else {
    dest = SPIFFS.open("/www/"+filename, "a");
  }
  if (!dest) {
    Serial.println("failed to open file!");
    request->send(500, "text/plain", "failed to open file!");
    return;
  }
  if (!dest.seek(index)) {
    request->send(500, "text/plain", "seek failed");
    return;
  }
  if (dest.write(data, len) != len) {
    request->send(500, "text/plain", "write failed");
    return;
  }
  dest.close();

  Serial.print("handled upload");
  Serial.print(", filename: ")                ;Serial.print(filename);
  Serial.print(", index: ")                   ;Serial.print(index);
  // Serial.print(", (long unsigned int)data: ") ;Serial.print((long unsigned int)data);
  Serial.print(", len: ")                     ;Serial.print(len);
  Serial.print(", final: ")                   ;Serial.print(final);
  Serial.println();
}

void notFound(AsyncWebServerRequest* request) {
  if (request->method() == HTTP_OPTIONS) {
    request->send(200);
    return;
  }

  String url = request->url();
  Serial.println(url);
  
  String gzFilepath = "/www" + url + ".gz";
  if (SPIFFS.exists(gzFilepath)) {
    AsyncWebServerResponse *response = nullptr;
    if (gzFilepath.endsWith(".html.gz")) {
      response = request->beginResponse(SPIFFS, gzFilepath, "text/html");
    } else if (gzFilepath.endsWith(".js.gz")) {
      response = request->beginResponse(SPIFFS, gzFilepath, "application/javascript");
      response->addHeader("Cache-Control", "max-age=604800, immutable");
    } else if (gzFilepath.endsWith(".css.gz")) {
      response = request->beginResponse(SPIFFS, gzFilepath, "text/css");
    }
    if (response != nullptr) {
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
      return;
    }
  }

  String filepath = "/www" + url;
  if (SPIFFS.exists(filepath)) {
    AsyncWebServerResponse *response = nullptr;
    if (filepath.endsWith(".html")) {  
      response = request->beginResponse(SPIFFS, filepath, "text/html");
    } else if (filepath.endsWith(".js")) {
      response = request->beginResponse(SPIFFS, filepath, "application/javascript");
      response->addHeader("Cache-Control", "max-age=604800, immutable");
    } else if (filepath.endsWith(".css")) {
      response = request->beginResponse(SPIFFS, filepath, "text/css");
    }
    if (response != nullptr) {
      request->send(response);
      return;
    }
  }

  request->send(404);
}

void webserver_setup() {
  server.on("/control-page", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncResponseStream* response = request->beginResponseStream("text/html", CONTROL_PAGE_SIZE_ESTIMATE);
    write_control_page(response);
    request->send(response);
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncWebServerResponse *response = nullptr;
    if (SPIFFS.exists("/www/index.html.gz")) {
      response = request->beginResponse(SPIFFS, "/www/index.html.gz", "text/html");
      response->addHeader("Content-Encoding", "gzip");
    } else {
      response = request->beginResponse(SPIFFS, "/www/index.html", "text/html");
    }
    request->send(response);
  });

  server.on("/system-info", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncResponseStream* response = request->beginResponseStream("text/html", SYSTEM_INFO_PAGE_SIZE_ESTIMATE);
    write_system_info_page(response);
    request->send(response);
  });

  server.on("/api/state", HTTP_GET, [](AsyncWebServerRequest* request) {
    String state_json_str = get_state_json_str();
    request->send(200, "application/json", state_json_str);
  });

  server.on("/api/replace_static_files", HTTP_POST, [](AsyncWebServerRequest* request) {
    Serial.println("POST to /api/replace_static_files");
    const int params = request->params();
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isFile()) { // p->isPost() is also true
        Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if (p->isPost()) {
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }
    request->send(200);
  }, onUpload);

  AsyncCallbackJsonWebHandler* updateStateHandler =
      new AsyncCallbackJsonWebHandler("/api/state", [](AsyncWebServerRequest* request, JsonVariant& json) {
        const JsonObject& jsonObj = json.as<JsonObject>();

        target_temp_f = jsonObj["target_temp_f"] | target_temp_f;
        threshold_temp_f = jsonObj["threshold_temp_f"] | threshold_temp_f;
        temp_hot = jsonObj["temp_hot"] | temp_hot;
        temp_cold = jsonObj["temp_cold"] | temp_cold;
        refresh_interval = jsonObj["refresh_interval"] | refresh_interval;
        Kp = jsonObj["kp"] | Kp;
        Ki = jsonObj["ki"] | Ki;
        Kd = jsonObj["kd"] | Kd;
        led_brightness = jsonObj["led_brightness"] | led_brightness;
        max_pid_output = jsonObj["max_pid_output"] | max_pid_output;

        if (!save_config_to_eeprom()) {
          request->send(500, "text/plain", "failed to save state to EEPROM");
        } else {
          request->send(200);
        }
      });
  server.addHandler(updateStateHandler);

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
  EVERY_N_MILLIS(300) {
    String state_json_str = get_state_json_str();
    events.send(state_json_str.c_str(), "state", millis());
  }
}

void webserver_end() {
  server.end();
  events.close();
}
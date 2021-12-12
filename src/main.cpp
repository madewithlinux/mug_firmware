#include <Arduino.h>
#include <Temperature_LM75_Derived.h>
#include <ESP_EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "secrets.h"


AsyncWebServer server(80);

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const char* PARAM_MESSAGE = "message";
const char* PARAM_TEMP = "temp";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

TI_TMP275 temperature(0x48);

float current_temp_f = 125;
float target_temp_f = 125;
float threshold_temp_f = 1;
bool is_heater_on = false;

struct {
  float target_temp_f;
  float threshold_temp_f;
} eeprom_data;



void setup() {
  while(!Serial) {}
  
  // Serial.begin(9600);
  Serial.begin(115200);
  Wire.begin();
  EEPROM.begin(sizeof(eeprom_data));

  // read saved data from eeprom (if there is any there)
  eeprom_data.target_temp_f = target_temp_f;
  eeprom_data.threshold_temp_f = threshold_temp_f;
  if (EEPROM.percentUsed() >= 0) {
    Serial.println("reading data from eeprom");
    EEPROM.get(0, eeprom_data);
    target_temp_f = eeprom_data.target_temp_f;
    threshold_temp_f = eeprom_data.threshold_temp_f;
    Serial.println(eeprom_data.target_temp_f);
    Serial.println(eeprom_data.threshold_temp_f);
  } else {
    Serial.println("no saved data found in eeprom");
  }


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.printf("WiFi Failed!\n");
      return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // request->send(200, "text/plain", "Hello, world");
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>Mug</title></head><body>");
    response->print("<h1>Mug</h1>");
    response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    response->print("</body></html>");
    request->send(response);
  });

  server.on("/state", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(
      200, "application/json",
      "{\"is_heater_on\":" + (is_heater_on ? String("true") : String("false")) +
      ",\"current_temp_f\":" + String(current_temp_f) +
      ",\"target_temp_f\":" + String(target_temp_f) +
      ",\"threshold_temp_f\":" + String(threshold_temp_f) +
      "}"
    );
  });

  server.on("/state", HTTP_POST, [] (AsyncWebServerRequest *request) {
    if (request->hasParam("target_temp_f", true)) {
        target_temp_f = request->getParam("target_temp_f", true)->value().toFloat();
    } else if (request->hasParam("target_temp_f")) {
        target_temp_f = request->getParam("target_temp_f")->value().toFloat();
    }
    if (request->hasParam("threshold_temp_f", true)) {
        threshold_temp_f = request->getParam("threshold_temp_f", true)->value().toFloat();
    } else if (request->hasParam("threshold_temp_f")) {
        threshold_temp_f = request->getParam("threshold_temp_f")->value().toFloat();
    }
    eeprom_data.target_temp_f = target_temp_f;
    eeprom_data.threshold_temp_f = threshold_temp_f;
    EEPROM.put(0, eeprom_data);
    bool ok = EEPROM.commit();
    if (!ok) {
      request->send(500, "text/plain", "failed to save state to EEPROM");
    } else {
      request->send(
        200, "application/json",
        "{\"target_temp_f\":" + String(target_temp_f) + 
        ",\"threshold_temp_f\":" + String(threshold_temp_f) +
        "}"
      );
    }
  });


  // Send a GET request to <IP>/get?message=<message>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String message;
    if (request->hasParam(PARAM_MESSAGE)) {
        message = request->getParam(PARAM_MESSAGE)->value();
    } else {
        message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, GET: " + message);
  });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
    String message;
    if (request->hasParam(PARAM_MESSAGE)) {
        message = request->getParam(PARAM_MESSAGE)->value();
        message += " in url";
    } else if (request->hasParam(PARAM_MESSAGE, true)) {
        message = request->getParam(PARAM_MESSAGE, true)->value();
    } else {
        message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, POST: " + message);
  });

  server.onNotFound(notFound);

  server.begin();

  Serial.println("current_temp_f,target_temp_f,threshold_temp_f,is_heater_on");
}

void loop() {
  current_temp_f = temperature.readTemperatureF();

  if (current_temp_f < (target_temp_f - threshold_temp_f)) {
    is_heater_on = true;
  } else if (current_temp_f > (target_temp_f + threshold_temp_f)) {
    is_heater_on = false;
  }
  // TODO: actually set heater state

  Serial.printf(
    "%f,%f,%f,%s\n",
    current_temp_f,
    target_temp_f,
    threshold_temp_f,
    is_heater_on ? "ON" : "OFF"
  );

  delay(1000);
}

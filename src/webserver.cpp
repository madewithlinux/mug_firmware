#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "config.h"
#include "webserver.h"
#include "thermostat.h"
#include "eeprom_manager.h"

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}

void writeControlPageResponse(AsyncResponseStream* response) {
  response->println("<!DOCTYPE html><html><head>");
  response->println("<title>Mug</title>");
  response->println("<meta http-equiv=\"refresh\" content=\"10\">");
  response->println("<meta name=\"viewport\" content=\"width=device-width, "
                    "initial-scale=1\">");
  response->println("</head><body>");
  response->println("<style>body {margin: 20px;}</style>");
  response->println("<a href=\"/\"><h1>Mug</h1></a>");
  response->println("<p><a href=\"/\">refresh</a></p>");
  response->println("<form action=\"\" method=\"post\">");
  response->printf("<p>heater state: %s</p>\n", is_heater_on ? "ON" : "OFF");
  response->printf("<p>current_temp_f: %.02f</p>\n", current_temp_f);

#define FORM_INPUT(varname)                                                    \
  response->printf("<p><label>" #varname ": "                                  \
                   "<input id=\"" #varname "\" name=\"" #varname "\""          \
                   "type=\"number\" min=\"0\" step=\"0.1\" value=\"%.01f\"/>"  \
                   "</label></p>\n",                                           \
                   varname);
  FORM_INPUT(target_temp_f)
  FORM_INPUT(threshold_temp_f)
  FORM_INPUT(temp_hot)
  FORM_INPUT(temp_cold)
#undef FORM_INPUT
  response->println("<input type=\"submit\" value=\"save\">");
  response->println("</form>");

  response->println("<div><h2>info</h2>");
#define STATUS_LINE(c_str_var)                                                 \
  response->printf("<p>" #c_str_var ": ");                                     \
  response->print((c_str_var));                                                \
  response->print("</p>\n");
  STATUS_LINE(WiFi.macAddress())
  STATUS_LINE(WiFi.localIP())
  STATUS_LINE(WiFi.gatewayIP())
  STATUS_LINE(WiFi.status())
  STATUS_LINE(MDNS.isRunning())
#undef STATUS_LINE
  response->println("</div>");
  response->println("</body></html>");
}

void webserver_setup() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncResponseStream* response = request->beginResponseStream("text/html");
    writeControlPageResponse(response);
    request->send(response);
  });

  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest* request) {
    request->send(200);
    ESP.restart();
  });

  server.on("/", HTTP_POST, [](AsyncWebServerRequest* request) {
    AsyncResponseStream* response = request->beginResponseStream("text/html");
#define READ_PARAM(param_name)                                                 \
  if (request->hasParam(#param_name, true)) {                                  \
    param_name = request->getParam(#param_name, true)->value().toFloat();      \
  } else if (request->hasParam(#param_name)) {                                 \
    param_name = request->getParam(#param_name)->value().toFloat();            \
  }
    READ_PARAM(target_temp_f)
    READ_PARAM(threshold_temp_f)
    READ_PARAM(temp_hot)
    READ_PARAM(temp_cold)
#undef READ_PARAM
    bool ok = save_config_to_eeprom();
    if (!ok) {
      request->send(500, "text/plain", "failed to save state to EEPROM");
    } else {
      writeControlPageResponse(response);
      request->send(response);
    }
  });

  server.onNotFound(notFound);
  server.begin();
}

void webserver_loop() {
}
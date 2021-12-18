#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <Temperature_LM75_Derived.h>
#include <ESP_EEPROM.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>
#include <CircularBuffer.h>
#include <ESP8266Ping.h>

#define HOSTNAME            "mug"

#define NUM_LEDS            12
#define DATA_PIN            16
#define BRIGHTNESS         255
#define FRAMES_PER_SECOND   60
#define HEATER_PIN          15


#include "secrets.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;


AsyncWebServer server(80);

IPAddress ping_ip = IPAddress();
int ping_count = 1;
bool ping_ret;
int ping_time = 1;
    

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

TI_TMP275 temperature(0x48);
// this is to see how long it takes it to read the temperature. Conclusion is: it's plenty fast
CircularBuffer<float, 128> buffer_temperature;
CircularBuffer<unsigned long, 128> buffer_temp_read_time;


float current_temp_f = 125;
float target_temp_f = 125;
float threshold_temp_f = 1;
bool is_heater_on = false;
float temp_hot = 130;
float temp_cold = 120;

struct {
  float target_temp_f;
  float threshold_temp_f;
  float temp_hot;
  float temp_cold;
} eeprom_data;


CRGB leds[NUM_LEDS];


void writeControlPageResponse(AsyncResponseStream *response) {
  response->println("<!DOCTYPE html><html><head>");
  response->println("<title>Mug</title>");
  response->println("<meta http-equiv=\"refresh\" content=\"10\">");
  response->println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  response->println("</head><body>");
  response->println("<style>body {margin: 20px;}</style>");
  response->println("<a href=\"/\"><h1>Mug</h1></a>");
  response->println("<p><a href=\"/\">refresh</a></p>");
  response->println("<form action=\"\" method=\"post\">");
  response->printf("<p>heater state: %s</p>\n", is_heater_on ? "ON" : "OFF");
  response->printf("<p>current_temp_f: %.02f</p>\n", current_temp_f);

#define FORM_INPUT(varname)                                     \
  response->printf(                                             \
    "<p><label>" #varname ": "                                  \
    "<input id=\"" #varname "\" name=\"" #varname "\""          \
    "type=\"number\" min=\"0\" step=\"0.1\" value=\"%.01f\"/>"  \
    "</label></p>\n",                                           \
    varname                                                     \
  );
  FORM_INPUT(target_temp_f)
  FORM_INPUT(threshold_temp_f)
  FORM_INPUT(temp_hot)
  FORM_INPUT(temp_cold)
#undef FORM_INPUT
  response->println("<input type=\"submit\" value=\"save\">");
  response->println("</form>");

  response->println("<div><h2>info</h2>");
#define STATUS_LINE(c_str_var)              \
  response->printf("<p>" #c_str_var ": ");  \
  response->print((c_str_var));             \
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

void setup() {
  while(!Serial) {}
  Serial.begin(115200);
  Serial.println("begin");
  
  pinMode(HEATER_PIN, OUTPUT);
  // make sure heater is initially off
  digitalWrite(HEATER_PIN, LOW);

  Wire.begin();
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);  
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
#pragma region  read from EEPROM
  EEPROM.begin(sizeof(eeprom_data));
  // read saved data from eeprom (if there is any there)
  eeprom_data.target_temp_f = target_temp_f;
  eeprom_data.threshold_temp_f = threshold_temp_f;
  eeprom_data.temp_hot = temp_hot;
  eeprom_data.temp_cold = temp_cold;
  if (EEPROM.percentUsed() >= 0) {
    Serial.println("reading data from eeprom");
    EEPROM.get(0, eeprom_data);
    target_temp_f = eeprom_data.target_temp_f;
    threshold_temp_f = eeprom_data.threshold_temp_f;
    temp_hot = eeprom_data.temp_hot;
    temp_cold = eeprom_data.temp_cold;
  } else {
    Serial.println("no saved data found in eeprom");
  }
  fill_solid(leds, NUM_LEDS, CRGB::Yellow);
  FastLED.show();
#pragma endregion  read from EEPROM


#pragma region connect to wifi
  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.printf("WiFi Failed!\n");
      fill_solid(leds, NUM_LEDS, CRGB::Blue); FastLED.show(); delay(1000);
      fill_solid(leds, NUM_LEDS, CRGB::Red); FastLED.show(); delay(1000);
      fill_solid(leds, NUM_LEDS, CRGB::Blue); FastLED.show(); delay(1000);
      fill_solid(leds, NUM_LEDS, CRGB::Red); FastLED.show(); delay(1000);
      fill_solid(leds, NUM_LEDS, CRGB::Blue); FastLED.show(); delay(1000);
      fill_solid(leds, NUM_LEDS, CRGB::Red); FastLED.show(); delay(1000);
      return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  FastLED.show();
#pragma endregion connect to wifi

#pragma region OTA
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.onStart([]() {
    Serial.println("ArduinoOTA.onStart");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nArduinoOTA.onEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("ArduinoOTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#pragma endregion OTA

#pragma region mDNS
  if (MDNS.begin(HOSTNAME)) {
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
#pragma endregion mDNS

#pragma region webserver
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    writeControlPageResponse(response);
    request->send(response);
  });
  
  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request){
    request->send(200);
    ESP.restart();
  });
  
  server.on("/temperature_timing", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("text/plain");
    while (!buffer_temp_read_time.isEmpty() && !buffer_temperature.isEmpty()) {
      response->print(buffer_temperature.pop());
      response->print("\t");
      response->print(buffer_temp_read_time.pop());
      response->print("\n");
    }
    request->send(response);
  });
  
  server.on("/ping", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("text/html");

    response->print("ip: ");
    response->print(ping_ip);
    response->print("\n");

    response->print("count: ");
    response->print(ping_count);
    response->print("\n");

    response->print("success: ");
    response->print(ping_ret ? "true" : "false");
    response->print("\n");

    response->print("time: ");
    response->print(ping_time);
    response->print("\n");

    request->send(response);
  });

  server.on("/ping", HTTP_POST, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("text/html");

    ping_ip.fromString(request->getParam("ip")->value());
    response->print("ip: ");
    response->print(ping_ip);
    response->print("\n");
    
    ping_count = 1;
    if (request->hasParam("count")) {
      ping_count = request->getParam("count")->value().toInt();
    }
    response->print("count: ");
    response->print(ping_count);
    response->print("\n");

    request->send(response);
  });

  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("text/html");
#define READ_PARAM(param_name)                                                \
    if (request->hasParam(#param_name, true)) {                               \
        param_name = request->getParam(#param_name, true)->value().toFloat(); \
    } else if (request->hasParam(#param_name)) {                              \
        param_name = request->getParam(#param_name)->value().toFloat();       \
    }                                                                         \
    eeprom_data. param_name = param_name;
    READ_PARAM(target_temp_f)
    READ_PARAM(threshold_temp_f)
    READ_PARAM(temp_hot)
    READ_PARAM(temp_cold)
#undef READ_PARAM
    EEPROM.put(0, eeprom_data);
    bool ok = EEPROM.commit();
    if (!ok) {
      request->send(500, "text/plain", "failed to save state to EEPROM");
    } else {
      writeControlPageResponse(response);
      request->send(response);
    }
  });

  server.onNotFound(notFound);
  server.begin();
#pragma endregion webserver
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  FastLED.show();

  Serial.println("current_temp_f,target_temp_f,threshold_temp_f,is_heater_on");
}


uint8_t gHue = 0;

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void loop() {
  ArduinoOTA.handle();
  MDNS.update();
  // WiFi.getAutoConnect

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);
  EVERY_N_MILLISECONDS( 20 ) { gHue++;}

  EVERY_N_MILLISECONDS(500) {
  if (false) {
  } else if (abs(current_temp_f - target_temp_f) <= (2 * threshold_temp_f)) {
    // temp within desired range
    fill_rainbow(leds, NUM_LEDS, gHue, 255/NUM_LEDS * 2);
  } else if (is_heater_on) {
    fill_solid(leds, NUM_LEDS, CRGB::Yellow);
  } else if (current_temp_f > temp_hot) {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
  } else if (current_temp_f < temp_cold) {
    fill_gradient_RGB(leds, NUM_LEDS, CRGB::Blue, CRGB::Cyan);
  } else {
    // unknown
    fill_solid(leds, NUM_LEDS, CRGB::Purple);
  }
  }

  EVERY_N_MILLISECONDS(100) {
    if (ping_count > 0) {
      ping_ret = Ping.ping(ping_ip, ping_count);
      ping_time = Ping.averageTime();
      ping_count = 0;
    }
  }
  EVERY_N_MILLISECONDS(100) {
    unsigned long time = micros();
    current_temp_f = temperature.readTemperatureF();
    buffer_temp_read_time.push(micros() - time);
    buffer_temperature.push(current_temp_f);

    if (current_temp_f < (target_temp_f - threshold_temp_f)) {
      is_heater_on = true;
    } else if (current_temp_f > (target_temp_f + threshold_temp_f)) {
      is_heater_on = false;
    }
    digitalWrite(HEATER_PIN, is_heater_on);
  }

  EVERY_N_SECONDS(60) {
    MDNS.announce();
  }

  EVERY_N_SECONDS(5) {
    Serial.printf(
      "%f,%f,%f,%s\n",
      current_temp_f,
      target_temp_f,
      threshold_temp_f,
      is_heater_on ? "ON" : "OFF"
    );
  }

}

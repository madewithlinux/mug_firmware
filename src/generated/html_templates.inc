// this is generated code, do not edit!
// generated by web/transform_template.py

// source: aaa_macros.html
#define FORM_INPUT(varname) \
    response->print("<p><label>"); \
    response->print(#varname); \
    response->print(": <input id=\""); \
    response->print(#varname); \
    response->print("\" name=\""); \
    response->print(#varname); \
    response->print("\" type=\"number\" min=\"0\" step=\"0.1\" value=\""); \
    response->printf("%.01f", varname); \
    response->print("\" /></label> </p>");
#define STATUS_LINE(varname) \
    response->print("<p>"); \
    response->print(#varname); \
    response->print(": "); \
    response->print(varname); \
    response->print(" </p>");

// source: control_page.html
#define CONTROL_PAGE_SIZE_ESTIMATE 1280
void write_control_page(AsyncResponseStream* response) {
   response->print("<!DOCTYPE html> <html> <head> <title>Mug</title> <meta http-equiv=\"refresh\" content=\"");
   response->print(refresh_interval);
   response->print("\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> </head> <body> <style> body { margin: 20px; } </style> <a href=\"/\"> <h1>Mug</h1> </a> <p><a href=\"/\">refresh</a></p> <form action=\"\" method=\"post\"> <p>heater state: ");
   response->print(is_heater_on ? "ON" : "OFF");
   response->print(" (level=");
   response->print(pwm_output);
   response->print(")</p> <p>current_temp_f: ");
   response->printf("%.02f", current_temp_f);
   response->print("</p> ");
   FORM_INPUT(target_temp_f)
   response->print(" ");
   FORM_INPUT(threshold_temp_f)
   response->print(" ");
   FORM_INPUT(temp_hot)
   response->print(" ");
   FORM_INPUT(temp_cold)
   response->print(" <div> <h2>info</h2> ");
   STATUS_LINE(WiFi.macAddress())
   response->print(" ");
   STATUS_LINE(WiFi.localIP())
   response->print(" ");
   STATUS_LINE(WiFi.gatewayIP())
   response->print(" ");
   STATUS_LINE(WiFi.status())
   response->print(" ");
   STATUS_LINE(MDNS.isRunning())
   response->print(" ");
   STATUS_LINE(get_led_state_str())
   response->print(" ");
   STATUS_LINE(digitalPinHasPWM(HEATER_PIN))
   response->print(" ");
   STATUS_LINE(Setpoint)
   response->print(" ");
   STATUS_LINE(Input)
   response->print(" ");
   STATUS_LINE(Output)
   response->print(" ");
   STATUS_LINE(Kp)
   response->print(" ");
   STATUS_LINE(Ki)
   response->print(" ");
   STATUS_LINE(Kd)
   response->print(" </div> </body> </html>");
}

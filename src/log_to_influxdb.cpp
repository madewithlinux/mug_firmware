// log_to_influxdb.cpp
// https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino/blob/master/examples/BasicWrite/BasicWrite.ino
#include <ESP8266WiFi.h>
#define DEVICE "ESP8266"
#include <InfluxDbClient.h>

#include "log_to_influxdb.h"
#include "lib8tion_standalone.h"
#include "thermostat.h"
#include "loops_per_second.h"
#include "config.h"
#include "secrets.h"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);

Point sensor("mug_status");

void log_to_influxdb_setup() {
  configTzTime("CST6CDT", "pool.ntp.org", "time.nis.gov");
  client.setWriteOptions(WriteOptions().writePrecision(WritePrecision::MS));

  // Add constant tags - only once
  sensor.addTag("device", DEVICE);
  sensor.addTag("wifi_mac_address", WiFi.macAddress());
  sensor.addTag("wifi_local_ip", WiFi.localIP().toString());
  sensor.addTag("chip_id", String(ESP.getChipId()));
  sensor.addTag("sketch_md5", ESP.getSketchMD5());
  sensor.addTag("reset_reason", ESP.getResetReason());

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void log_to_influxdb_loop() {
  EVERY_N_MILLIS(TEMP_INTERVAL) {
    sensor.clearFields();

    sensor.addField("millis", millis());

    sensor.addField("current_temp_f", current_temp_f);
    sensor.addField("current_temp_f_avg", current_temp_f_avg);
    sensor.addField("current_temp_f_avg2", current_temp_f_avg2);
    sensor.addField("target_temp_f", target_temp_f);
    sensor.addField("threshold_temp_f", threshold_temp_f);
    sensor.addField("is_heater_on", is_heater_on);
    sensor.addField("pwm_output", pwm_output);

    sensor.addField("Setpoint", Setpoint);
    sensor.addField("Input", Input);
    sensor.addField("Output", Output);
    sensor.addField("Kp", Kp);
    sensor.addField("Ki", Ki);
    sensor.addField("Kd", Kd);

    sensor.addField("loops_per_second", loops_per_second);

    sensor.addField("wifi_rssi", WiFi.RSSI());
    sensor.addField("free_heap", ESP.getFreeHeap());
    sensor.addField("heap_fragmentation", ESP.getHeapFragmentation());
    sensor.addField("max_free_block_size", ESP.getMaxFreeBlockSize());

    if (!client.writePoint(sensor)) {
      Serial.print("Writing: ");
      Serial.println(client.pointToLineProtocol(sensor));
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
  }
}

void log_to_influxdb_end() {
}

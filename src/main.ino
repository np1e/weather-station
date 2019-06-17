#include <MQTT.h>
#include <MQTTClient.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <pins_arduino.h>
#include <ESP8266WiFi.h>
#include "config.h"

#define DHT_PIN 0
#define DHT_TYPE DHT22

DHT_Unified dht(DHT_PIN, DHT_TYPE);
WiFiClient wifi;
MQTTClient mqtt;

void connect();

void setup() {
    Serial.begin(115200);
    Serial.println(F("DHTxx test!"));

    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(SSID, PASSWORD);

    mqtt.begin(HOST, wifi);

    connect();

    dht.begin();

    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    Serial.println(F("------------------------------------"));
    Serial.println(F("Temperature Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
    Serial.println(F("------------------------------------"));
    // Print humidity sensor details.
    dht.humidity().getSensor(&sensor);
    Serial.println(F("Humidity Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
    Serial.println(F("------------------------------------"));
}

void loop() {

    if (!mqtt.connected()) {
        Serial.println("connecting to mqtt");
        connect();
    }

    delay(2000);

    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if(isnan(event.temperature)) {
        Serial.println("Failed to read temperature.");
        return;
    } else {
        mqtt.publish("temperature", String(event.temperature));
        Serial.print("Temperature: ");
        Serial.print(event.temperature);
        Serial.println(" °C");
    }

    dht.humidity().getEvent(&event);
    if(isnan(event.relative_humidity)) {
        Serial.println("Failed to read humidity");
        return;
    } else {
        mqtt.publish("humidity", String(event.relative_humidity));
        Serial.print("Humidity: ");
        Serial.print(event.relative_humidity);
        Serial.println(" %");
    }
    
}

void connect() {
    while(WiFi.waitForConnectResult() != WL_CONNECTED) {
        WiFi.begin(SSID, PASSWORD);
        Serial.println("Wifi connection failed. Retry...");
    }

    Serial.print("Wifi connection successful - IP Address: ");
    Serial.println(WiFi.localIP());

    while(!mqtt.connect("D1mini")) {
        Serial.println(".");
    }

    Serial.println("MQTT connected!");
}
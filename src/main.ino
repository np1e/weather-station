#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <pins_arduino.h>
#include <MQTTClient.h>
#include <ESP8266WiFi.h>
#include "config.h"

#define DHT_PIN 6
#define DHT_TYPE DHT11

DHT_Unified dht(DHT_PIN, DHT_TYPE);
WiFiClient wifi;
MQTTClient mqtt;

void connect();

void setup() {
    Serial.begin(19200);
    Serial.println(F("DHTxx test!"));
    pinMode(BUILTIN_LED, OUTPUT);

    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(SSID, PASSWORD);

    mqtt.begin(HOST, wifi);

    connect();

    dht.begin();
}

void loop() {

    if (!mqtt.connected()) {
        connect();
    }

    delay(2000);

    digitalWrite(BUILTIN_LED, HIGH);
    delay(1000);
    digitalWrite(BUILTIN_LED, LOW);

    sensors_event_t event;
    dht.temperature().getEvent(&event);

    if(isnan(event.temperature)) {
        Serial.println("Failed to read data.");
        return;
    }

    mqtt.publish("temperature" ,event.temperature);
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" °C");
    
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
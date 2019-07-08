#include <MQTT.h>
#include <MQTTClient.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <pins_arduino.h>
#include <ESP8266WiFi.h>
#include "config.h"

#define DHT_PIN 0 // D3
#define RED_PIN 12 // D6
#define GREEN_PIN 14 // D5
#define DHT_TYPE DHT22
#define SLEEP_TIME 1800 // 900s = 15min

DHT_Unified dht(DHT_PIN, DHT_TYPE);
WiFiClient wifi;
MQTTClient mqtt;

void connect();

void setup() {
    Serial.begin(115200);
    Serial.println(F("DHTxx test!"));
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
    digitalWrite(GREEN_PIN, HIGH);

    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(SSID, PASSWORD);

    mqtt.begin(HOST, wifi);

    //testLEDs();

    connect();

    readTemperature();
    delay(1000);

    Serial.print("Going to sleep now for "); Serial.print(SLEEP_TIME); Serial.println(" seconds.");
    ESP.deepSleep(SLEEP_TIME * 1000000);
    digitalWrite(RED_PIN, HIGH);
}

void readTemperature() {

    if (!mqtt.connected()) {
        Serial.println("connecting to mqtt");
        connect();
    }

    dht.begin();

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

void loop() {

}

void connect() {
    setLED("error");
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
    setLED("ok");
}

void testLEDs() {
    Serial.println("Testing leds...");
    for(int i = 0; i < 5; i += 1){
        digitalWrite(GREEN_PIN, HIGH);
        delay(100);
        digitalWrite(RED_PIN, HIGH);
        delay(100);
        digitalWrite(GREEN_PIN, LOW);
        delay(100);
        digitalWrite(RED_PIN, LOW);
        delay(100);
    }
}

void setLED(String mode) {
    if(mode == "error") {
        digitalWrite(RED_PIN, HIGH);
        digitalWrite(GREEN_PIN, LOW);
    } else if (mode == "ok") {
        digitalWrite(RED_PIN, LOW);
        digitalWrite(GREEN_PIN, HIGH);
    }
}
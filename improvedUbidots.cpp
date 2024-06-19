#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Ubidots.h"
#include <DHT.h>

const char* UBIDOTS_TOKEN = "xxxxxxxxx";
const char* WIFI_SSID = "xxxxxxx";
const char* WIFI_PASS = "xxxxxxxxxxx";

Ubidots ubidots(UBIDOTS_TOKEN);
float hum = 0.0, temp_c = 0.0, temp_f = 0.0;
const byte DHT_PIN = D1;
const byte DHT_TYPE = 22;
DHT dht(DHT_PIN, DHT_TYPE);
int gasValue = 0;
int relayState = 0;
const byte RELAY = D7;
const byte FLAME_SENSOR = D6;
ADC_MODE(ADC_TOUT);
String received_data = "";

void callback(char* topic, byte* payload, unsigned int length) {
  received_data = topic;
  if (received_data.indexOf("483FDA6212F8/Relay") > -1) {
    if ((char)payload[0] == '1') {
      digitalWrite(RELAY, HIGH);
    } else {
      digitalWrite(RELAY, LOW);
    }
  }
}

void setup() {
  dht.begin();
  Serial.begin(115200);
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
  ubidots.setDebug(true);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  ubidots.subscribe("483FDA6212F8", "Relay", callback);
}

void loop() {
  if (!ubidots.wifiConnected()) {
    ubidots.reconnectWiFi();
    ubidots.subscribe("483FDA6212F8", "Relay", callback);
  }

  ubidots.loop();

  delay(5000);

  hum = dht.readHumidity();
  temp_c = dht.readTemperature();

  if (isnan(hum) || isnan(temp_c)) {
    Serial.println("Failed To Read From DHT Sensor");
    return;
  }

  // Read gas sensor value
  gasValue = analogRead(A0);

  // Read the state of the relay pin
  relayState = digitalRead(RELAY);

  // Read the state of the flame sensor
  int flameState = digitalRead(FLAME_SENSOR);

  // Send data to Ubidots
  ubidots.add("Temperature", temp_c);
  ubidots.add("Humidity", hum);
  ubidots.add("Gas", gasValue);
  ubidots.add("Relay", relayState);
  ubidots.add("Flame", flameState);

  ubidots.send();
}

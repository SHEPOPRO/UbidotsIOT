#include "Ubidots.h"
#include <ESP8266WiFi.h>
#include <DHT.h>

const char* UBIDOTS_TOKEN = "BBUS-wpyqbdBK8B3kGW8uKIX2MeHLo7hMVy";  // Put here your Ubidots TOKEN
const char* WIFI_SSID = "SHEPO";                                    // Put here your Wi-Fi SSID
const char* WIFI_PASS = "01024063";                                 // Put here your Wi-Fi password

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

void setup() {
  dht.begin();
  Serial.begin(115200);
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
  ubidots.setDebug(true);
  pinMode(RELAY, OUTPUT);
  pinMode(FLAME_SENSOR, INPUT);
}

void loop() {
  delay(5000);
  
  // Read DHT sensor values
  hum = dht.readHumidity();
  temp_c = dht.readTemperature();

  if (isnan(hum) || isnan(temp_c)) {
    Serial.println("Failed To Read From DHT Sensor");
    return;
  }

  // Read flame sensor value
  int sensorValue = analogRead(FLAME_SENSOR);
  Serial.print("Flame Sensor Value: ");
  Serial.println(sensorValue);

  // Define the threshold value for flame detection
  int thresholdValue = 500;  // Adjust this value as per your requirements

  // Add flame detection logic
  if (sensorValue > thresholdValue) {
    Serial.println("Flame detected!");
  } else {
    Serial.println("No flame detected");
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

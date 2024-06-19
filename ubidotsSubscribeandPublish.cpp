#include "UbidotsESPMQTT.h"
#include <ESP8266WiFi.h>
#include <DHT.h>
/****************************************
 * Define Constants
 ****************************************/
#define TOKEN "BBUS-Wxi0J1rgYeh9nMG7bK2vSg4C3HxgsR"     // Your Ubidots TOKEN
#define WIFINAME  "SHEPO"  // Your SSID
#define WIFIPASS "01024063"  // Your Wifi Pass

Ubidots client(TOKEN);
float hum = 0.0, temp_c = 0.0, temp_f = 0.0;
const byte DHT_PIN = D1;
const byte DHT_TYPE = 22;
DHT dht(DHT_PIN, DHT_TYPE);
int digital =0;
ADC_MODE(ADC_TOUT);
const byte RELAY = D7;
const byte FLAME_SENSOR = D6;
ADC_MODE(ADC_TOUT);
String received_data = "";
/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char* topic, byte* payload, unsigned int length) {
  received_data = topic;
  if (received_data.indexOf("NodeMCUu/relay") > -1) {
    if ((char)payload[0] == '1') {
      digitalWrite(RELAY, HIGH);
    } else {
      digitalWrite(RELAY, LOW);
    }
  }
}

/****************************************
 * Main Functions
 ****************************************/

void setup() {
  // put your setup code here, to run once:
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
 
  Serial.begin(115200);
  client.setDebug(true);  // Pass a true or false bool value to activate debug messages
  client.wifiConnection(WIFINAME, WIFIPASS);
  client.begin(callback);
  client.ubidotsSubscribe("NodeMCU","relay");  // Insert the dataSource and Variable's Labels
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    client.reconnect();
    client.ubidotsSubscribe("NodeMCU","relay");  // Insert the dataSource and Variable's Labels
  }
    hum = dht.readHumidity();
  temp_c = dht.readTemperature();

  if (isnan(hum) || isnan(temp_c)) {
    Serial.println("Failed To Read From DHT Sensor");
    return;
  }
   digital = analogRead(A0);

  client.add("Temperature", temp_c);
  client.add("Humidity", hum);
  client.add("Gas", digital);
  client.add("Flame",FLAME_SENSOR);
  client.ubidotsPublish("NodeMCU");
  client.loop();

}

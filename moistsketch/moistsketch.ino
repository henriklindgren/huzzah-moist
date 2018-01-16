/*
 *  Author: Henrik Lindgren
 *  Licensed under Apache License Version 2.0 
 *  See http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Hardware requirements:
 *    * Adafruit Feather HUZZAH ESP8266
 *    * Moisture sensor based on LM393
 *    * Humidity and Temperature sensor DHT11 (or easily converted to using DHT22)
 *    
 *  Software requirements:
 *    * Programmed and uploaded using Arduino IDE 1.8.5
 *    See https://www.arduino.cc/en/Main/Software
 *    * Arduino ESP8266 plugin 2.4.0
 *    Using http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *    See https://github.com/esp8266/Arduino/releases/tag/2.4.0
 *    
 *    DHT library depends on Adafruit Sensor Library, both available by searching in the Arduino IDE.
 *    
 *    Code based on examples for ESP8266WiFi, Blink and AnalogInput.
 */

#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>

#include "config.h"

WiFiClient client;
const int httpPort = 80;

//huzzah analog pin for the moisture sensor
const int analogPin = MOISTURESENSORANALOG;
// main red led on the huzzah
// const int ledPin = 0;

//expected to be between 0 and 1024
int soilMoisture = 0;
int humidity = 0;
int temperature = 0;
DHT dht = DHT(DHTPIN, DHTTYPE);

const int secondsLoopDelay = SLEEPTIMESECONDS;

void setup() {  
  Serial.begin(115200);
  // Delay atleast a second before initializing DHT11 according to docs.
  delay(1100);
  dht.begin();
  
  Serial.print("Connecting to WiFi SSID: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.print("Connecting to ");
  Serial.println(HOST);
  
  if (!client.connect(HOST, httpPort)) {
    Serial.println("Connection failed.");
    return;
  } else {
    // Make measurement.
    soilMoisture = analogRead(analogPin);
    delay(100);

    // Documentation states these measurements can take up to 250ms per measure.
    humidity = (int)dht.readHumidity();
    temperature = (int)dht.readTemperature();
    Serial.println(String("moist:") + soilMoisture + " temp:" + temperature + " hum:" + humidity);
    
    // Send measurement
    client.print(String("GET ") + ENDPOINT + "?moisture=" + soilMoisture + "&temp=" + soilMoisture + "&humidity=" + soilMoisture + 
      " HTTP/1.1\r\nHost: " + HOST + "\r\nConnection: close\r\n\r\n");
    delay(500);
    
    // Read all the lines of the reply from server and print them to Serial.
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    Serial.println();
     
    Serial.println("Closing connection.");
    client.stop();
  } 
  delay(secondsLoopDelay * 1000);
}


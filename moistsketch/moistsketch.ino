/*
 *  Author: Henrik Lindgren
 *  Licensed under Apache License Version 2.0 
 *  See http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Hardware requirements:
 *    * Adafruit Feather HUZZAH ESP8266
 *    * Moisture sensor based on LM393
 *    
 *  Software requirements:
 *    * Programmed and uploaded using Arduino 1.6.9
 *    See https://www.arduino.cc/en/Main/Software
 *    * Arduino ESP8266 plugin 2.3.0
 *    See http://esp8266.github.io/Arduino/versions/2.3.0/
 *    
 *    Code based on examples for ESP8266WiFi, Blink and AnalogInput.
 */

#include <ESP8266WiFi.h>
/*
 * Local configuration
 * 
 * SSID
 * PASSWORD
 * HOST
 * ENDPOINT
 */
#include "config.h"

WiFiClient client;
const int httpPort = 80;

//huzzah analog pin for the moisture sensor
const int analogPin = A0;
// main red led on the huzzah
// const int ledPin = 0;
//expected to be between 0 and 1024
int sensorValue = 0;
const int secondsLoopDelay = 30;

void setup() {  
  Serial.begin(115200);
  delay(100);
  Serial.print("Connecting to wifi SSID: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.print("connecting to ");
  Serial.println(HOST);
  
  if (!client.connect(HOST, httpPort)) {
    Serial.println("connection failed");
    return;
  } else {
    // Make measurement
    sensorValue = analogRead(analogPin);
    delay(100);
    // Send measurement
    client.print(String("GET ") + ENDPOINT + "?moisture=" + sensorValue + 
      " HTTP/1.1\r\nHost: " + HOST + "\r\nConnection: close\r\n\r\n");
    delay(500);
    
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    Serial.println();
     
    Serial.println("closing connection");
    client.stop();
  } 
  delay(secondsLoopDelay * 1000);
}


/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin 9 to ground

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogInOutSerial
*/
#include <ESP8266WiFi.h>
#include <SPI.h>
#define PubNub_BASE_CLIENT WiFiClient
#define PUBNUB_DEBUG
#include <PubNub.h>


// Wireless settings
const char* ssid = "wifiwifiwifiwifi";
const char* pass = "passpasspasspass";

const static char pubkey[] = "txurururu";         // your publish key 
const static char subkey[] = "yeyeyeyeye";         // your subscribe key
const static char channel[] = "plantinhas-felizes"; // channel to use

int status = WL_IDLE_STATUS; // the Wifi radio's status

WiFiClient client;

// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int valveOpener = 0;

int sensorValue = 0;        // value read from the pot

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
    pinMode(0, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
     wifiConnection();
     PubNub.begin(pubkey, subkey);
}

void openValve() {
    digitalWrite(valveOpener, HIGH);
    Serial.println("OPEN");
}

void closeValve() {
    digitalWrite(valveOpener, LOW);
    Serial.println("CLOSED");
}

void loop() {           
  sensorValue = analogRead(analogInPin);

  Serial.print("sensor = ");
  Serial.println(sensorValue);

  if (sensorValue > 500) {
    Serial.println("valor alto demais!");
    return;
  }
  if (sensorValue < 0) {
    Serial.println("valor baixo demais!");
    return;
  }
  if (sensorValue > 80) {
    openValve();
  } else {
    closeValve();
  }

   if (WiFi.status() != WL_CONNECTED) {
        wifiConnection();
    } else {
      delay(1000);
      sendMessage(sensorValue);
    }
  
  delay(3000);
}

void sendMessage(int sensorValue) {
   WiFiClient *client;

      String message = String("\"" + String(sensorValue) + "\"");
      char msg[5];
      message.toCharArray(msg, 5);
    
      client = PubNub.publish(channel, msg);
      
      if (!client) {
          Serial.println("publishing error");
          delay(1000);
          return;
      }
      
      if (PubNub.get_last_http_status_code_class() != PubNub::http_scc_success) {
          Serial.print("Got HTTP status code error from PubNub, class: ");
          Serial.print(PubNub.get_last_http_status_code_class(), DEC);
      }
      
      while (client->available()) {
          Serial.write(client->read());
      }
      
      client->stop();
      Serial.println("---");
}


void wifiConnection() {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

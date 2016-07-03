/***************************************************************************************************
 * This sample code is designed for WiThumb.
 * https://www.kickstarter.com/projects/hackarobot/withumb-arduino-compatible-wifi-usb-thumb-imu
 * 
 * 
 * In this example, WiThumb is programmed as a MQTT client publishing topic 'temperature'
 * 
 * It requires two libraries. Please install them before compiling this.
 * 1. https://github.com/knolleary/pubsubclient
 * 2. https://github.com/adafruit/Adafruit_MCP9808_Library
 * 
 * For more information about WiThumb, please check out our web site:
 * http://www.hackarobot.com
 * 
 * Comments, questions? Please check out our Google+ community:
 * https://plus.google.com/u/0/communities/111064186308711245928
 * 
 **************************************************************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "Adafruit_MCP9808.h"

long  firstTime=millis();           

// sample and post temerapture about every 2 minutes
#define sampleInterval 120

// temperature sensor timeout in 3 seconds
#define timeOutSensor 3
// if sensor timed out, deep sleep and wake up in 30 seconds
#define tempSensorResetInterval 30 

// WiFi timeout if not connected in 10 seconds
#define timeOutWiFi   10  

// if not able to connect to WiFi , deep sleep and wake up in 30 seconds
#define wifiResetInterval 30

// I2C address of MCP9808
#define tempSensorAddress 0x1F

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

float temperature;                  // temperature in F

//------------------------------------
// WiFi
//------------------------------------
char ssid[] = "ssid";                // enter your network SSID (name)
char pass[] = "pass";                // enter your network password



//------------------------------------
// MQTT
//------------------------------------
char mqttServer[]="127.0.0.1";       // MQTT server IP address
#define mqttPort 1883
WiFiClient wifi;
PubSubClient client(wifi);

void setup() {
  Serial.begin(115200);
  
  if (findTemperatureSensor()) {
    temperature=readTemperature();
    publishMQTT();
  } else {
     Serial.println("Unable to find sensor - reset and wake up in "+String(tempSensorResetInterval)+ " seconds");
     ESP.deepSleep(tempSensorResetInterval*1000000, WAKE_RF_DEFAULT);
  }

  long sleepTime = sampleInterval*1000 - (millis()-firstTime);
  if (sleepTime < 0) { 
      sleepTime =1;
  } 
  Serial.println("\nData posted. Go to deep sleep and reset in "+String((sleepTime/1000.0))+ " seconds"); 
  ESP.deepSleep(sleepTime*1000, WAKE_RF_DEFAULT);
}

  


void publishMQTT() {
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  doDelays(1500);   
  if (!client.connected()) {
    reconnect();
  }
  char msg[10];
  // well, snprintf doesn't really handle floating point format... this is a work-around
  snprintf(msg,10, "t=%d.%02d",(int)temperature,((int)(temperature*100+0.5))%100);
  client.publish("temperature",msg);
}
void loop() {
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("WiThumb","hello world");
      // ... and resubscribe
      //client.subscribe("reset");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      doDelays(5000);
    }
  }
}

bool findTemperatureSensor() {
  long startMS=millis();
  while(1) {
    if (millis()-startMS > timeOutSensor*1000) {     // timeout
      return false;
    } else if (tempsensor.begin(tempSensorAddress)) {
      return true;
    }
  }
}

float readTemperature() {
  float t;
  Serial.println("wake up MCP9808.... "); 
  tempsensor.shutdown_wake(0);   // Don't remove this line! required before reading temp
  delay(300);                    // wait 300ms so that the sensor is ready for reading. Otherwise, the temperature won't change.
  t = tempsensor.readTempC()*1.8 + 32.0;
  Serial.println("Temperature is "+String(t)+" degree F"); 
  Serial.println("Shutdown MCP9808.... ");
  tempsensor.shutdown_wake(1);   // shutdown MSP9808 - power consumption ~0.1 micro Ampere
  return t;
}

void doDelays(long ms) { // yield for 'ms' milli-seconds
  long endMs = millis() + ms;
  while (millis() < endMs) {
     yield();
  }
}

/***************************************************************************************************
 * This sample code is designed for WiThumb:
 * https://www.kickstarter.com/projects/hackarobot/withumb-arduino-compatible-wifi-usb-thumb-imu
 * 
 * 
 * In this example, WiThumb posts temperature readings to the cloud (hosted on data.sparkfun.com)
 * 
 * It requires the following library. Please install it before compiling this.
 *     https://github.com/adafruit/Adafruit_MCP9808_Library
 * 
 * For more information about WiThumb, please check out my Instructables
 * http://www.instructables.com/id/How-to-Build-a-WiFi-Thermometer/
 * 
 * Comments, questions? Please check out our Google+ community:
 * https://plus.google.com/u/0/communities/111064186308711245928
 * 
 **************************************************************************************************/
#include <Wire.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MCP9808.h"

long  firstTime=millis();

// sample and post temperature about every 2 minutes
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


char ssid[] = "...";                // enter your network SSID (name)
char pass[] = "...";                // enter your network password

float temperature;                  // temperature in F

const char* host = "data.sparkfun.com";
const int httpPort = 80;  
const char* privatekey = "....................";     // enter your private key to post data to the host
const char* publickey =  "....................";     // enter your public key to post data to the host


void setup() {

  Serial.begin(115200);
  Serial.println("\n-----------------------------------------\n");
  Serial.println("REBOOT");
  Wire.begin();
  

  if (findTemperatureSensor()) {
    temperature=readTemperature();
    if (!connectToWiFi()) {
      Serial.println("\nWiFi timeout - reset in "+String(wifiResetInterval)+ " seconds");
      ESP.deepSleep(wifiResetInterval*1000000, WAKE_RF_DEFAULT);
    } else {
      postToSparkFun();
    } 
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

void loop() {

}

void postToSparkFun() {
    Serial.println("connecting to "+String(host));

    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }
    String url = "/input/"+String(publickey)+"?private_key="+String(privatekey)+"&temp="+String(temperature);

    Serial.println("Requesting URL: "+url);
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
}




void doDelays(long ms) { // yield for 'ms' milli-seconds
  long endMs = millis() + ms;
  while (millis() < endMs) {
     yield();
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

bool connectToWiFi() {
  Serial.println("Connecting to "+String(ssid));
  WiFi.begin(ssid, pass);
  long startMS=millis();

  while (WiFi.status() != WL_CONNECTED) {
    doDelays(500);
    Serial.print(".");
    if (millis()-startMS > timeOutWiFi *1000) {
      return false;
    } 
  } 
  Serial.print("\nWiFi connected (IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println(")");
  return true;
}


/***************************************************************************************************
 * This sample code is designed for Hackabot Nano (Arduino Compatible Robotic Kit)
 * 
 * 
 * Tutorial 5:
 * In this example, the program reads GPS location and report it to a tablet/phone over bluetooth.
 * 
 * It requires two libraries. Please install them before compiling this.
 * 1. https://www.arduino.cc/en/Reference/SoftwareSerial
 * 2. http://arduiniana.org/libraries/tinygps/
 * 
 * For more information about Hackabot Nano, please check out our web site:
 * http://www.hackarobot.com
 * 
 * Comments, questions? Please check out our Google+ community:
 * https://plus.google.com/u/0/communities/111064186308711245928
 * 
 **************************************************************************************************/

#include <SoftwareSerial.h>
#include <TinyGPS.h>
#define pinGPSTX 4
#define pinGPSRX 5
TinyGPS gps;
SoftwareSerial ss(pinGPSTX, pinGPSRX);


void setup() {
  Serial.begin(9600);       // Baud rate of Bluetooth HC06
  ss.begin(9600);           // start GPS
}

void loop() {
  reportGPS();          // report GPS location
  delay(5000);          // wait 5 seconds 
}


//==========================================================
// GPS
//==========================================================

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

void reportGPS() {
  float flat, flon;
  unsigned long age;
  bool failed=false;
  String msg="";
  
  gps.f_get_position(&flat, &flon, &age); 
  gps.satellites();
  smartdelay(0);
  gps.hdop();
  smartdelay(0);
  if (TinyGPS::GPS_INVALID_F_ANGLE == flat) {
    failed=true;
  } 
  smartdelay(0);
  if (TinyGPS::GPS_INVALID_F_ANGLE == flon) {
    failed=true;
  } 
  if (failed) {
    Serial.println("Unable to get location information. It may take a few minutes.");
  } else {
    Serial.println("Lat,Long = " +String(flat) +  "," + String(flon));
  }
  smartdelay(100);
}


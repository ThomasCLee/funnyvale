/***************************************************************************************************
 * This sample code is designed for WiThumb.
 * https://www.kickstarter.com/projects/hackarobot/withumb-arduino-compatible-wifi-usb-thumb-imu
 * 
 * 
 * In this example, we are reading temperature from MCP9808 temperature sensor and 
 * the orientation through the MPU6050 6-axis gyroscope/acclerometer.
 * 
 * It requires the following. Please install it before compiling this program.
 * 1. https://github.com/adafruit/Adafruit_MCP9808_Library
 * 
 * For more information about WiThumb, please check out our web site:
 * http://www.funnyvale.com/WiThumb
 * 
 * Comments, questions? Please check out our Google+ community:
 * https://plus.google.com/u/0/communities/111064186308711245928
 * 
 **************************************************************************************************/

#include "Adafruit_MCP9808.h"
// temperature sensor timeout in 3 seconds
#define timeOutSensor 3
long  firstTime=millis();


// I2C address of MCP9808
#define tempSensorAddress 0x1F

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

float temperature;                  // temperature in F


// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;


int16_t ax, ay, az;
int16_t gx, gy, gz;

// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
//#define OUTPUT_READABLE_ACCELGYRO

// uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// binary, one right after the other. This is very fast (as fast as possible
// without compression or data loss), and easy to parse, but impossible to read
// for a human.
//#define OUTPUT_BINARY_ACCELGYRO

int gyroCount=0;
int gyroThreshold=5000;
bool test2done=false;
int stage=0;

void setup() {

  Serial.begin(115200);
  Serial.println("\n-----------------------------------------\n");
  Serial.println("REBOOT");
  Wire.begin();
  
  temperatureTest(10);
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
     Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
     Fastwire::setup(400, true);
  #endif
  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  
}

void loop() {
  gyroCount++;

  if (!test2done) {
    readGyro(); 
    if (stage==0){ if (gx< -gyroThreshold) {stage++; Serial.println("X+");}}
    else if (stage==1){ if (gx> gyroThreshold)  {stage++;Serial.println("X-");}}  
    else if (stage==2){ if (gy< -gyroThreshold)  {stage++;Serial.println("Y+");}}  
    else if (stage==3){ if (gy> gyroThreshold)  {stage++;Serial.println("Y-");}}   
    else if (stage==4){ if (gz< -gyroThreshold)  {stage++;Serial.println("Z-");}}  
    else if (stage==5){ if (gz> gyroThreshold)  {stage++;Serial.println("Z+");}}  
    if (stage==6) {
      Serial.println("Test 2 : Pass");
      test2done=true;
    } else if (gyroCount>10000) {
      Serial.println("Test 2 : Fail");
      test2done=true;
    }
  } 

}

void readGyro() {
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    #ifdef OUTPUT_READABLE_ACCELGYRO
        // display tab-separated accel/gyro x/y/z values
        Serial.print("a/g:\t");
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.println(gz);
    #endif

    #ifdef OUTPUT_BINARY_ACCELGYRO
        Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 0xFF));
        Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
        Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
        Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 0xFF));
        Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 0xFF));
        Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 0xFF));
    #endif


}

void temperatureTest(int COUNT) {
  int i;
  float total,average;
 
  if (findTemperatureSensor()) {
    temperature=readTemperature();
    for(i=0;i<COUNT;i++) {
      temperature=readTemperature();
      total+=temperature;
     // Serial.print("Temperature = ");Serial.println(temperature);
     Serial.print(" .");
    }
    Serial.println(" ");
    average=total/COUNT;
    //Serial.print  ("Average     = ");Serial.println(average);
    Serial.print  ("Result    = ");Serial.println(average);
    if ((average <120)&&(average>40)) {
        Serial.println("Test 1: Pass");
    } else {
        Serial.println("Test 1: Failed");
    }
  } else {
    Serial.println("Test 1: Failed");
  }
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
  tempsensor.shutdown_wake(0);   // Don't remove this line! required before reading temp
  delay(300);                    // wait 300ms so that the sensor is ready for reading. Otherwise, the temperature won't change.
  t = tempsensor.readTempC()*1.8 + 32.0;
  tempsensor.shutdown_wake(1);   // shutdown MSP9808 - power consumption ~0.1 micro Ampere
  return t;
}





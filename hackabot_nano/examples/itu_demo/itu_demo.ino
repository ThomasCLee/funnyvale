// GPS 
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#define pinGPSTX 4
#define pinGPSRX 5
TinyGPS gps;
SoftwareSerial ss(pinGPSTX, pinGPSRX);

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);


// pins controller motors
#define motor1_pos 3
#define motor1_neg 10
#define motor2_pos 6
#define motor2_neg 9
#define motor_en   A2
#define TRIGGER_PIN  A1 // (A1) Arduino pin tied to trigger pin on the ultrasonic sensor. 
#define ECHO_PIN     A0  // (A0) Arduino pin tied to echo pin on the ultrasonic sensor.
float distance;

void setup()
{
  Serial.begin(9600);       //Baud rate of Bluetooth HC06
  ss.begin(9600);           // start GPS
  setupMotor();             // setup motor
}

void loop()
{  
 reportGPS();
  
  delay(100);
  int readData;

  
  
  if (Serial.available()) {
    readData=Serial.read();
    Serial.println("Read = "+readData);   
    switch(readData) {
        case 'F': 
        case 'f': 
          enableMotor();
          robotForward(200);
          break;
        case 'B': 
        case 'b': 
          enableMotor();
          robotBackward(200);
          break;
        case 'L': 
        case 'l': 
          enableMotor();
          robotLeft(200);
          break;
        case 'R': 
        case 'r': 
          enableMotor();
          robotRight(200);
          break;
        case 'G':
        case 'g':
          reportGPS();
          break;
          
    }
    robotStop(50);
  }
}

void reportGPS() {
  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
  // get Latitude and Longitude
  gps.f_get_position(&flat, &flon, &age); 

     
  
  gps.satellites();
  smartdelay(0);
  gps.hdop();
  smartdelay(0);
 if (TinyGPS::GPS_INVALID_F_ANGLE == flat) {
     Serial.println("Unable to get Latitude from GPS");
  } else {
 //    Serial.println("Lat = " + gpsPrintString(flat));
  }
  smartdelay(0);
  if (TinyGPS::GPS_INVALID_F_ANGLE == flon) {
   Serial.println("Unable to get Longitude from GPS");
  } else {
     //Serial.println("Long = " + gpsPrintString(flon));
  }  

  float d;
  d=getDistance();

  //Serial.println("Lat,Long = " +gpsPrintString(flat) + "," +gpsPrintString(flon)+", Distance = "+gpsPrintString(d));
  Serial.println("Lat,Long = " +gpsPrintString(flat) + "," +gpsPrintString(flon));
  smartdelay(100);
}

String gpsPrintString(float number) {
    char str[12];
    dtostrf(number,11,6,str);
    return String(str);
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

static void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartdelay(0);
}

static void print_date(TinyGPS &gps)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ******** ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
        month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  smartdelay(0);
}

static void print_str(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartdelay(0);
}


//-----------------------------------------------------------------------------------------------------
// motor
//-----------------------------------------------------------------------------------------------------
void setupMotor() {
  pinMode(motor1_pos,OUTPUT);
  pinMode(motor1_neg,OUTPUT);
  pinMode(motor2_pos,OUTPUT);
  pinMode(motor2_neg,OUTPUT);
  pinMode(motor_en,OUTPUT);
  enableMotor();
  robotStop(50);
}

void enableMotor() {
  //Turn on the motor driver chip : L293D
  digitalWrite(motor_en, HIGH); 
}

void disableMotor() {
  //Turn off the motor driver chip : L293D
  digitalWrite(motor_en, LOW); 
}

void robotStop(int ms){
  digitalWrite(motor1_pos, LOW); 
  digitalWrite(motor1_neg, LOW); 
  digitalWrite(motor2_pos, LOW); 
  digitalWrite(motor2_neg, LOW); 
  delay(ms);
}

void robotForward(int ms){
  digitalWrite(motor1_pos, HIGH); 
  digitalWrite(motor1_neg, LOW); 
  digitalWrite(motor2_pos, HIGH); 
  digitalWrite(motor2_neg, LOW); 
  delay(ms);
}

void robotBackward(int ms){
  digitalWrite(motor1_pos, LOW); 
  digitalWrite(motor1_neg, HIGH); 
  digitalWrite(motor2_pos, LOW); 
  digitalWrite(motor2_neg, HIGH); 
  delay(ms);
}
void robotRight(int ms){
  digitalWrite(motor1_pos, LOW); 
  digitalWrite(motor1_neg, HIGH); 
  digitalWrite(motor2_pos, HIGH); 
  digitalWrite(motor2_neg, LOW); 
  delay(ms);
}

void robotLeft(int ms){
  digitalWrite(motor1_pos, HIGH); 
  digitalWrite(motor1_neg, LOW); 
  digitalWrite(motor2_pos, LOW); 
  digitalWrite(motor2_neg, HIGH); 
  delay(ms);
}

//-----------------------------------------------------------------------------------------------------
// Ultrasonic Distance Sensor
//-----------------------------------------------------------------------------------------------------
float getDistance() {
   digitalWrite(TRIGGER_PIN, LOW); 
   delayMicroseconds(2); 
   digitalWrite(TRIGGER_PIN, HIGH);
   delayMicroseconds(10); 
   digitalWrite(TRIGGER_PIN, LOW);
   return (  pulseIn(ECHO_PIN, HIGH) *0.017);
}


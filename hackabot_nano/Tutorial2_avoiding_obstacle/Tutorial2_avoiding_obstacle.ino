/***************************************************************************************************
 * This sample code is designed for Hackabot Nano (Arduino Compatible Robotic Kit)
 * 
 * In this example, the robot measure the distance from an obstacle ahead. 
 * If it is too close (within 10cm), the robot moves backward and then randomly turn left or right slightly.
 * 
 * 
 * For more information about Hackabot Nano, please check out our web site:
 * http://www.hackarobot.com
 * 
 * For more detailed description of this example, please check out our Instructables here:
 * http://www.instructables.com/id/Arduino-Robot-that-avoids-human
 * 
 * Comments, questions? Please check out our Google+ community:
 * https://plus.google.com/u/0/communities/111064186308711245928
 * 
 **************************************************************************************************/

// pins controller motors
#define motor1_pos 3
#define motor1_neg 10
#define motor2_pos 6
#define motor2_neg 9
#define motor_en   A2
#define TRIGGER_PIN  15 // (A1) Arduino pin tied to trigger pin on the ultrasonic sensor. 
#define ECHO_PIN     14  // (A0) Arduino pin tied to echo pin on the ultrasonic sensor.
float distance;


void setup()
{
   Serial.begin(57600);
   setupMotor();
}


void loop() 
{
   distance = getDistance();
   Serial.print("Distance = ");
   Serial.print(distance);
   Serial.println("cm");
   if (distance < 10) {
       robotBackward(300);
       if (random(2)==0) {
         robotRight(200);
       } else {
         robotLeft(200);
       }
     }
   robotStop(50);
}
   
void setupMotor() {
  pinMode(motor1_pos,OUTPUT);
  pinMode(motor1_neg,OUTPUT);
  pinMode(motor2_pos,OUTPUT);
  pinMode(motor2_neg,OUTPUT);
  pinMode(motor_en,OUTPUT);
  enableMotor();
  robotStop(50);
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
//-----------------------------------------------------------------------------------------------------
// motor
//-----------------------------------------------------------------------------------------------------


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

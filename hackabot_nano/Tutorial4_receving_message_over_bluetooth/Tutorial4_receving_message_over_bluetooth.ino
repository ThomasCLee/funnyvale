/***************************************************************************************************
 * This sample code is designed for Hackabot Nano (Arduino Compatible Robotic Kit)
 * 
 * Tutorial 4: 
 * In this example, the robot receives commands through bluetooth. It moves forward, backward,
 * left, right or stops based on the command received.
 * 
 * Please refer to tutorial 3 and the following article on how to connect bluetooth module:
 * http://hackarobot.com/arduino-bluetooth-example-tutorial-1/
 * 
 * For more information about Hackabot Nano, please check out our web site:
 * http://www.hackarobot.com
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
#define actionTime 300            // move left,right,forward,backward for 300ms

void setup() 
{
    Serial.begin(9600);           // assmes that bluetooth module is connected.
    setupMotor();
}

void loop() {
  int ch;
  if (Serial.available()) {
    ch=Serial.read();
    switch(ch) {
      case 'f': 
        robotForward(actionTime);
        robotStop(actionTime);
        break;
      case 'b': 
        robotBackward(actionTime);
        robotStop(actionTime);
        break;
      case 'l':    
        robotLeft(actionTime);
        robotStop(actionTime);
        break;  
      case 'r':    
        robotRight(actionTime);
        robotStop(actionTime);
        break;
       case 's':
         robotStop(actionTime);
         break;
    }
  }
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



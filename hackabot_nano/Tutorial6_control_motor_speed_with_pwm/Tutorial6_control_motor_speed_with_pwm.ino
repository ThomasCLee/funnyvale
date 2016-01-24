/***************************************************************************************************
 * This sample code is designed for Hackabot Nano (Arduino Compatible Robotic Kit)
 * 
 * Tutorial 6:
 * 
 * In this example, you will learn how to apply PWM (pulse width modulation)
 * to control the speed of the motor
 * 
 * For more information about Hackabot Nano, please check out our web site:
 * http://www.hackarobot.com
 * 
 
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


void setup() {
  setupMotor();  // initialize the pins that contorl the motors
}

void loop() {
  // move forward slowly for 3 seconds
  robotForwardPWM(40,3000);
     
  // move forward quickly for 3 seconds
  robotForwardPWM(200,3000);
  
  // move backward slowly for 3 seconds
  robotBackwardPWM(40,3000);
  
  // move backward quickly for 3 seconds
  robotBackwardPWM(200,3000);
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

void robotForwardPWM(int pwmValue,int ms) {
  // pwmValue should be between 0 and 255
  // 255 - fasest
  // 0 - slowest
  analogWrite(motor1_pos, pwmValue); 
  digitalWrite(motor1_neg, LOW); 
  analogWrite(motor2_pos, pwmValue); 
  digitalWrite(motor2_neg, LOW); 
  delay(ms);  
}


void robotBackwardPWM(int pwmValue,int ms){
  digitalWrite(motor1_pos, LOW); 
  analogWrite(motor1_neg, pwmValue); 
  digitalWrite(motor2_pos, LOW); 
  analogWrite(motor2_neg, pwmValue); 
  delay(ms);
}


void robotStop(int ms){
  digitalWrite(motor1_pos, LOW); 
  digitalWrite(motor1_neg, LOW); 
  digitalWrite(motor2_pos, LOW); 
  digitalWrite(motor2_neg, LOW); 
  delay(ms);
}


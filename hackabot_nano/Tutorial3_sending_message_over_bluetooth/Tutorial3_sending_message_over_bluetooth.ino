/***************************************************************************************************
 * This sample code is designed for Hackabot Nano (Arduino Compatible Robotic Kit)
 * 
 * In this example, the program simply keep incrementing and printing the value of the counter. 
 * Since the serial port is connected to the bluetooth module, the messages printed could be viewed
 * on a tablet, phone or PC.
 * 
 * 
 * 
 * For more information about Hackabot Nano, please check out our web site:
 * http://www.hackarobot.com
 * 
 * For more detailed description of this example, please check out our Instructables here:
 * http://hackarobot.com/arduino-bluetooth-example-tutorial-1/
 * 
 * Comments, questions? Please check out our Google+ community:
 * https://plus.google.com/u/0/communities/111064186308711245928
 * 
 **************************************************************************************************/

int count;
void setup() {
  count=0;
  Serial.begin(9600);
}
void loop() {
  count++;
  Serial.println("Count = "+String(count));
  delay(3000);
}

/* Encoder Library - Basic Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

#include <Encoder.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder rEnc(14, 15);
Encoder lEnc(19, 18);
  //   avoid using pins with LEDs attached

const int EN_A = 46;
const int EN_B = 45;
const int A_1 = 23;
const int A_2 = 22;
const int B_1 = 24;
const int B_2 = 25;
long oldRPosition  = -999;
long oldLPosition  = -999;

void pin_init(){
  Serial.begin(57600);
  Serial.println("Basic Encoder Test:");
  pinMode(A_1,OUTPUT);
  pinMode(A_2,OUTPUT);
  pinMode(B_1,OUTPUT);
  pinMode(B_2,OUTPUT);
}

/*
*
* Function Name: forward
* Input: void
* Output: void
* Logic: To move the bot forward
* Example Call: forward()
*
*/
void forward()
{
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);
  digitalWrite(A_1, HIGH);
  digitalWrite(A_2, LOW);
  digitalWrite(B_1, HIGH);
  digitalWrite(B_2, LOW);
}

/*
*
* Function Name: backward
* Input: void
* Output: void
* Logic: To move the bot backward
* Example Call: backward()
*
*/
void backward()
{
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);
  digitalWrite(A_1, LOW);
  digitalWrite(A_2, HIGH);
  digitalWrite(B_1, LOW);
  digitalWrite(B_2, HIGH);
}

/*
*
* Function Name: stop
* Input: void
* Output: void
* Logic: To stop the bot
* Example Call: stop()
*
*/
void stop()
{
  digitalWrite(EN_A, LOW);
  digitalWrite(EN_B, LOW);
  digitalWrite(A_1, LOW);
  digitalWrite(A_2, LOW);
  digitalWrite(B_1, LOW);
  digitalWrite(B_2, LOW);
}

/*
*
* Function Name: soft_left
* Input: void
* Output: void
* Logic: To turn the bot to left softly
* Example Call: soft_left()
*
*/
void soft_left()
{
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);
  digitalWrite(A_1, LOW);
  digitalWrite(A_2, LOW);
  digitalWrite(B_1, HIGH);
  digitalWrite(B_2, LOW);
}

/*
*
* Function Name: soft_right
* Input: void
* Output: void
* Logic: To turn the bot to right softly
* Example Call: soft_right()
*
*/
void soft_right()
{
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);
  digitalWrite(A_1, HIGH);
  digitalWrite(A_2, LOW);
  digitalWrite(B_1, LOW);
  digitalWrite(B_2, LOW);
}

/*
*
* Function Name: left
* Input: void
* Output: void
* Logic: To turn the bot to left sharply
* Example Call: left()
*
*/
void left()
{
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);
  digitalWrite(A_1, LOW);
  digitalWrite(A_2, HIGH);
  digitalWrite(B_1, HIGH);
  digitalWrite(B_2, LOW);
}

/*
*
* Function Name: right
* Input: void
* Output: void
* Logic: To turn the bot to right sharply
* Example Call: right()
*
*/
void right()
{
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);
  digitalWrite(A_1, HIGH);
  digitalWrite(A_2, LOW);
  digitalWrite(B_1, LOW);
  digitalWrite(B_2, HIGH);
}

void left_angle(int angle) {
  lEnc.write(0);
  double threshold_angle = (3000/180) * angle;
  left();
  while(lEnc.read() < threshold_angle){
    continue;
  }
  stop();
}

void right_angle(int angle) {
  lEnc.write(0);
  double threshold_angle = (2800/180) * angle;
  right();
  while(lEnc.read() > -(threshold_angle)){
    continue;
  }
  stop();
}

void left_turn_wls()
{
  lEnc.write(0);
  right();
  while(lEnc.read() > -(1300)){
    continue;
  }
  stop();
}

void right_turn_wls()
{
  lEnc.write(0);
  left();
  while(lEnc.read() < 1300){
    continue;
  }
  stop();
}

void setup() {
  pin_init();
  left_turn_wls();
  delay(5000);
  right_turn_wls();
}

void forward_enc(int distance){
  int threshold = distance * 100;
  forward();
  while(lEnc.read() < threshold){
    forward();
  }
  stop();
}

void loop(){
  long newPosition = lEnc.read();
  if (newPosition != oldLPosition) {
    oldLPosition = newPosition;
    Serial.println(newPosition);
  }
}

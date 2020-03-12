#include <Servo.h>    
Servo servo1;  // create servo object to control a servo  
Servo servo2;  
const int EN_A = 46;
const int EN_B = 45;
const int A_1 = 6;
const int A_2 = 8;
const int B_1 = 7;
const int B_2 = 9;
int threshold = 500;
int rot_delay = 380;
int lspeed=255;
int rspeed=255;


void pin_init(){
  pinMode(EN_A,OUTPUT);
  pinMode(EN_B,OUTPUT);
  pinMode(A_1,OUTPUT);
  pinMode(A_2,OUTPUT);
  pinMode(B_1,OUTPUT);
  pinMode(B_2,OUTPUT);
  servo1.attach(53);   // attaches the servo on B0 to the servo object    
  servo2.attach(52);
  Serial.begin(57600);
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);

}

void forward()
{
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);
  digitalWrite(A_1, HIGH);
  digitalWrite(A_2, LOW);
  digitalWrite(B_1, HIGH);
  digitalWrite(B_2, LOW);
}

void backward()
{
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);
  digitalWrite(A_1, LOW);
  digitalWrite(A_2, HIGH);
  digitalWrite(B_1, LOW);
  digitalWrite(B_2, HIGH);
}

void stop1()
{
  digitalWrite(EN_A, LOW);
  digitalWrite(EN_B, LOW);
  digitalWrite(A_1, LOW);
  digitalWrite(A_2, LOW);
  digitalWrite(B_1, LOW);
  digitalWrite(B_2, LOW);
}

void soft_left()
{
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);
  digitalWrite(A_1, LOW);
  digitalWrite(A_2, LOW);
  digitalWrite(B_1, HIGH);
  digitalWrite(B_2, LOW);
}

void soft_right()
{
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);
  digitalWrite(A_1, HIGH);
  digitalWrite(A_2, LOW);
  digitalWrite(B_1, LOW);
  digitalWrite(B_2, LOW);
}

void l1(){
  digitalWrite(A_1, LOW);
  digitalWrite(A_2, HIGH);
}

void l2(){

}

void left()
{
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);
  digitalWrite(A_1, LOW);
  digitalWrite(A_2, LOW);
  digitalWrite(B_1, HIGH);
  digitalWrite(B_2, LOW);
  delay(rot_delay);
  digitalWrite(A_1, LOW);
  digitalWrite(A_2, HIGH);
  digitalWrite(B_1, LOW);
  digitalWrite(B_2, LOW);
  delay(rot_delay);
  stop1();
}

void right()
{
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);
  digitalWrite(A_1, HIGH);
  digitalWrite(A_2, LOW);
  digitalWrite(B_1, LOW);
  digitalWrite(B_2, HIGH);
}

void velocity(int left_speed, int right_speed){
  lspeed = left_speed;
  rspeed = right_speed;
}

void pick(unsigned char direction)
{
  if(direction=='L')
  { servo2.write(80);
    servo1.write(185);
    delay(800);
    servo2.write(180);
    delay(800);
    servo1.write(90);
    servo2.write(90);
  }
  else
  {
    servo2.write(100);
    servo1.write(5);
    delay(800);
    servo2.write(0);
    delay(800);
    servo1.write(90);
    servo2.write(90);
  }
  delay(500);
}

void drop(unsigned char direction) {
  if(direction=='R')
    {
      servo2.write(0);
      servo1.write(5);
      delay(800);
      servo2.write(85);
      delay(800);
      servo1.write(90);
      servo2.write(90);
    }
  else {
    servo2.write(180);
      servo1.write(1855);
      delay(800);
      servo2.write(95);
      delay(800);
      servo1.write(90);
      servo2.write(90);
   }
}

void forward_wls()
{
  int left_sensor, center_sensor, right_sensor, flag = 0;
  while (flag==0){
    left_sensor = analogRead(A1);
    center_sensor = analogRead(A2);
    right_sensor = analogRead(A3);
    
    if (left_sensor < threshold and center_sensor > threshold and right_sensor < threshold) {       // x < threshold -> white
      Serial.begin("FORWARD");
      forward();
      //velocity(200, 200);
      _delay_ms(25);
    }
    else if (left_sensor < threshold and center_sensor < threshold and right_sensor > threshold) {
      Serial.begin("SOFT RIGHT");
      soft_right();
      //velocity(125, 0);
      _delay_ms(25);
    }
    else if (left_sensor > threshold and center_sensor < threshold and right_sensor < threshold) {
      Serial.begin("SOFT LEFT");
      soft_left();
      //velocity(0, 125);
      _delay_ms(25);
    }

    else if ((left_sensor > threshold and center_sensor > threshold and right_sensor > threshold) || (left_sensor < threshold and center_sensor > threshold and right_sensor > threshold) || (left_sensor > threshold and center_sensor > threshold and right_sensor < threshold)) {
      Serial.begin("NODE DETECTED");
      forward();
      delay(200);
      stop1();
      flag = 1;
      break;
    }
  }
  stop1();
}

void right_turn_wls()
{
  Serial.println("LEFT TURN");
  left();
  _delay_ms(rot_delay);
  stop1();
}

void left_turn_wls()
{
  Serial.println("RIGHT TURN");
  right();
  _delay_ms(rot_delay);
  stop1();
}

void task_4(){
  forward_wls();
  right_turn_wls();
  forward_wls();
  right_turn_wls();
  forward_wls();
  pick('R');
  forward_wls();
  forward_wls();
  forward_wls();
  right_turn_wls();
  velocity(150, 150);
  forward_wls();
  velocity(255, 255);
  drop('H');
  right_turn_wls();
  right_turn_wls();
  forward_wls();
  left_turn_wls();
  forward_wls();
  pick('R');
  right_turn_wls();
  right_turn_wls();
  forward_wls();
  right_turn_wls();
  forward_wls();
  right_turn_wls();
  forward_wls();
  forward_wls();
  drop('L');
}

void setup(){
  pin_init();
  //task_4();
  forward();
  delay(1000);
  left();
  stop1();
   
}

void loop(){
  
  }

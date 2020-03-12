#include <Servo.h>    
Servo servo1;  // create servo object to control a servo  
Servo servo2;  
const int EN_A = 46;
const int EN_B = 45;
const int A_1 = 23;
const int A_2 = 22;
const int B_1 = 24;
const int B_2 = 25;
int threshold = 500;
int rot_delay = 390;

/*
*
* Function Name: pin_init
* Input: void
* Output: void
* Logic: To initialise all the pins
* Example Call: pin_init()
*
*/
void pin_init(){
  pinMode(A_1,OUTPUT);
  pinMode(A_2,OUTPUT);
  pinMode(B_1,OUTPUT);
  pinMode(B_2,OUTPUT);
  servo1.attach(53);   // attaches the servo on B0 to the servo object    
  servo2.attach(52);
  Serial.begin(57600);
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

/*
*
* Function Name: pick
* Input: Left or Right
* Output: void
* Logic: To pick the CM according to the position of the CM
* Example Call: pick('L')
*
*/
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

/*
*
* Function Name: drop
* Input: HighRise or LowRise
* Output: void
* Logic: To drop the CM according to the height of the Warehouse
* Example Call: drop('H')
*
*/
void drop(unsigned char direction) {
  if(direction=='L')
  {
      servo2.write(0);
      servo1.write(15);
      delay(800);
      servo2.write(85);
      delay(800);
      servo1.write(90);
      servo2.write(90);
    }
   else {
      servo2.write(180);
      servo1.write(60);
      delay(800);
      servo2.write(55);
      delay(800);
      servo1.write(90);
      servo2.write(90);
   }
   delay(500);
}

/*
*
* Function Name: forward_wls
* Input: void
* Output: void
* Logic: Uses white line sensors to go forward by a node 
* Example Call: forward_wls(); //Goes forward
*
*/
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
      _delay_ms(50);
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
      stop();
      flag = 1;
      break;
    }
  }
  stop();
}

/*
*
* Function Name: right_turn_wls
* Input: void
* Output: void
* Logic: Uses white line sensors to turn right until black line is encountered
* Example Call: right_turn_wls(); //Turns right until black line is encountered
*/
void right_turn_wls()
{
  Serial.println("LEFT TURN");
  left();
  _delay_ms(rot_delay);
  stop();
}

/*
*
* Function Name: left_turn_wls
* Input: void
* Output: void
* Logic: Uses white line sensors to turn left until black line is encountered
* Example Call: left_turn_wls(); //Turns right until black line is encountered
*
*/
void left_turn_wls()
{
  Serial.println("RIGHT TURN");
  right();
  _delay_ms(rot_delay);
  stop();
}

/*
*
* Function Name: task_4_zig()
* Input: void
* Output: void
* Logic: Uses white line sensors to complete task 4 using the ZigZag lines.
* Example Call: task_4_zig()
*/
void task_4_zig(){
  forward_wls();
  right_turn_wls();
  forward_wls();
  right_turn_wls();
  forward_wls();
  delay(1000);
  forward_wls();
  forward_wls();
  forward_wls();
  right_turn_wls();
  forward_wls();
  forward_wls();
  delay(1000);
  right_turn_wls();
  right_turn_wls();
  forward_wls();
  left_turn_wls();
  forward_wls();
  delay(1000);
  right_turn_wls();
  right_turn_wls();
  forward_wls();
  right_turn_wls();
  forward_wls();
  right_turn_wls();
  forward_wls();
  forward_wls();
}

/*
*
* Function Name: task_4()
* Input: void
* Output: void
* Logic: Uses white line sensors to complete task 4 without using the ZigZag lines.
* Example Call: task_4()
*/
void task_4(){
  forward_wls();
  right_turn_wls();
  forward_wls();
  right_turn_wls();
  forward_wls();
  pick('R');
  left_turn_wls();
  left_turn_wls();
  forward_wls();
  left_turn_wls();
  forward_wls();
  forward_wls();
  left_turn_wls();
  forward_wls();
  forward_wls();
  forward_wls();
  forward_wls();
  drop('H');
  left_turn_wls();
  left_turn_wls();
  forward_wls();
  forward_wls();
  forward_wls();
  forward_wls();
  right_turn_wls();
  forward_wls();
  forward_wls();
  right_turn_wls();
  forward_wls();
  forward_wls();
  forward_wls();
  pick('R');
  left_turn_wls();
  left_turn_wls();
  forward_wls();
  forward_wls();
  forward_wls();
  left_turn_wls();
  forward_wls();
  forward_wls();  
  left_turn_wls();
  forward_wls();
  forward_wls();  
  drop('L');
}
/*
*
* Function Name: setup
* Input: void
* Output: void
* Logic: The main function
* Example Call: setup()
*/
void setup(){
  pin_init();
  task_4();
}

void loop(){
  
  }
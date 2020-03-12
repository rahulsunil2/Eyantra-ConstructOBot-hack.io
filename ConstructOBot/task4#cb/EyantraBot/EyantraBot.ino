const int pwm = 2 ;  //initializing pin 2 as pwm
const int in_1 = 8 ;
const int in_2 = 9 ;

void setup()
{
  pinMode(pwm,OUTPUT) ;   //we have to set PWM pin as output
  pinMode(in_1,OUTPUT) ;  //Logic pins are also set as output
  pinMode(in_2,OUTPUT) ;
  analogWrite(pwm,150) ;
    /*setting pwm of the motor to 255
  we can change the speed of rotaion
  by chaning pwm input but we are only
  using arduino so we are using higest
  value to driver the motor  */
}

void forward()
{
  digitalWrite(in_1,HIGH) ;
  digitalWrite(in_2,LOW) ;
}

void stop(){    
  //For brake
  digitalWrite(in_1,HIGH) ;
  digitalWrite(in_2,HIGH) ;
}
void left(){
  digitalWrite(in_1,LOW) ;
  digitalWrite(in_2,HIGH) ;
}
void right(){
  digitalWrite(in_1,HIGH) ;
  digitalWrite(in_2,HIGH) ;
 }



/*
*
* Function Name: forward_wls
* Input: node
* Output: void
* Logic: Uses white line sensors to go forward by the number of nodes specified
* Example Call: forward_wls(2); //Goes forward by two nodes
*
*/
void forward_wls(unsigned char node)
{
  int left_sensor, center_sensor, right_sensor, nodeDetect = 0, flag = 0;
  printf("\nFORWARD BY %d NODE", node);
  while (flag==0){
    left_sensor = analogRead(A1);
    center_sensor = analogRead(A2);
    right_sensor = analogRead(A3);

    forward();
    if (left_sensor != 255 and center_sensor == 255 and right_sensor != 255) {       // x != 255 -> white
      forward();
    }
    else if (left_sensor != 255 and center_sensor != 255 and right_sensor == 255) {
      right();
      delay(50);
    }
    else if (left_sensor == 255 and center_sensor != 255 and right_sensor != 255) {
      left();
      delay(50);
    }

    else if ((left_sensor == 255 and center_sensor == 255 and right_sensor == 255)) {
      nodeDetect++;

      if (nodeDetect == node) {
        forward();
        delay(260);
        stop();
        break;
      }
      while ((left_sensor == 255 and center_sensor == 255 and right_sensor == 255)) {
        left_sensor = analogRead(A1);
        center_sensor = analogRead(A2);
        right_sensor = analogRead(A3);
        forward();
      }
    }
    else
      continue;
  }
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
void left_turn_wls(void)
{
  printf("\nLEFT TURN");
  left();
  delay(300);
  int left_sensor, center_sensor, right_sensor;
  left_sensor = analogRead(A1);
  center_sensor = analogRead(A2);
  right_sensor = analogRead(A3);

  while (left_sensor == 255 and center_sensor != 255 and right_sensor != 255)
  {
    left();
    left_sensor = analogRead(A1);
    center_sensor = analogRead(A2);
    right_sensor = analogRead(A3);
  }
  delay(75);
  
  while (!(left_sensor != 255 and center_sensor == 255 and right_sensor != 255)) {
    if (left_sensor != 255 and center_sensor != 255 and right_sensor == 255) {
      right();
      delay(50);
      }
    else if (left_sensor == 255 and center_sensor != 255 and right_sensor != 255) {
      left();
      delay(50);
    }
    left_sensor = analogRead(A1);
    center_sensor = analogRead(A2);
    right_sensor = analogRead(A3);
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
void right_turn_wls(void)
{
  printf("\nRIGHT TURN");
  right();
  delay(100);
  int left_sensor, center_sensor, right_sensor;
  left_sensor = analogRead(A1);
  center_sensor = analogRead(A2);
  right_sensor = analogRead(A3);

  while (left_sensor != 255 and center_sensor != 255 and right_sensor == 255)
  {
    right();
    left_sensor = analogRead(A1);
    center_sensor = analogRead(A2);
    right_sensor = analogRead(A3);
  }

  while (!(left_sensor != 255 and center_sensor == 255 and right_sensor != 255)) {
    if (left_sensor != 255 and center_sensor != 255 and right_sensor == 255) {
      right();
      delay(50);
    }
    else if (left_sensor == 255 and center_sensor != 255 and right_sensor != 255) {
      left();
      delay(50);
    }
    left_sensor = analogRead(A1);
    center_sensor = analogRead(A2);
    right_sensor = analogRead(A3);
  }

  stop();
}


/*
*
* Function Name: Task_1_1
* Input: void
* Output: void
* Logic: Use this function to encapsulate your Task 1.1 logic
* Example Call: Task_1_1();
*/
void loop()
{

  printf("\nForward by 1 node");
  forward_wls(1);

  printf("\nRight turn");
  right_turn_wls();

  printf("\nForward by 4 nodes");
  forward_wls(4);

  printf("\nLeft turn");
  left_turn_wls();

  for (int i = 0; i < 4; i++){
    printf("\nForward by 1 node");
    forward_wls(1);

    printf("\nLeft turn");
    left_turn_wls();

    printf("\nForward by 1 node");
    forward_wls(1);

    printf("\nRight turn");
    right_turn_wls();
  }
  
  printf("\nInverted Forward by 1 node");
  //forward_inv();

  printf("\nRight turn");
  right_turn_wls();

  printf("\nForward by 1 node");
  forward_wls(1);

  printf("\nLeft turn");
  left_turn_wls();

  printf("\nForward by 1 node");
  forward_wls(1);
  stop();


}

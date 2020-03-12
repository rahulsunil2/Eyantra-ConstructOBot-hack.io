#include <Servo.h>    
Servo servo1;  // create servo object to control a servo  
Servo servo2;  

void pin_init(){
  servo1.attach(53);   // attaches the servo on B0 to the servo object    
  servo2.attach(52);
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
}


void setup()    
{ 
  pin_init();
  pick('L');
}    

void loop(){     
  
 }  

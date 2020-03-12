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
int proxleft = 8; //K0
int proxright = 9 ;//K1

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

void drop(unsigned char direction,int rise)
{ 
  
  if(rise){
      servo2.write(180);
      servo1.write(60);
      delay(800);
      servo2.write(55);
      delay(800);
      servo1.write(90);
      servo2.write(90);

  }
  else{
      servo2.write(0);
      servo1.write(15);
      delay(800);
      servo2.write(85);
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
* Function Name: forward_wls_inv
* Input: void
* Output: void
* Logic: Uses white line sensors to go forward by a node 
* Example Call: forward_wls(); //Goes forward
*
*/
void forward_wls_inv()
{
  int left_sensor, center_sensor, right_sensor, flag = 0;
  while (flag==0){
    left_sensor = analogRead(A1);
    center_sensor = analogRead(A2);
    right_sensor = analogRead(A3);
    
    if (left_sensor > threshold and center_sensor < threshold and right_sensor > threshold) {       // x > threshold -< white
      Serial.begin("FORWARD");
      forward();
      //velocity(200, 200);
      _delay_ms(50);
    }
    else if (left_sensor > threshold and center_sensor > threshold and right_sensor < threshold) {
      Serial.begin("SOFT RIGHT");
      soft_right();
      //velocity(125, 0);
      _delay_ms(25);
    }
    else if (left_sensor < threshold and center_sensor > threshold and right_sensor > threshold) {
      Serial.begin("SOFT LEFT");
      soft_left();
      //velocity(0, 125);
      _delay_ms(25);
    }

    else if ((left_sensor < threshold and center_sensor < threshold and right_sensor < threshold) || (left_sensor > threshold and center_sensor < threshold and right_sensor < threshold) || (left_sensor < threshold and center_sensor < threshold and right_sensor > threshold)) {
      Serial.begin("NODE DETECTED");
      forward();
      delay(200);
      stop();
      flag = 1;
      drop_h5();
      break;
    }
  }
  stop();
}

void drop_h5(){
//  soft_right();
//  delay(rot_delay*1.5);
//  soft_left();
//  delay(rot_delay*1.5);
//  backward();
//  delay(500);
  
  right();
  delay(rot_delay*1.5);
  forward();
  delay(400);
  left();
  delay(rot_delay*1.5);
  stop();
  drop('L', 0);
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
###########################################################################################
#####################################  PATH PLANNING  #####################################
###########################################################################################
*/

/*
 * Coordinate List
 * First Row -> nodename
 * Second Row -> x-coordinate
 * Third Row -> y-cordinate
 * Index -> [nodeName][x][y]
 */
int coordinates[3][16] = {
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  {2, 1, 1, 1, 1, 1, 1, 1, 2, 3,  3,  3,  3,  3,  3,  3},
  {1, 1, 2, 3, 4, 5, 6, 7, 7, 7,  6,  5,  4,  3,  2,  1}
};

/*
 * Distance Graph
 * 16 Rows and 16 Columns for every node and the distance between them.
 * Index -> [Source][Target]
 * Eg: distance_graph[5][6] -> gives the shortest distance between 5 and 6.
 */
int distance_graph[16][16] = {
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 2, 1},
  {1, 0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 5, 4, 3, 2},
  {2, 1, 0, 1, 2, 3, 4, 5, 6, 6, 5, 4, 5, 5, 4, 3},
  {3, 2, 1, 0, 1, 2, 3, 4, 5, 5, 4, 3, 4, 5, 5, 4},
  {4, 3, 2, 1, 0, 1, 2, 3, 4, 4, 3, 2, 3, 4, 5, 5},
  {5, 4, 3, 2, 1, 0, 1, 2, 3, 3, 2, 1, 2, 3, 4, 5},
  {6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 3, 2, 3, 4, 5, 6},
  {7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 3, 4, 5, 6, 7},
  {8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7},
  {7, 7, 6, 5, 4, 3, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6},
  {6, 6 ,5, 4, 3, 2, 3, 3, 2, 1, 0, 1, 2, 3, 4, 5},
  {5, 5, 4, 3, 2, 1, 2, 3, 3, 2, 1, 0, 1, 2, 3, 4},
  {4, 5, 5, 4, 3, 2, 3, 4, 4, 3, 2, 1, 0, 1, 2, 3},
  {3, 4, 5, 5, 4, 3, 4, 5, 5, 4, 3, 2, 1, 0, 1, 2},
  {2, 3, 4, 5, 5, 4, 5, 6, 6, 5, 4, 3, 2, 1, 0, 1},
  {1, 2, 3, 4, 5, 5, 6, 7, 7, 6, 5, 4, 3, 2, 1, 0}
};

/*
 * Adjacency List
 * Index -> [nodename][nearestNode]
 * Eg: adjacency_list[3] -> gives the nearest nodes of 3.
 */
int adjacency_list[16][4] = {
  { 1, 15, -1, -1}, { 0,  2, -1, -1}, { 1,  3, -1, -1}, { 2,  4, -1, -1},
  { 3,  5, -1, -1}, { 4,  6, 11, -1}, { 5,  7, -1, -1}, { 6,  8, -1, -1},
  { 7,  9, -1, -1}, { 8, 10, -1, -1}, { 9, 11, -1, -1}, { 5, 10, 12, -1},
  {11, 13, -1, -1}, {12, 14, -1, -1}, {13, 15, -1, -1}, {14,  0, -1, -1}
};

/*
 * Movement Array
 * Index -> [movements]
 * Eg: movement_array -> gives the movements the bot should make to reach the target.
 */
int movement_array[16];

/*
 * Bot Orientation
 * North -> 1, East -> 2, South -> 3, West -> 4
 * The bot is initially facing South(3) from node0 after moving forward from START.
 */
int botOrientation = 3;

/*
 *
 * Function Name: nextNode
 * Input: currentNodeArray, targetNode
 * Output: minNode
 * Logic: Finds the nextNode the bot should move to reach the targetNode in the shortest way possible.
 * Example Call: nextNode({1, 15, -1, -1}, 3} -> it returns 1 as the bot should move towards node1 next to reach node3.
 *
 */
int nextNode(int currentNodeArray[], int targetNode) {
  int i = 0, minNode;
  int minDistance = 100;
  int distance;
  while (currentNodeArray[i] != -1) {
    distance = distance_graph[currentNodeArray[i]][targetNode];
    if (distance < minDistance) {
      minDistance = distance;
      minNode = currentNodeArray[i];
    }
    i++;
  }
  return minNode;
}

/*
 *
 * Function Name: botMover
 * Input: currentNode, nextNode
 * Output: void
 * Logic: Updates the botMover array based path guiding it to move forward by how many nodes and to take proper turns.
 * 1 -> forward_wls(1)
 * 2 -> left_turn_wls()
 * 3 -> right_turn_wls()
 *
 * Example Call: botMover(1, 2) -> if the bot is facing towards North at node1, then it moves forward to node2.
 *
 */
void botMover(int currentNode, int nextNode)
{

  int x = (coordinates[1][nextNode] - coordinates[1][currentNode]),
    y = (coordinates[2][nextNode] - coordinates[2][currentNode]);

  if (
    (currentNode == 1 && nextNode == 2) || (currentNode == 15 && nextNode == 0) ||
    (currentNode == 9 && nextNode == 10) || (currentNode == 7 && nextNode == 8)) {
    //printf("CASE 1");
    right_turn_wls();
    forward_wls();
  }
  else if (
    (currentNode == 1 && nextNode == 0) || (currentNode == 15 && nextNode == 14) ||
    (currentNode == 9 && nextNode == 8) || (currentNode == 7 && nextNode == 6)) {
    //printf("CASE 2");
    left_turn_wls();
    forward_wls();
  }

  else if (
    (y > 0 && botOrientation == 1) || (y < 0 && botOrientation == 3) ||
    (x > 0 && botOrientation == 2) || (x < 0 && botOrientation == 4)) {
    //printf("CASE 3");
    forward_wls();
  }
  else if (
    (y > 0 && botOrientation == 2) || (y < 0 && botOrientation == 4) ||
    (x > 0 && botOrientation == 3) || (x < 0 && botOrientation == 1)) {
    //printf("CASE 4");
    left_turn_wls();
    forward_wls();
  }
  else if (
    (y > 0 && botOrientation == 3) || (y < 0 && botOrientation == 1) ||
    (x > 0 && botOrientation == 4) || (x < 0 && botOrientation == 2)) {
    //printf("CASE 5");
    left_turn_wls();
    left_turn_wls();
    forward_wls();
  }
  else if (
    (y > 0 && botOrientation == 4) || (y < 0 && botOrientation == 2) ||
    (x > 0 && botOrientation == 1) || (x < 0 && botOrientation == 3)) {
    //printf("CASE 6");
    right_turn_wls();
    forward_wls();
  }

  if (y > 0)
    botOrientation = 1;
  else if (y < 0)
    botOrientation = 3;
  else if (x > 0)
    botOrientation = 2;
  else if (x < 0)
    botOrientation = 4;

}

/*
 *
 * Function Name: pathPlanning
 * Input: sourceNode, targetNode
 * Output: void
 * Logic: Updates the movement_array based on the shortest path that the bot should take.
 * Example Call: nextNode(1, 3} -> It updates the movement_array to {2, 3} which means move to node2 and then to node3.
 *
 */
void pathPlanning(int sourceNode, int targetNode)
{
  for (int i = 0; i < 16; i++)
    movement_array[i] = -1;
  int currentNode = sourceNode;
  movement_array[0] = currentNode;
  int i = 1;
  while (currentNode != targetNode)
  {
    currentNode = nextNode(adjacency_list[currentNode], targetNode);
    movement_array[i] = currentNode;
    i++;
  }
}

/*
 *
 * Function Name: followPathBot
 * Input: sourceNode, targetNode
 * Output: void
 * Logic: Plans a path for the bot and makes it run through that path
 * Example Call: followPathBot(0, 6) -> Moves the bot from 0 to 6
 *
 */
void followPathBot(int sourceNode, int targetNode) {
  int i = 1;
  printf("\nPATH FROM %d --> %d\n", sourceNode, targetNode);
  pathPlanning(sourceNode, targetNode);
  while (movement_array[i] != -1)
  {
    printf("\n%d -> %d\n", movement_array[i - 1], movement_array[i]);
    botMover(movement_array[i - 1], movement_array[i]);
    i++;
  }
}
/*
*
* Function Name: pickup_item
* Input: void
* Output: void
* Logic: Uses proximity sensors to pickup the item from the right Warehouse
* Example Call: pickup_item(); //Picks up the item from either right or left Warehouse.
*
*/

void pickup_item(void) {
  unsigned char right_IR, left_IR;
  left_IR = analogRead(proxleft);
  right_IR = analogRead(proxright);

  //printf("\nBOT ORIENTATION : %d\n", botOrientation);

//  printf("%d, %d, %d\n", front_IR, left_IR, right_IR);
//  if (left_IR > right_IR or botOrientation == 2)
//    right_turn_wls();
//  else
//    left_turn_wls();

  if ((left_IR > right_IR && botOrientation == 1) || (left_IR <= right_IR && botOrientation == 3))
    botOrientation = 4;
  else if ((left_IR <= right_IR && botOrientation == 1) || (left_IR > right_IR && botOrientation == 3))
    botOrientation = 2;

  forward_wls();
  if (left_IR > right_IR or botOrientation == 2)
    pick('R');
  else
    pick('L');
 // pick();
  left_turn_wls();
  forward_wls();
  //printf("\nBOT ORIENTATION : %d\n", botOrientation);
}

/*
*
* Function Name: drop_item
* Input: currentNode
* Output: void
* Logic: Places the item in the house
* Example Call: drop_item(3); // Place the item in house at node3
*
*/
void drop_item(int currentNode, int rise){
  forward_wls();
  if (currentNode < 7 && botOrientation == 1 || currentNode > 7 && botOrientation == 3 || currentNode == 8 && botOrientation == 4)
    drop('L', rise);
  else if (currentNode < 7 && botOrientation == 3 || currentNode > 7 && botOrientation == 1 || currentNode == 8 && botOrientation == 2)
    drop('R', rise);
  if (currentNode < 7)
    botOrientation = 2;
  else if (currentNode > 7)
    botOrientation = 4;
  else
    botOrientation = 1;

//  drop()
  left_turn_wls();
  forward_wls();
}

/*
*
* Function Name: transport
* Input: currentNode, pickUp, dropOff
* Output: void
* Logic: Uses calls the neccesary functions to do the operations of picking up the item and dropping it off at a point.
* Example Call: transport(3, 4, 15); //Moves to 3, Picks up the item from 3 and drops it off at 15.
*
*/
void transport(int currentNode, int pickUp, int dropOff, int rise) {
  followPathBot(currentNode, pickUp);
  pickup_item();
  followPathBot(pickUp, dropOff);
  drop_item(dropOff, rise);
}

/*
###########################################################################################
##################################   PATH PLANNING END   ##################################
###########################################################################################
*/


void task_5(int house_total_requirement[], int house_node[], int which_material[], int floor_array[]) {
  int i = 1, j, k, pos1 = 0, cm=0;
  forward_wls();
  for (i=0;i<(sizeof(house_node) / sizeof(int));i++)
  {
    j=house_total_requirement[i];
    for (k=0;k<j;k++)
    { 
      if(i!=4)
      {
        transport(pos1,which_material[cm++],house_node[i], floor_array[i]);
        pos1= house_node[i];   
      }
      else
      {//node H5
        stop();
        break;
      }
    }
  }
}

/*
*
* Function Name: setup
* Input: void
* Output: void
* Logic: The main function
* Example Call: setup()
*/
void config()
{
  pin_init();
  int floor_array[5] = {1,0,0,1,0}; //0 low-rise house and 1 for high-rise house
  int house_total_requirement[5] = {2,2,2,1,2}; //How many required at particular house
  int house_node[5]={3,13,5,11,8};
  int which_material[9] = {14,10,6,4,14,12,2,2,12}; //requirement of each house
  task_5(house_total_requirement, house_node, which_material, floor_array);
}

void setup(){
  forward_wls_inv();
}

void loop(){
  
}

//You are allowed to define your own function to fulfill the requirement of tasks
//Dont change the name of following functions

#include "CB_Task_1_Sandbox.h"

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
	unsigned char left_sensor, center_sensor, right_sensor, nodeDetect = 0;
	printf("\nFORWARD BY %d NODE", node);
	while (1)
	{	
		left_sensor = ADC_Conversion(1);
		center_sensor = ADC_Conversion(2);
		right_sensor = ADC_Conversion(3);

		forward();
		if (left_sensor != 255 and center_sensor == 255 and right_sensor != 255)       // x != 255 -> white
			//forward();
			velocity(150, 150);
		else if (left_sensor != 255 and center_sensor != 255 and right_sensor == 255) {
			//soft_right();
			velocity(125, 0);
			_delay_ms(50);
		}
		else if (left_sensor == 255 and center_sensor != 255 and right_sensor != 255) {
			//soft_left();
			velocity(0, 125);
			_delay_ms(50);
		}
		else if ((left_sensor == 255 and center_sensor == 255 and right_sensor == 255)) {
			nodeDetect++;
			//printf("\n%d", nodeDetect);
			//stop();
			//_delay_ms(10000);

			if (nodeDetect == node) {
				forward();
				velocity(200, 200);
				_delay_ms(260);
				stop();
				break;
			}
			while ((left_sensor == 255 and center_sensor == 255 and right_sensor == 255) /*or (left_sensor != 255 and center_sensor == 255 and right_sensor == 255) or (left_sensor == 255 and center_sensor == 255 and right_sensor != 255)*/) {
				left_sensor = ADC_Conversion(1);
				center_sensor = ADC_Conversion(2);
				right_sensor = ADC_Conversion(3);
				forward();
				velocity(200, 200);
			}
		}
		else
			continue;
	}
	stop();
	//_delay_ms(1000);
}

/*
*
* Function Name: forward_inv
* Input: void
* Output: void
* Logic: Uses white line sensors to go forward through the black block with white line.
* Example Call: forward_inv(); //Moves forward
*/

void forward_inv(void)
{
	unsigned char left_sensor, center_sensor, right_sensor, nodeDetect = 0;
	while (1)
	{
		left_sensor = ADC_Conversion(1);
		center_sensor = ADC_Conversion(2);
		right_sensor = ADC_Conversion(3);


		if (left_sensor != 255 and center_sensor == 255 and right_sensor != 255)
			forward();
		else if (left_sensor != 255 and center_sensor != 255 and right_sensor == 255) {
			//soft_right();
			velocity(125, 0);
			_delay_ms(50);
		}
		else if (left_sensor == 255 and center_sensor != 255 and right_sensor != 255) {
			//soft_left();
			velocity(0, 125);
			_delay_ms(50);
		}
		else if (left_sensor == 255 and center_sensor != 255 and right_sensor == 255) {
			forward();
			while (1) {
				if (left_sensor == 255 and center_sensor != 255 and right_sensor == 255)
					forward();
				else if (left_sensor == 255 and center_sensor == 255 and right_sensor != 255) {
					//soft_right();
					velocity(125, 0);
					_delay_ms(50);
				}
				else if (left_sensor != 255 and center_sensor == 255 and right_sensor == 255) {
					//soft_left();
					velocity(0, 125);
					_delay_ms(50);
				}
				else if (left_sensor != 255 and center_sensor == 255 and right_sensor != 255)
					break;
			}
		}
		else if ((left_sensor == 255 and center_sensor == 255 and right_sensor == 255)) {
			forward();
			_delay_ms(260);
			stop();
			break;
		}
		else
			continue;
	}
	stop();
}

/*
*
* Function Name : forward_prox
* Input : void
* Output : void
* Logic : Uses proximity sensors to go forward between the walls.
* Example Call : forward_prox(); //Moves forward
*/

void forward_prox(void)
{
	unsigned char left_sensor, center_sensor, right_sensor, front_IR, left_IR, right_IR, IR_diff;
	while (1)
	{
		left_sensor = ADC_Conversion(1);
		center_sensor = ADC_Conversion(2);
		right_sensor = ADC_Conversion(3);

		if (left_sensor != 255 and center_sensor == 255 and right_sensor != 255)       // x != 255 -> white
			forward();
		else if (left_sensor != 255 and center_sensor != 255 and right_sensor == 255) {
			//soft_right();
			velocity(125, 0);
			_delay_ms(50);
		}
		else if (left_sensor == 255 and center_sensor != 255 and right_sensor != 255) {
			//soft_left();
			velocity(0, 125);
			_delay_ms(50);
		}
		else if (left_sensor != 255 and center_sensor != 255 and right_sensor != 255) { //{Proximity Sensor
			printf("Entered Proximity Sensor");
			forward();

			while (left_sensor != 255 and center_sensor != 255 and right_sensor != 255) {
				left_sensor = ADC_Conversion(1);
				center_sensor = ADC_Conversion(2);
				right_sensor = ADC_Conversion(3);
				front_IR = ADC_Conversion(4);
				left_IR = ADC_Conversion(5);
				right_IR = ADC_Conversion(6);

				IR_diff = (left_IR - right_IR);

				if (left_IR == 32 || right_IR == 32) {
					continue;
				}
				else if (abs(IR_diff) < 10)
					forward();
				else if (IR_diff > 0) {
					//soft_right();
					velocity(100, 0);
					_delay_ms(10);
					forward();
					_delay_ms(50);
					velocity(0, 100);

				}
				else if (IR_diff < 0) {
					//soft_left();
					velocity(0, 100);
					_delay_ms(10);
					forward();
					_delay_ms(50);
					velocity(100, 0);
				}
				else if (left_sensor != 255 and center_sensor == 255 and right_sensor != 255)
					break;

				printf("%d, %d\n", left_IR, right_IR);
			}
			printf("%d, %d\n", left_IR, right_IR);
			stop();
		}
		/*else if ((left_sensor == 255 and center_sensor == 255 and right_sensor == 255)) {
			forward();
			_delay_ms(260);
			stop();
			break;
		}*/
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
	_delay_ms(250);
	unsigned char left_sensor, center_sensor, right_sensor;
	left_sensor = ADC_Conversion(1);
	center_sensor = ADC_Conversion(2);
	right_sensor = ADC_Conversion(3);

	while (left_sensor == 255 and center_sensor != 255 and right_sensor != 255)
	{
		left();
		left_sensor = ADC_Conversion(1);
		center_sensor = ADC_Conversion(2);
		right_sensor = ADC_Conversion(3);
	}
	
	while (!(left_sensor != 255 and center_sensor == 255 and right_sensor != 255)) {
		if (left_sensor != 255 and center_sensor != 255 and right_sensor == 255) {
			velocity(100, 0);
			_delay_ms(50);
			}
		else if (left_sensor == 255 and center_sensor != 255 and right_sensor != 255) {
			velocity(0, 100);
			_delay_ms(50);
		}
		left_sensor = ADC_Conversion(1);
		center_sensor = ADC_Conversion(2);
		right_sensor = ADC_Conversion(3);
	}

	stop();
	//_delay_ms(1000);
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
	_delay_ms(100);
	unsigned char left_sensor, center_sensor, right_sensor;
	left_sensor = ADC_Conversion(1);
	center_sensor = ADC_Conversion(2);
	right_sensor = ADC_Conversion(3);

	while (left_sensor != 255 and center_sensor != 255 and right_sensor == 255)
	{
		right();
		left_sensor = ADC_Conversion(1);
		center_sensor = ADC_Conversion(2);
		right_sensor = ADC_Conversion(3);
	}

	while (!(left_sensor != 255 and center_sensor == 255 and right_sensor != 255)) {
		if (left_sensor != 255 and center_sensor != 255 and right_sensor == 255) {
			velocity(100, 0);
			_delay_ms(50);
		}
		else if (left_sensor == 255 and center_sensor != 255 and right_sensor != 255) {
			velocity(0, 100);
			_delay_ms(50);
		}
		left_sensor = ADC_Conversion(1);
		center_sensor = ADC_Conversion(2);
		right_sensor = ADC_Conversion(3);
	}

	stop();
	//_delay_ms(1000);
}

/*
###########################################################################################
#######################################PATH PLANNING#######################################
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
int movement_array[16] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

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
	while (currentNodeArray[i] != -1) {
		int distance = distance_graph[currentNodeArray[i]][targetNode];
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
		(currentNode == 9 && nextNode == 10) || (currentNode == 7 && nextNode == 8))
	{
		//printf("CASE 1");
		right_turn_wls();
		forward_wls(1);
	}
	else if (
		(currentNode == 1 && nextNode == 0) || (currentNode == 15 && nextNode == 14) ||
		(currentNode == 9 && nextNode == 8) || (currentNode == 7 && nextNode == 6))
	{
		printf("CASE 2");
		left_turn_wls();
		forward_wls(1);
	}

	else if (
		(y == 1 && botOrientation == 1) || (y == -1 && botOrientation == 3) ||
		(x == 1 && botOrientation == 2) || (x == -1 && botOrientation == 4))
	{
		printf("CASE 3");
		forward_wls(1);
	}
	else if (
		(y == 1 && botOrientation == 2) || (y == -1 && botOrientation == 4) ||
		(x == 1 && botOrientation == 3) || (x == -1 && botOrientation == 1))
	{
		printf("CASE 4");
		left_turn_wls();
		forward_wls(1);
	}
	else if (
		(y == 1 && botOrientation == 3) || (y == -1 && botOrientation == 1) ||
		(x == 1 && botOrientation == 4) || (x == -1 && botOrientation == 2))
	{
		printf("CASE 5");
		left_turn_wls();
		left_turn_wls();
		forward_wls(1);
	}
	else if (
		(y == 1 && botOrientation == 4) || (y == -1 && botOrientation == 2) ||
		(x == 1 && botOrientation == 1) || (x == -1 && botOrientation == 3))
	{
		printf("CASE 6");
		right_turn_wls();
		forward_wls(1);
	}

	if (y == 1)
		botOrientation = 1;
	else if (y == -1)
		botOrientation = 3;
	else if (x == 1)
		botOrientation = 2;
	else
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
###########################################################################################
#####################################PATH PLANNING END#####################################
###########################################################################################
*/

/*
*
* Function Name: e_shape
* Input: void
* Output: void
* Logic: Use this function to make the robot trace a e shape path on the arena
* Example Call: e_shape();
*/
void e_shape(void)
{
	left();
	_delay_ms(500);
}


/*
*
* Function Name: Task_1_1
* Input: void
* Output: void
* Logic: Use this function to encapsulate your Task 1.1 logic
* Example Call: Task_1_1();
*/
void Task_1_1(void)
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
	forward_inv();

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

/*
*
* Function Name: Task_1_2
* Input: void
* Output: void
* Logic: Use this function to encapsulate your Task 1.2 logic
* Example Call: Task_1_2();
*/
void Task_1_2(void)
{
	forward_wls(1);
	pathPlanning(0, 12);
	int i = 0;
	printf("\n");
	while (movement_array[i] != -1) {
		printf("%d ", movement_array[i]);
		i++;
	}
	i = 1;
	while (movement_array[i] != -1) {
		printf("\n%d -> %d\n", movement_array[i - 1], movement_array[i]);
		botMover(movement_array[i - 1], movement_array[i]);
		i++;
	}
}  
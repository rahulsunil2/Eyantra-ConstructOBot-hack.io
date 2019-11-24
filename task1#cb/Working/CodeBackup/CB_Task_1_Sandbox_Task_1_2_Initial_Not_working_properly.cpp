//You are allowed to define your own function to fulfill the requirement of tasks
//Dont change the name of following functions

/*
 * Team Id :			#0366
 *
 * Author List :		Advaith U, Rahul Sunil, Kesia Mary Joies, Alen S
 *
 * Filename:			CB_Task_1_Sandbox.cpp
 *
 * Theme:				ConstructOBot
 *
 * Functions:			forward_wls, forward_inv, left_turn_wls, right_turn_wls, e_shape, Task_1_1, Task_1_2
 *
 * Global Variables:	left_sensor, right_sensor, center_sensor
 *
 */

#include "CB_Task_1_Sandbox.h"

unsigned char left_sensor, center_sensor, right_sensor, left_prox, front_prox, right_prox;

/*
 * Function Name:	read_sensor_values
 * Input:			Global variables left_sensor, center_sensor, right_sensor, left_prox, front_prox, right_prox
 * Output:			It updates global variables left_sensor, center_sensor, right_sensor, left_prox, front_prox, right_prox
 * Logic:			It reads all 6 sensor values by using function ADC_Conversion()
 * Example Call:	read_sensor_values()
 */
void read_sensor_values()
{
	left_sensor = ADC_Conversion(1);	//Getting data of Left WhiteLine Sensor
	center_sensor = ADC_Conversion(2);	//Getting data of Center WhiteLine Sensor
	right_sensor = ADC_Conversion(3);	//Getting data of Right WhiteLine Sensor
	left_prox = ADC_Conversion(5);	    //Getting data of Left Proximity Sensor
	front_prox = ADC_Conversion(4);   	//Getting data of Center Proximity Sensor
	right_prox = ADC_Conversion(6);	    //Getting data of Right Proximity Sensor
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
	int nodeDetect = 0;
	while (1)
	{	
		read_sensor_values();

		//printf("Prox Sensor : %d\n", prox_sensor);

		forward();
		if (left_sensor != 255 and center_sensor == 255 and right_sensor != 255)       // x != 255 -> white
			velocity(200, 200);
		else if (left_sensor != 255 and center_sensor != 255 and right_sensor == 255) {
			velocity(125, 0);
			_delay_ms(50);
		}
		else if (left_sensor == 255 and center_sensor != 255 and right_sensor != 255) {
			velocity(0, 125);
			_delay_ms(50);
		}
		else if ((left_sensor == 255 and center_sensor == 255 and right_sensor == 255)) {
			nodeDetect++;
			printf("\n%d", nodeDetect);

			if (nodeDetect == node) {
				forward();
				_delay_ms(260);
				stop();
				break;
			}
			while ((left_sensor == 255 and center_sensor == 255 and right_sensor == 255)) {
				read_sensor_values();
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
* Function Name: forward_inv
* Input: node
* Output: void
* Logic: Uses white line sensors to go forward through the black block with white line.
* Example Call: forward_inv(); //Moves forward
*/

void forward_inv(unsigned char node)
{
	int nodeDetect = 0;

	while (1)
	{
		read_sensor_values();


		if (left_sensor != 255 and center_sensor == 255 and right_sensor != 255)
			forward();

		else if (left_sensor != 255 and center_sensor != 255 and right_sensor == 255) {
			velocity(125, 0);
			_delay_ms(50);
		}

		else if (left_sensor == 255 and center_sensor != 255 and right_sensor != 255) {
			velocity(0, 125);
			_delay_ms(50);
		}

		else if ((left_sensor == 255 and center_sensor != 255 and right_sensor == 255) or (left_sensor == 255 and center_sensor == 255 and right_sensor != 255) or (left_sensor != 255 and center_sensor == 255 and right_sensor == 255)) {
			forward();
			while (1) {
				if (left_sensor == 255 and center_sensor != 255 and right_sensor == 255)
					velocity(150, 150);

				else if (left_sensor == 255 and center_sensor == 255 and right_sensor != 255) {
					velocity(125, 0);
					_delay_ms(50);
				}

				else if (left_sensor != 255 and center_sensor == 255 and right_sensor == 255) {
					velocity(0, 125);
					_delay_ms(50);
				}

				else if (left_sensor != 255 and center_sensor == 255 and right_sensor != 255)
					break;

				else if ((left_sensor != 255 and center_sensor != 255 and right_sensor != 255)) {
					nodeDetect++;
					printf("\n%d", nodeDetect);

					while ((left_sensor != 255 and center_sensor != 255 and right_sensor != 255)) {
						read_sensor_values();
						forward();
					}
				}

				read_sensor_values();
			}
		}

		else if ((left_sensor == 255 and center_sensor == 255 and right_sensor == 255)) {
			forward();
			_delay_ms(260);
			stop();
			break;
		}
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
	left();
	_delay_ms(250);
	read_sensor_values();

	while (left_sensor == 255 and center_sensor != 255 and right_sensor != 255)
	{
		left();
		read_sensor_values();
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
		read_sensor_values();
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
	right();
	_delay_ms(100);
	read_sensor_values();

	while (left_sensor != 255 and center_sensor != 255 and right_sensor == 255)
	{
		right();
		read_sensor_values();
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
		read_sensor_values();
	}

	stop();
}

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
	forward_inv(0);

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
	left_turn_wls();
	forward_wls(1);
	left_turn_wls();
	forward_wls(4);
	left_turn_wls();
	forward_wls(1);
	//forward_inv(2);
}  
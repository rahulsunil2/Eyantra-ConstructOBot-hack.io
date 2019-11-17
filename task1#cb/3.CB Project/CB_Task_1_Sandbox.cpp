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
	while (1)
	{	
		left_sensor = ADC_Conversion(1);
		center_sensor = ADC_Conversion(2);
		right_sensor = ADC_Conversion(3);

		if (left_sensor != 255 and center_sensor == 255 and right_sensor != 255)       // x != 255 -> white
			forward();
		else if (left_sensor != 255 and center_sensor != 255 and right_sensor == 255) {
			//soft_right();
			velocity(150, 0);
			_delay_ms(50);
		}
		else if (left_sensor == 255 and center_sensor != 255 and right_sensor != 255) {
			//soft_left();
			velocity(0, 150);
			_delay_ms(50);
		}
		else if ((left_sensor == 255 and center_sensor == 255 and right_sensor == 255)) {
			nodeDetect++;
			printf("\n%d", nodeDetect);
			//stop();
			//_delay_ms(10000);

			if (nodeDetect == node) {
				forward();
				_delay_ms(200);
				stop();
				break;
			}
			while ((left_sensor == 255 and center_sensor == 255 and right_sensor == 255) /*or (left_sensor != 255 and center_sensor == 255 and right_sensor == 255) or (left_sensor == 255 and center_sensor == 255 and right_sensor != 255)*/) {
				left_sensor = ADC_Conversion(1);
				center_sensor = ADC_Conversion(2);
				right_sensor = ADC_Conversion(3);
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
	left();
	_delay_ms(250);
	unsigned char left_sensor, center_sensor, right_sensor;
	left_sensor = ADC_Conversion(1);
	center_sensor = ADC_Conversion(2);
	right_sensor = ADC_Conversion(3);

	while (left_sensor != 255 and center_sensor != 255 and right_sensor != 255)
	{
		left();
		left_sensor = ADC_Conversion(1);
		center_sensor = ADC_Conversion(2);
		right_sensor = ADC_Conversion(3);
	}
	
	while (!(left_sensor != 255 and center_sensor == 255 and right_sensor != 255)) {
		if (left_sensor != 255 and center_sensor != 255 and right_sensor == 255) {
			soft_right();
			_delay_ms(50);
			}
		else if (left_sensor == 255 and center_sensor != 255 and right_sensor != 255) {
			soft_left();
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
	right();
	_delay_ms(100);
	unsigned char left_sensor, center_sensor, right_sensor;
	left_sensor = ADC_Conversion(1);
	center_sensor = ADC_Conversion(2);
	right_sensor = ADC_Conversion(3);

	while (left_sensor != 255 and center_sensor != 255 and right_sensor != 255)
	{
		right();
		left_sensor = ADC_Conversion(1);
		center_sensor = ADC_Conversion(2);
		right_sensor = ADC_Conversion(3);
	}

	while (!(left_sensor != 255 and center_sensor == 255 and right_sensor != 255)) {
		if (left_sensor != 255 and center_sensor != 255 and right_sensor == 255) {
			soft_right();
			_delay_ms(50);
		}
		else if (left_sensor == 255 and center_sensor != 255 and right_sensor != 255) {
			soft_left();
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

	for (int i = 0; i < 3; i++){
		printf("\nForward by 1 node");
		forward_wls(1);

		printf("\nLeft turn");
		left_turn_wls();
		/*left();
		_delay_ms(500);
		stop;*/

		printf("\nForward by 1 node");
		forward_wls(1);

		printf("\nRight turn");
		right_turn_wls();
		/*right();
		_delay_ms(500);
		stop;*/
	}
	
	printf("\nForward by 1 node");
	forward_wls(1);
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
	//write your task 1.2 logic here
}  
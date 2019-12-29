
/* 
 * Team Id :			#1850
 * 
 * Author List :		Bharath T.U , Abhijeet C ,Gautham Rajesh , Joyal T John
 * 
 * Filename:			robot.c
 * 
 * Theme:				Homecoming
 * 
 * Functions:			get_point_cost, get_nearest_point, get_mapping_index, pick, drop, follow_black_line, follow_black_line_mm,
 *						turn_robot, change_direction, move_one_cell, match_column, match_row,
 *						go_to_coordinate, go_to_cell_no, pick_up, droping, print_input
 * 
 * Global Variables:	ADC_Value, Left_white_line, Center_white_line, Right_white_line,
 *						ShaftCountLeft, ShaftCountRight, Degrees, data, input_str, habitat_position_map, 
 *						animal_pos, habitat_pos, current_velocity, current_direction,
 *						current_coordinate, current_index, BNW_Thresh, left_velocity_float, 
 *						right_velocity_float, left_velocity, right_velocity,visited
 */

#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>	//included to support power function
#include "lcd.h"
#include <string.h>

void port_init();
void timer5_init();
void velocity(unsigned char, unsigned char);
void motors_delay();
char *str[50];

unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned char flag = 0;
unsigned char Left_white_line = 0;
unsigned char Center_white_line = 0;
unsigned char Right_white_line = 0;

volatile unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder
volatile unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
volatile unsigned int Degrees; //to accept angle in degrees for turning

char data; //to store received data from UDR1
char input_str[100] = ""; // stores the raw input string
void servo1_pin_config (void)
{
	DDRB  = DDRB | 0x20;  //making PORTB 5 pin output
	PORTB = PORTB | 0x20; //setting PORTB 5 pin to logic 1
}

//Configure PORTB 6 pin for servo motor 2 operation
void servo2_pin_config (void)
{
	DDRB  = DDRB | 0x40;  //making PORTB 6 pin output
	PORTB = PORTB | 0x40; //setting PORTB 6 pin to logic 1
}

//Function to configure LCD port
void lcd_port_config (void)
{
	DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
	PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}

//ADC pin configuration
void adc_pin_config (void)
{
	DDRF = 0x00;
	PORTF = 0x00;
	DDRK = 0x00;
	PORTK = 0x00;
}

//Function to configure ports to enable robot's motion
void motion_pin_config (void) 
{
	DDRA = DDRA | 0x0F;
	PORTA = PORTA & 0xF0;
	DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
	PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}


//Function to configure INT4 (PORTE 4) pin as input for the left position encoder
void left_encoder_pin_config (void)
{
	DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
	PORTE = PORTE | 0x10; //Enable internal pull-up for PORTE 4 pin
}

//Function to configure INT5 (PORTE 5) pin as input for the right position encoder
void right_encoder_pin_config (void)
{
	DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
	PORTE = PORTE | 0x20; //Enable internal pull-up for PORTE 4 pin
}

//Function to initialize Buzzer
void buzzer_pin_config (void)
{
	DDRC = DDRC | 0x08;		//Setting PORTC 3 as output
	PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}

//Function to configure Interrupt switch
void interrupt_switch_config (void)
{
	DDRE = DDRE & 0x7F;  //PORTE 7 pin set as input
	PORTE = PORTE | 0x80; //PORTE7 internal pull-up enabled
}

//Function to Initialize PORTS
void port_init()
{
	servo1_pin_config(); //Configure PORTB 5 pin for servo motor 1 operation
	servo2_pin_config(); //Configure PORTB 6 pin for servo motor 2 operation
	lcd_port_config();
	adc_pin_config();
	motion_pin_config();	
	left_encoder_pin_config(); //left encoder pin config
	right_encoder_pin_config(); //right encoder pin config
	buzzer_pin_config();
	interrupt_switch_config();
}

void left_position_encoder_interrupt_init (void) //Interrupt 4 enable
{
	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
	EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
	sei();   // Enables the global interrupt
}

void right_position_encoder_interrupt_init (void) //Interrupt 5 enable
{
	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
	EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
	sei();   // Enables the global interrupt
}


void buzzer_on (void)
{
	unsigned char port_restore = 0;
	port_restore = PINC;
	port_restore = port_restore | 0x08;
	PORTC = port_restore;
}

void buzzer_off (void)
{
	unsigned char port_restore = 0;
	port_restore = PINC;
	port_restore = port_restore & 0xF7;
	PORTC = port_restore;
}


//ISR for right position encoder
ISR(INT5_vect)
{
	ShaftCountRight++;  //increment right shaft position count
}


//ISR for left position encoder
ISR(INT4_vect)
{
	ShaftCountLeft++;  //increment left shaft position count
}


// Timer 5 initialized in PWM mode for velocity control
// Prescale:256
// PWM 8bit fast, TOP=0x00FF
// Timer Frequency:225.000Hz
void timer5_init()
{
	TCCR5B = 0x00;	//Stop
	TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
	OCR5AH = 0x00;	//Output compare register high value for Left Motor
	OCR5AL = 0xFF;	//Output compare register low value for Left Motor
	OCR5BH = 0x00;	//Output compare register high value for Right Motor
	OCR5BL = 0xFF;	//Output compare register low value for Right Motor
	OCR5CH = 0x00;	//Output compare register high value for Motor C1
	OCR5CL = 0xFF;	//Output compare register low value for Motor C1
	TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionality to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
	TCNT1H = 0xFC; //Counter high value to which OCR1xH value is to be compared with
	TCNT1L = 0x01;	//Counter low value to which OCR1xH value is to be compared with
	OCR1AH = 0x03;	//Output compare Register high value for servo 1
	OCR1AL = 0xFF;	//Output Compare Register low Value For servo 1
	OCR1BH = 0x03;	//Output compare Register high value for servo 2
	OCR1BL = 0xFF;	//Output Compare Register low Value For servo 2
	ICR1H  = 0x03;	
	ICR1L  = 0xFF;
	TCCR1A = 0xAB; /*{COM1A1=1, COM1A0=0; COM1B1=1, COM1B0=0; COM1C1=1 COM1C0=0}
 					For Overriding normal port functionality to OCRnA outputs.
				  {WGM11=1, WGM10=1} Along With WGM12 in TCCR1B for Selecting FAST PWM Mode*/
	TCCR1C = 0x00;
	TCCR1B = 0x0C; //WGM12=1; CS12=1, CS11=0, CS10=0 (Prescaler=256)
}

void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}

//Function For ADC Conversion
unsigned char ADC_Conversion(unsigned char Ch) 
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;  			
	ADMUX= 0x20| Ch;	   		
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}

//Function To Print Sensor Values At Desired Row And Column Location on LCD
void print_sensor(char row, char coloumn,unsigned char channel)
{
	ADC_Value = ADC_Conversion(channel);
	lcd_print(row, coloumn, ADC_Value, 3);
}

//Function for velocity control
void velocity (unsigned char left_motor, unsigned char right_motor)
{
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
	unsigned char PortARestore = 0;

	Direction &= 0x0F; 		// removing upper nibbel for the protection
	PortARestore = PORTA; 		// reading the PORTA original status
	PortARestore &= 0xF0; 		// making lower direction nibbel to 0
	PortARestore |= Direction; // adding lower nibbel for forward command and restoring the PORTA status
	PORTA = PortARestore; 		// executing the command
}

void forward (void) //both wheels forward
{
	motion_set(0x06);
}

void back (void) //both wheels backward
{
	motion_set(0x09);
}

void left (void) //Left wheel backward, Right wheel forward
{
	motion_set(0x05);
}

void right (void) //Left wheel forward, Right wheel backward
{
	motion_set(0x0A);
}

void stop (void)
{
	motion_set(0x00);
}


//Function used for turning robot by specified degrees
void angle_rotate(unsigned int Degrees)
{
	float ReqdShaftCount = 0;
	unsigned long int ReqdShaftCountInt = 0;

	ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
	ShaftCountRight = 0;
	ShaftCountLeft = 0;

	while (1)
	{
		if((ShaftCountRight >= ReqdShaftCountInt) | (ShaftCountLeft >= ReqdShaftCountInt))
		break;
	}
	stop(); //Stop robot
}

//Function used for moving robot forward by specified distance

void linear_distance_mm(unsigned int DistanceInMM)
{
	float ReqdShaftCount = 0;
	unsigned long int ReqdShaftCountInt = 0;

	ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned long int) ReqdShaftCount;
	
	ShaftCountRight = 0;
	while(1)
	{
		if(ShaftCountRight > ReqdShaftCountInt)
		{
			break;
		}
	}
	stop(); //Stop robot
}


void forward_mm(unsigned int DistanceInMM)
{
	forward();
	linear_distance_mm(DistanceInMM);
}

void back_mm(unsigned int DistanceInMM)
{
	back();
	linear_distance_mm(DistanceInMM);
}

void left_degrees(unsigned int Degrees)
{
	// 88 pulses for 360 degrees rotation 4.090 degrees per count
	left(); //Turn left
	angle_rotate(Degrees);
}


void right_degrees(unsigned int Degrees)
{
	// 88 pulses for 360 degrees rotation 4.090 degrees per count
	right(); //Turn right
	angle_rotate(Degrees);
}

//Function To Initialize UART2
// desired baud rate:9600
// actual baud rate:9600 (error 0.0%)
// char size: 8 bit
// parity: Disabled
void uart2_init(void)
{
	UCSR2B = 0x00; //disable while setting baud rate
	UCSR2A = 0x00;
	UCSR2C = 0x06;
	UBRR2L = 0x5F; //set baud rate lo
	UBRR2H = 0x00; //set baud rate hi
	UCSR2B = 0x98;
}

SIGNAL(SIG_USART2_RECV)			// ISR for receive complete interrupt
{		
	data = UDR2; 				// making copy of data from UDR2 in 'data' variable
	UDR2 = data; 				// echo data back to PC
	strcat(input_str, &data);	// concatenate each ASCII character received to string input_str
}
void servo_1(unsigned char degrees)
{
	float PositionPanServo = 0;
	PositionPanServo = ((float)degrees / 1.86) + 35.0;
	OCR1AH = 0x00;
	OCR1AL = (unsigned char) PositionPanServo;
}
void servo_2(unsigned char degrees)
{
	float PositionTiltServo = 0;
	PositionTiltServo = ((float)degrees / 1.86) + 35.0;
	OCR1BH = 0x00;
	OCR1BL = (unsigned char) PositionTiltServo;
}
//servo_free functions unlocks the servo motors from the any angle
//and make them free by giving 100% duty cycle at the PWM. This function can be used to
//reduce the power consumption of the motor if it is holding load against the gravity.

void servo_1_free (void) //makes servo 1 free rotating
{
	OCR1AH = 0x03;
	OCR1AL = 0xFF; //Servo 1 off
}

void servo_2_free (void) //makes servo 2 free rotating
{
	OCR1BH = 0x03;
	OCR1BL = 0xFF; //Servo 2 off
}


void init_devices (void)
{
 	cli(); //Clears the global interrupts
	port_init();
	lcd_set_4bit();
	lcd_init();
	uart2_init(); //Initialize UART2 for serial communication
	adc_init();
	timer5_init();
	left_position_encoder_interrupt_init();
	right_position_encoder_interrupt_init();
	sei();   //Enables the global interrupts
}
// #########################################################################################################
// ################################ my functions and variables start #######################################
// #########################################################################################################


// ################################ declaring global variables start #######################################

// mapping / setting cell co-ordinates in habitat
//	25 cells, 4 points in each cell, 2 index for row-column in each point
//	4 points for each cell are sorted in this order:  bottom-left,bottom-right,top-right,top-left 


int habitat_position_map[25][4][2] = 
{
{{1, 1}, {1, 2},  {2, 2}, {2, 1}},    {{1, 2}, {1, 3}, {2, 3}, {2, 2}},     {{1, 3}, {1, 4}, {2, 4}, {2, 3}},     {{1, 4}, {1, 5}, {2, 5}, {2, 4}},       {{1, 5}, {1, 6}, {2, 6}, {2, 5}},
{{2, 1}, {2, 2},  {3, 2}, {3, 1}},    {{2, 2}, {2, 3}, {3, 3}, {3, 2}},     {{2, 3}, {2, 4}, {3, 4}, {3, 3}},     {{2, 4}, {2, 5}, {3, 5}, {3, 4}},       {{2, 5}, {2, 6}, {3, 6}, {3, 5}},
{{3, 1}, {3, 2},  {4, 2}, {4, 1}},    {{3, 2}, {3, 3}, {4, 3}, {4, 2}},     {{3, 3}, {3, 4}, {4, 4}, {4, 3}},     {{3, 4}, {3, 5}, {4, 5}, {4, 4}},       {{3, 5}, {3, 6}, {4, 6}, {4, 5}},
{{4, 1}, {4, 2},  {5, 2}, {5, 1}},    {{4, 2}, {4, 3}, {5, 3}, {5, 2}},     {{4, 3}, {4, 5}, {5, 4}, {5, 3}},     {{4, 4}, {4, 5}, {5, 5}, {5, 4}},       {{4, 5}, {4, 6}, {5, 6}, {5, 5}},
{{5, 1}, {5, 2},  {6, 2}, {6, 1}},    {{5, 2}, {5, 3}, {6, 3}, {6, 2}},     {{5, 3}, {5, 4}, {6, 4}, {6, 3}},     {{5, 4}, {5, 5}, {6, 5}, {6, 4}},       {{5, 5}, {5, 6}, {6, 6}, {6, 5}}
};
int visited[25]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};				// a array to save the visited cell no if the a cell is visited the visited[cell_no-1] is set as 1 otherwise 0
char animal_pos[][3]={"","","","","","","","","","","","","","","","","","","",""}; // a 2D char matrix to store the locations of animals

char habitat_pos[][3]={"","","","","","","","","","","","","","","","","","","",""}; // a 2D char matrix to store the locations of habitat
	
unsigned char current_velocity = 127;	// value can be 0-255, default velocity 127
char current_direction = 'N';			// value can be E/W/N/S i.e. east, west, north or south respectively, Default N
int current_coordinate[2] = {-1, -1};	// co-ordinate of the cell i.e. row-column number from position map, initially -1, -1 as invalid
int current_index = -1;					// it stores the present index of the animal_pos & habita_pos at any instance it is also the control varible for the main loop
int BNW_Thresh = 15;					// black and white threshold value, default 28 [formula: ((W+B)/2)-(B/3)]

float left_velocity_float, right_velocity_float;	// stores velocity of left and right wheel respectively as float
unsigned char left_velocity, right_velocity;		// stores velocity of left and right wheel respectively as unsigned char
char pick_up_direction='N';							// stores the side from which the bot picked the animal
// ################################ declaring global variables end #########################################

/*
 * Function Name:	get_point_cost
 * Input:			current_point - int array containing row & column number of current coordinate point,
 *					target_point - int array containing row & column number of target coordinate point
 * Output:			total_cost - int which is the calculated cost between current and target coordinate point
 * Logic:			It calculates the cost of traveling between two points i.e. calculating row difference and  
 *					column difference and make sum of them
 * Example Call:	get_point_cost(current_point, target_point)
 */
int get_point_cost (int current_point[2], int target_point[2])
{
	int total_cost;	
	total_cost = abs(current_point[0] - target_point[0]) + abs(current_point[1] - target_point[1]);
	return total_cost;
}

// It receives current point and target cell which contains 4 points and returns nearest point from current point among those 4 points

/*
 * Function Name:	get_nearest_point
 * Input:			current_point - int array containing row & column number of current coordinate point,
 *					target_cell - 2D int array contains target cell's 4 points with coordinate based on row & column
 * Output:			nearest_point - int array containing row & column number of nearest coordinate point among 4 points of target_cell
 * Logic:			We are calculating traveling cost i.e. distance between current point and each of target cell's 4 points (top-left,
 *					top-right, bottom-right, bottom-left) and returning that point with lowest cost
 * Example Call:	get_nearest_point(current_coordinate, habitat_position_map[target_cell_no])
 */
int * get_nearest_point (int current_point[2], int target_cell[4][2]) 
{
	int * nearest_point = malloc(2 * sizeof(int));
	
	int i, current_cost, lowest_cost = 100;
	
	for (i=0; i<4; i++) 
	{
		current_cost = get_point_cost(current_point, target_cell[i]);
		if (current_cost < lowest_cost)
		{
			nearest_point[0] = target_cell[i][0];
			nearest_point[1] = target_cell[i][1];
			lowest_cost = current_cost;
		}
	}
	
	return nearest_point;
}


/*
 * Function Name:	get_mapping_index
 * Input:			coordinate: current coordinate that passed
 * Output:			index of coordinate in current cell's mapping
 * Logic:			to find the index point of a cell from 4 coordinate of a cell
 * Example Call:	get_nearest_point(current_coordinate)
 */

int get_mapping_index(int coordinate[])
{	int i,cell_no;
	cell_no=atoi((char*)habitat_pos[current_index])-1;	
	for(i=0;i<4;i++)
	{
		if(habitat_position_map[cell_no][i][0]==coordinate[0]&&habitat_position_map[cell_no][i][1]==coordinate[1])
		{
			return i;
		}
	}
}

/*
 * Function Name:	pick
 * Input:			direction: current direction that passed
 * Output:			mechanical movement of arm
 * Logic:			to move mechanical arm with help of servo1 and servo 2 so as to pick an animal
 *	                parameter 'direction' will accept the characters 'L'&'R' to pick from left and right
 *					respectively
 * Example Call:	pick('L')
 */

void pick(unsigned char direction)
{
	if(direction=='L')
	{	servo_2(80);
		servo_1(185);
		_delay_ms(800);
		servo_2(180);
		_delay_ms(800);
		servo_1(90);
		servo_2(90);
	}
	else
	{
		servo_2(100);
		servo_1(5);
		_delay_ms(800);
		servo_2(0);
		_delay_ms(800);
		servo_1(90);
		servo_2(90);
	}
}

/*
 * Function Name:	drop
 * Input:			direction: current direction that passed
 * Output:			mechanical movement of arm
 * Logic:			to move mechanical arm with help of servo1 and servo 2 to place an animal in a habitat
 *	                parameter 'direction' will accept the characters 'L'&'R' to pick from left and right
 *					respectively and the parameter 'stack' will accept the characters 1 & 0 to know if the 
 *					habitat already has a occupant or not respectively if it has a occupant the bot will stack 
 *					the animal(our team developed this code after uploading original task so the original task 
 *					didn't have the stacking feature)                
 * Example Call:	drop('L',1)
 */
void drop(unsigned char direction,int stack)
{	
	if(stack)
	{
		if(direction=='R')
		{
			servo_2(0);
			servo_1(40);
			_delay_ms(800);
			servo_2(120);
			_delay_ms(800);
			servo_2(90);
			servo_1(90);
		}
		else
		{
			servo_2(180);
			servo_1(150);
			_delay_ms(800);
			servo_2(60);
			_delay_ms(800);
			servo_2(90);
			servo_1(90);
			
		}	
	}
	else
	{
		if(direction=='R')
		{
			servo_2(0);
			servo_1(5);
			_delay_ms(800);
			servo_2(85);
			_delay_ms(800);
			servo_2(90);
			servo_1(90);
		}
		else
		{
			servo_2(180);
			servo_1(185);
			_delay_ms(800);
			servo_2(95);
			_delay_ms(800);
			servo_2(90);
			servo_1(90);
		
		}
	}
}

// It follows a 1cm thick black line on white surface

/*
 * Function Name:	read_wl_sensor_values
 * Input:			Global variables Left_white_line, Center_white_line, Right_white_line
 * Output:			It updates global variables Left_white_line, Center_white_line, Right_white_line
 * Logic:			It reads all 3 white line sensor values by using function ADC_Conversion()
 * Example Call:	read_wl_sensor_values()
 */
void read_wl_sensor_values () 
{
	Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
	Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
	Right_white_line = ADC_Conversion(1);	//Getting data of Right WL Sensor
}

/*
 * Function Name:	follow_black_line
 * Input:			Left_white_line, Center_white_line, Right_white_line are global variables,
 *					direction can be 'F' or 'B' as forward or backward respectively
 * Output:			It follows a 1cm thick black line on white surface
 * Logic:			To follow a 1cm black line, we can linearly forward the robot if all three white line sensors
 *					are on white surface or the center white line sensor is on the black surface. We decrease velocity of the
 *					right wheel and increase velocity of the left wheel if right white line sensor is on the black surface and
 *					other twos are on the white surface. Similarly we decrease velocity of the left wheel and increase velocity
 *					of the right wheel if left white line sensor is on the black surface and other twos are on the white surface.
 *					This way the robot follows a 1cm thick black line.
 *					
 * Example Call:	follow_black_line('F')
 */
void follow_black_line (char direction , int f) 
{
	flag = 0;
	if(f==0)		
	{
		if (((Left_white_line <= BNW_Thresh) && (Center_white_line <= BNW_Thresh) && (Right_white_line <= BNW_Thresh)) || (Center_white_line > BNW_Thresh)) 
		{
			flag = 1;
			if (direction == 'F') forward();
			else back();
			velocity(left_velocity, right_velocity);
		}

		if((Left_white_line <= BNW_Thresh) && (Center_white_line <= BNW_Thresh) && (Right_white_line > BNW_Thresh) && (flag == 0)) 
		{
			flag = 1;
			if (direction == 'F')
			 {
				forward();
				velocity(left_velocity+30, right_velocity-50);
			} 
			else 
			{
				back();
				velocity(left_velocity-50, right_velocity+30);
			}
		}

		if((Left_white_line > BNW_Thresh) && (Center_white_line <= BNW_Thresh) && (Right_white_line <= BNW_Thresh) && (flag == 0)) 
		{
			flag = 1;
			if (direction == 'F')
			 {
				forward();
				velocity(left_velocity-50, right_velocity+30);
			} 
			else 
			{
				back();
				velocity(left_velocity+30, right_velocity-50);
			}
		}
	}
	else
	{
		if (((Left_white_line <= BNW_Thresh) && (Center_white_line <= BNW_Thresh) && (Right_white_line <= BNW_Thresh)) || (Center_white_line > BNW_Thresh))
		{
			flag = 1;
			if (direction == 'F') forward();
			else back();
			velocity(255, 255);
		}

		if((Left_white_line <= BNW_Thresh) && (Center_white_line <= BNW_Thresh) && (Right_white_line > BNW_Thresh) && (flag == 0))
		{
			flag = 1;
			if (direction == 'F')
			{
				forward();
				velocity(255, 170);
			}
			else
			{
				back();
				velocity(170, 255);
			}
		}

		if((Left_white_line > BNW_Thresh) && (Center_white_line <= BNW_Thresh) && (Right_white_line <= BNW_Thresh) && (flag == 0))
		{
			flag = 1;
			if (direction == 'F')
			{
				forward();
				velocity(170, 255);
			}
			else
			{
				back();
				velocity(255, 170);
			}
		}	
	}	
}


// It follows a black line up to a fixed distance

/*
 * Function Name:	follow_black_line_mm
 * Input:			DistanceInMM - is a unsigned integer, 
 *					direction - is a char variable
 * Output:			It follows a 1cm thick black line up to a specific distance
 * Logic:			It calculates required shaft count to travel up to DistanceInMM
 *					and follows the black line. ShaftCountRight and ShaftCountLeft are initially
 *					set to 0. While moving the robot, we increase them and When the ShaftCountRight
 *					or ShaftCountLeft reaches as required, we stop the robot
 * Example Call:	follow_black_line_mm(100, 'F')
 */
void follow_black_line_mm (unsigned int DistanceInMM, char direction , int f)
 {
	float ReqdShaftCount = 0;
	unsigned long int ReqdShaftCountInt = 0;

	ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned long int) ReqdShaftCount;
	
	ShaftCountRight = 0;
	ShaftCountLeft = 0;
	while(1) 
	{
		read_wl_sensor_values();
		
		if(ShaftCountRight > ReqdShaftCountInt || ShaftCountLeft > ReqdShaftCountInt) {
			break;
		} 
		else 
		{
			follow_black_line(direction,f);
		}
	}
	
	stop(); //Stop robot	
}

// It turns the robot left/right by 5 degrees until encounters a black line.

/*
 * Function Name:	turn_robot
 * Input:			direction is a char variable which can be 'L' or 'R' as left or right respectively
 * Output:			This function rotates the robot perfect 90 degrees left or right
 * Logic:			At first it reads all white line sensor values and increase velocity to current velocity + 10
 *					left_degrees(90) is static and does not always as per hardware and physical limitations.We 
 *					rotate the robot by degrees got by the parameter 'deg' and towards the direction got in the 
 *					parameter 'direction'. 	
 * Example Call:	turn_robot('L',90)
 */
void turn_robot (char direction,int deg) 
{
	read_wl_sensor_values();
	velocity(current_velocity+10, current_velocity+10);
	
	if (direction == 'L')
	 {
		left_degrees(deg);
	} 
	else 
	{		
		right_degrees(deg);
	}
	velocity(current_velocity-10 ,current_velocity-10);
	_delay_ms(300);
}

// It changes the direction of the robot i.e. east/west/north/south.

/*
 * Function Name:	change_direction
 * Input:			desired_direction is a unsigned char variable which can be 'E'/'W'/'N'/'S' as
 *					ease, west, north or south respectively
 * Output:			It changes the robots direction to ease, west, north or south and updates its current 
 *					direction which is stored in global variable current_direction
 * Logic:			if current direction is north and desired direction is west then
 *						we turn the robot left by 90 degrees using turn_robot('L',90) and updates current direction to west
 *					if current direction is north and desired direction is east then
 *						we turn the robot right by 90 degrees using turn_robot('R',90) and updates current direction to east
 *					if current direction is north and desired direction is south then
 *						if current column is 0 then we turn the robot right by 90 degrees using turn_robot('R',180)
 *						and update current direction to south
 *					if current direction is south and desired direction is north then
 *						if current column is 0 then we turn the robot left by 90 degrees using turn_robot('L',180)
 *						and update current direction to north
 *					if current direction is south and desired direction is east then
 *						we turn the robot left by 90 degrees using turn_robot('L',90) and updates current direction to east
 *					if current direction is south and desired direction is west then
 *						we turn the robot right by 90 degrees using turn_robot('R',90) and updates current direction to west
 *					if current direction is east and desired direction is north then
 *						we turn the robot left by 90 degrees using turn_robot('L',90) and updates current direction to north	
 *					if current direction is east and desired direction is west then
 *						if current row is 0 then we turn the robot right by 90 degrees using turn_robot('R',180)
 *						and update current direction to west
 *					if current direction is east and desired direction is south then
 *						we turn the robot right by 90 degrees using turn_robot('R',90) and updates current direction to south
 *					if current direction is west and desired direction is north then
 *						we turn the robot right by 90 degrees using turn_robot('R',90) and updates current direction to north
 *					if current direction is west and desired direction is east then
 *						if current row is 0 then we turn the robot left by 90 degrees using turn_robot('L',180)
 *						and update current direction to east
 *					if current direction is west and desired direction is south then
 *						we turn the robot left by 90 degrees using turn_robot('L',90) and updates current direction to south
 * Example Call:	change_direction('N')
 */
void change_direction (unsigned char desired_direction) 
{
	if (current_direction == desired_direction) 
		return;
	if (current_direction == 'N' && desired_direction == 'W') 
	{ // north
		turn_robot('L',90);
		current_direction = 'W';
	} 
	else if (current_direction == 'N' && desired_direction == 'E') 
	{ // north
		turn_robot('R',90);
		current_direction = 'E';
	} 
	else if (current_direction == 'N' && desired_direction == 'S') 
	{ // north
		if (current_coordinate[1] == 0)
		{
			turn_robot('R',180);
		} 
		else 
		{
			turn_robot('L',180);
		}

		current_direction = 'S';
	} 
	else if (current_direction == 'S' && desired_direction == 'N') 
	{ //south
		if (current_coordinate[1] == 0) 
		{
			turn_robot('L',180);
		} 
		else 
		{
			turn_robot('R',180);
		}
		
		current_direction = 'N';
	} 
	else if (current_direction == 'S' && desired_direction == 'E') 
	{ //south
		turn_robot('L',90);
		current_direction = 'E';
	} 
	else if (current_direction == 'S' && desired_direction == 'W') 
	{ //south
		turn_robot('R',90);
		current_direction = 'W';
	} 
	else if (current_direction == 'E' && desired_direction == 'N') 
	{ //east
		turn_robot('L',90);
		current_direction = 'N';
	} 
	else if (current_direction == 'E' && desired_direction == 'W') 
	{ //east
		if (current_coordinate[0] == 0) 
		{
			turn_robot('R',180);	
		} 
		else 
		{
			turn_robot('L',180);
		}
		
		current_direction = 'W';
	} 
	else if (current_direction == 'E' && desired_direction == 'S') 
	{ //east
		turn_robot('R',90);	
		current_direction = 'S';
	} 
	else if (current_direction == 'W' && desired_direction == 'N') 
	{ //west
		turn_robot('R',90);	
		current_direction = 'N';
	} 
	else if (current_direction == 'W' && desired_direction == 'E') 
	{ //west
		if (current_coordinate[0] == 0) 
		{
			turn_robot('L',180);
		} 
		else 
		{
			turn_robot('R',180);	
		}
		
		current_direction = 'E';
	} 
	else if (current_direction == 'W' && desired_direction == 'S') 
	{ //west
		turn_robot('L',90);
		current_direction = 'S';
	}

}

// It follows a black line until encounter a 3x3 cm black square.

/*
 * Function Name:	move_one_cell
 * Input:			Reads global variables Left_white_line, Center_white_line, Right_white_line, BNW_Thresh		
 * Output:			It moved the robot 1 cell in forward direction i.e. robot forwards until get a 3x3 black square
 * Logic:			if the parameter d is 0 the bot is starting fom the other node so th bot will only check for the box after 27 cm
 *					If right and center white line sensors are on black surface and left white line sensor is on white
 *						surface, we also detect it as a 3x3cm black square.
 *					If left and center white line sensors are on the black surface and right white line sensor is on
 *						white surface, we detect it as a 3x3cm black square.
 *					Again, if all three white line sensors are on black surface, we also detect it as a 3x3cm black square.
 *					After all, forward the robot 5 cm to make the wheels on the 3x3cm black squares.					
 * Example Call:	move_one_cell(d)
 */
void move_one_cell (int d) 
{
	// the bellow if condition will prevent the bot from checking for the box until it is nearby to prevent misreading
	if(d==0)
	{
	follow_black_line_mm(270, 'F',1);
	}
	read_wl_sensor_values();
	while (!(((Left_white_line > BNW_Thresh) && (Center_white_line > BNW_Thresh)) || ((Center_white_line > BNW_Thresh) && (Right_white_line > BNW_Thresh)) // 1-2 or 3-2 on white
			|| ((Left_white_line > BNW_Thresh) && (Center_white_line > BNW_Thresh) && (Right_white_line > BNW_Thresh)))) 
	{ // center on black
		read_wl_sensor_values();
		follow_black_line('F',0);
	}
	buzzer_on()	;
	// this delay is important: if disabled, the bot may get out of line
	_delay_ms(200);
	buzzer_off();
	// adjust 10/2 cm forward to make the wheels on the 3x3cm black squares.
	forward_mm(50);
}

// It is a helper function, used to match columns.

/*
 * Function Name:	match_column
 * Input:			target_coordinate is a int array which holds a co-ordinate i.e. row, column number
 * Output:			It updates current co-ordinate's column number i.e. the global variable current_coordinate[1]
 * Logic:			Until current co-ordinate's column > target co-ordinates's column, we repeat the process indented below
 *						change direction to west by using change_direction('W') and then move the robot by one cell 
 *						forward by using move_one_cell(). After that we update current co-ordinate's column number by decreasing 1
 *					All after above, Until current co-ordinate's column < target co-ordinates's column, we repeat the process indented below
 *						change direction to east by using change_direction('E') and then move the robot by one cell
 *						forward by using move_one_cell(). After that we update current co-ordinate's column number by increase 1
 * Example Call:	match_column((int){1, 3})
 */
void match_column (int target_coordinate[]) 
{
	while (current_coordinate[1] > target_coordinate[1]) 
	{// go east/west until both position on same column
		change_direction('W');
		move_one_cell(0);
		current_coordinate[1]--;
	}
		
	while (current_coordinate[1] < target_coordinate[1]) 
	{// go east/west until both position on same column
		change_direction('E');
		move_one_cell(0);
		current_coordinate[1]++;
	}
}

// It is a helper function, used to match rows.

/*
 * Function Name:	match_row
 * Input:			target_coordinate is a int array which holds a co-ordinate i.e. row, column number
 * Output:			It updates current co-ordinate's row number i.e. the global variable current_coordinate[0]
 * Logic:			Until current co-ordinate's row > target co-ordinates's row, we repeat the process indented below
 *						change direction to north by using change_direction('N') and then move the robot by one cell 
 *						forward by using move_one_cell(). After that we update current co-ordinate's column number by decreasing 1
 *					All after above, Until current co-ordinate's row < target co-ordinates's row, we repeat the process indented below
 *						change direction to south by using change_direction('S') and then move the robot by one cell
 *						forward by using move_one_cell(). After that we update current co-ordinate's column number by increase 1
 * Example Call:	match_row((int){1, 3})
 */
void match_row (int target_coordinate[]) 
{
	while (current_coordinate[0] > target_coordinate[0]) 
	{// go north/south until both position on same row
		change_direction('S');
		move_one_cell(0);
		current_coordinate[0]--;
	}
	
	while (current_coordinate[0] < target_coordinate[0]) 
	{// go north/south until both position on same row
		change_direction('N');
		move_one_cell(0);
		current_coordinate[0]++;
	}
}

// It is used to move the robot to a particular coordinate.


/*
 * Function Name:	go_to_coordinate
 * Input:			target_coordinate is a int array which holds a co-ordinate i.e. row, column number
 * Output:			It moves the robot to a specific co-ordinate by using match_column and match_row functions
 * Logic:			it moves to the coordinate  w.r.t the current direction and finds the best set of motion
 * Example Call:	go_to_coordinate((int){6, 5})
 */
void go_to_coordinate (int target_coordinate[]) 
{

	if(current_coordinate[0]>target_coordinate[0]&&current_coordinate[1]>target_coordinate[1])
	{
	//if the target coordinate is SW to the current coordinate
		if (current_direction == 'N' || current_direction == 'W') 
		{ // match column then row
			match_column(target_coordinate);
			match_row(target_coordinate);
		} 
		else 
		{ // if current_direction = S or E  
			// match row then column
			match_row(target_coordinate);
			match_column(target_coordinate);
		}
	}	
	else if(current_coordinate[0]<target_coordinate[0]&&current_coordinate[1]<target_coordinate[1])
	{
		//if the target coordinate is NE to the current coordinate
		if (current_direction == 'S' || current_direction == 'E') 
		{ // match column then row
			match_column(target_coordinate);
			match_row(target_coordinate);
		} 
		else
		{ // if current_direction = N or W  
			// match row then column
			match_row(target_coordinate);
			match_column(target_coordinate);
		}
	}
	else if(current_coordinate[0]>target_coordinate[0]&&current_coordinate[1]<target_coordinate[1])
	{
		//if the target coordinate is SE to the current coordinate
		if (current_direction == 'N' || current_direction == 'E') 
		{ // match column then row
			match_column(target_coordinate);
			match_row(target_coordinate);
		} else 
		{ // if current_direction = S or W  
			// match row then column
			match_row(target_coordinate);
			match_column(target_coordinate);
		}
	}	
	else if(current_coordinate[0]<target_coordinate[0]&&current_coordinate[1]>target_coordinate[1])
	{
		//if the target coordinate is NW to the current coordinate
		if (current_direction == 'S' || current_direction == 'W') 
		{ // match column then row
			match_column(target_coordinate);
			match_row(target_coordinate);
		} 
		else 
		{ // if current_direction = N or E  
			// match row then column
			match_row(target_coordinate);
			match_column(target_coordinate);
		}
	}	
	else if(current_coordinate[0]<target_coordinate[0]&&current_coordinate[1]==target_coordinate[1])
	{
		//if the target coordinate is N to the current coordinate
		match_row (target_coordinate);
	}
	else if(current_coordinate[0]>target_coordinate[0]&&current_coordinate[1]==target_coordinate[1])
	{
		//if the target coordinate is S to the current coordinate
		match_row (target_coordinate);
	}
	else if(current_coordinate[0]==target_coordinate[0]&&current_coordinate[1]>target_coordinate[1])
	{
		//if the target coordinate is W to the current coordinate
		match_column(target_coordinate);
	}
	else if(current_coordinate[0]==target_coordinate[0]&&current_coordinate[1]<target_coordinate[1])
	{
		//if the target coordinate is SW to the current coordinate
		match_column(target_coordinate);
	}
}

// It is used to go to nearest co-ordinate point of a cell from current point's co-ordinate.

/*
 * Function Name:	go_to_cell_no
 * Input:
 *					target_cell_no - is an cell no
 * Output:			It moves the robot to nearest among 4 points (bottom-right, bottom-left, top-right,top-left)
 *					of a given cell number with target division
 * Logic:			It creates an integer pointer to store a co-ordinate. Then if the target division is 1 i.e. habitat_map
 *					then copy returned data from get_nearest_point(current_coordinate, habitat__map[target_cell_no])
 *					to nearest_point.
 * Example Call:	go_to_cell_no(2, 5)
 */
void go_to_cell_no (int target_cell_no) 
{
	int * nearest_point = (int *) malloc(2 * sizeof(int));
	memcpy(nearest_point, get_nearest_point(current_coordinate, habitat_position_map[target_cell_no-1]), 2 * sizeof(int));
	go_to_coordinate(nearest_point);
}
/*
 * Function Name:	pick_up
 * Input:			none
 * Output:			none
 * Logic:			go to the animal location according to the conditions and picks up the animal 
 * Example Call:	pick_up()
 */

void pick_up()
{
	int * nearest_point = (int *) malloc(2 * sizeof(int));
	memcpy(nearest_point, get_nearest_point(current_coordinate, habitat_position_map[atoi((char*)habitat_pos[current_index+1])-1]), 2 * sizeof(int));
	if(current_coordinate[0]==current_coordinate[1]||(current_coordinate[0]==1&&current_coordinate[1]==6)||(current_coordinate[0]==6&&current_coordinate[1]==1))
	{
		//corner condition 
		if(current_coordinate[0]==current_coordinate[1]&&current_coordinate[1]==1)
		{
			if(current_direction=='S') // if current direction is south turn bot 45 degree and go east
			{
				left_degrees(45);
				pick('R');
				left_degrees(45);
				current_direction='E';
				pick_up_direction='R';
			}
			else if(current_direction=='W') // if current direction is west turn bot 45 degree and go north
			{
				right_degrees(45);
				pick('L');
				right_degrees(45);
				current_direction='N';
				pick_up_direction='L';
			}
			
		}
		else if(current_coordinate[0]==current_coordinate[1]&&current_coordinate[1]==6)
		{
			if(current_direction=='N') // if current direction is north turn bot 45 degree and go west
			{
				left_degrees(45);
				pick('R');
				left_degrees(45);
				current_direction='W';
				pick_up_direction='R';
			}
			else if(current_direction=='E') // if current direction is east turn bot 45 degree and go south
			{
				right_degrees(45);
				pick('L');
				right_degrees(45);
				current_direction='S';
				pick_up_direction='L';
			}
			
		}
		else if(current_coordinate[0]!=current_coordinate[1]&&current_coordinate[0]==6)	
		{
			if(current_direction=='W') // if current direction is west turn bot 45 degree and go south
			{
				left_degrees(45);
				pick('R');
				left_degrees(45);
				current_direction='S';
				pick_up_direction='R';
			}
			else if(current_direction=='N') // if current direction is north turn bot 45 degree and go east
			{
				right_degrees(45);
				pick('L');
				right_degrees(45);
				current_direction='E';
				pick_up_direction='L';
			}
			
		}
		else if(current_coordinate[0]!=current_coordinate[1]&&current_coordinate[1]==6)
		{
			if(current_direction=='E')
			{
				left_degrees(45);
				pick('R');
				left_degrees(45);
				current_direction='N';
				pick_up_direction='R';
			}
			else if(current_direction=='S')
			{
				right_degrees(45);
				pick('L');
				right_degrees(45);
				current_direction='W';
				pick_up_direction='L';
			}
			
		}
	}
	else if(current_coordinate[1]==1)
	{
		if(current_direction=='N')
		{
			pick('L');
			pick_up_direction='L';
		}
		else if(current_direction=='S')
		{
			pick('R');
			pick_up_direction='R';
		}
		else if(current_direction=='W')
		{
			if(nearest_point[0]>current_coordinate[0])
				{
				change_direction('N');
				pick('L');
				pick_up_direction='L';
				}
			else
				{
				change_direction('S');
				pick('R');
				pick_up_direction='R';
				}
				
		}
	}
	else if(current_coordinate[0]==1)
	{
		if(current_direction=='W')
		{
			pick('L');
			pick_up_direction='L';
		}
		else if(current_direction=='E')
		{
			pick('R');
			pick_up_direction='R';
		}
		else if(current_direction=='S')
		{
			//right or left according to habitat
			if(current_coordinate[1]<nearest_point[1])
			{
				change_direction('E');
				pick('R');
				pick_up_direction='R';
			}
			else
			{
				change_direction('W');
				pick('L');
				pick_up_direction='L';
			}				
		}
	}
	else if(current_coordinate[1]==6)
	{
		if(current_direction=='S')
		{
			pick('L');
			pick_up_direction='L';
		}
		else if(current_direction=='N')
		{
			pick('R');
			pick_up_direction='R';
		}
		else if(current_direction=='E')
		{
			
			if(nearest_point[0]>current_coordinate[0])
				{
					change_direction('N');
					pick('R');
					pick_up_direction='R';
				}
			else
				{
					change_direction('S');
					pick('L');
					pick_up_direction='L';
				}					
		}
	}	
	else if(current_coordinate[0]==6)
	{
		if(current_direction=='E')
		{
			pick('L');
			pick_up_direction='L';
		}
		else if(current_direction=='W')
		{
			pick('R');
			pick_up_direction='R';
		}
		else if(current_direction=='N')
		{
			if(current_coordinate[1]<nearest_point[1])
				{
					change_direction('E');
					pick('L');
					pick_up_direction='L';
				}
			else
				{
					change_direction('W');
					pick('R');
					pick_up_direction='R';
				}
		}
	}
}

/*
 * Function Name:	dropping
 * Input:			none
 * Output:			none
 * Logic:			go to the habitat location according to the conditions and drops the animal 
 * Example Call:	dropping()
 */
void droping()
{
int index,cell_no,flag=0;

cell_no=atoi((char*)habitat_pos[current_index])-1;
index=get_mapping_index(current_coordinate);
//*********************************start************************************
// the bellow set of conditions are for changing the direction of the bot so as to drop the animal from the same side as it was picked
if(current_direction=='N')
{
	if(index==0 && pick_up_direction=='L')
		change_direction('E');
	else if(index==1 && pick_up_direction=='R')
		change_direction('W');
}
else if(current_direction=='S')
{
	if(index==2 && pick_up_direction=='L')
		change_direction('W');
	else if(index==3 && pick_up_direction=='R')
		change_direction('E');
}
else if(current_direction=='W')
{
	if(index==1 && pick_up_direction=='L')
		change_direction('N');
	else if(index==2 && pick_up_direction=='R')
		change_direction('S');
}
else if(current_direction=='E')
{
	if(index==0 && pick_up_direction=='R')
		change_direction('N');
	else if(index==3 && pick_up_direction=='L')
		change_direction('S');
}
//**********************************end***********************************
if(visited[cell_no]!=0)//to check if the cell is visited or not
	flag=1;
follow_black_line_mm(150,'F',1);// to go to the middle of the edge the habitat
//*******************************start**************************************
//the bellow set of conditions are for finding which side to drop the animal and drop the animal
if(index==0)
{
	if(current_direction=='N')
	{
		drop('R',flag);
	}
	else if(current_direction=='E')
	{
		drop('L',flag);
	}
}
else if(index==1)
{
	if(current_direction=='W')
	{
		drop('R',flag);
	}
	else if(current_direction=='N')
	{
		drop('L',flag);
	}
}
else if(index==2)
{
	if(current_direction=='S')
	{
		drop('R',flag);
	}
	else if(current_direction=='W')
	{
		drop('L',flag);
	}
}
else if(index==3)
{
	if(current_direction=='E')
	{
		drop('R',flag);
	}
	else if(current_direction=='S')
	{
		drop('L',flag);
	}
}
//*******************************end**************************************
}
/*
 * Function Name:	 print_input
 * Input:			none
 * Output:			none
 * Logic:			print animal and habitat location in lcd
 * Example Call:	 print_input()
 * this function was used in progress task not used now
 */

void print_input()
{
	int i=0,j=0;
	char animals[100],habitats[100];
	while(input_str[i]!='\n')
	{	if(input_str[i]!=' ')
		{habitats[j]=input_str[i];j++;}
		i++;
	}
	lcd_cursor(1,1);
	lcd_string(habitats);
	i++;
	j=0;
	while(input_str[i]!='#')
	{	if(input_str[i]!=' ')
		{
		animals[j]=input_str[i];
		j++;
		}
		i++;
	}
	lcd_cursor(2,1);
	lcd_string(animals);
}
// my functions and variables end ##########################################################################

//Main Function start ######################################################################################
/*
 * Function Name:	main
 * Input:			input_str - is a string which is a global variable. It contains comma separated numbers.
 * Output:			int to inform the caller that the program exited correctly or incorrectly (C code standard)
 * Logic:			At first, we initialize devices. Then we make the robot busy until pressing interrupt key.
 *					If interrupt key is pressed then initialize data  is stored .
 *					Then we convert the comma separated string input_str and store 
 *					Set current_velocity = 220 and synchronize wheel speed if needed and set synchronized velocity.
 *                  animal and habitat location is extracted from the data that was transfered from serial port
                    then it is stored in animal and habitat position. 
 *					Now, follow from the START position, follow black line for 80 mm to get the wheels on the horizontal black line.
 *					if the first animal is A1 we have hard coded how to pick then it is dropped as any other animal
 *                  the current direction is set facing north and current coordinate as (1,1) . then each animal and its corresponding habitat is visited 
 *				    till all habitat is visited . in loop the go_to_coordinate() of a particular animal is called and after bot reaching that postion pick() function 
 *				    is called to pick up the animal and then it moves to habitat by calling go_to_cell_no() which will go to habitat and arm drops the a animal when  dropping()
 *				    function is called . this is done till it reaches all animal are  placed in its corresponding habitat. after reaching last animal a continuous buzzer is played and 
 *				    servos are free-ed with help of servo1_free() and sevo2_free().
 *					
 * Example Call:	Called automatically by the Operating System
 */
int main() 
{
	init_devices();
	
	// make the robot busy until detecting boot switch i.e. interrupt is pressed
	while (1) 
	{
		if((PINE | 0x7F) == 0x7F) 
		{ // interrupt switch is pressed
			break;
		}
	}
	//print_input();
	//**********************************start***********************************
	//the bellow set of loops are for extracting the locations from the serial input 
	char c;
	int i=0,j=0,k=0,p=0;
	while(input_str[i]!='\n')
	{c=input_str[i];
		if(c==',')
		{
			i++;
			k++;
			p=0;
		}
		else
		{
			habitat_pos[k][p]=c;
			p++;
		}
		i++;
	}
	i++;
	p=0;
	while(j<=k)
	{
		c=input_str[i];
	
		if(c==','||c==' ')
		{
			i++;
			j++;
			p=0;
		}
		else
		{
			animal_pos[j][p]=c;
			p++;
		}
		i++;
	}
	//********************************end*************************************
	// set velocity
	current_velocity = 220; // 200 on full charge
		
	// synchronize wheels
	// suppose left wheel is physically 7.18% slower than the right wheel, so increase velocity of left wheel
	left_velocity_float = current_velocity + current_velocity * 0/100.0; // replace 0 with patch
	right_velocity_float = current_velocity + current_velocity * 0/100.0; // replace 0 with patch
	left_velocity = (unsigned char) left_velocity_float;
	right_velocity = (unsigned char) right_velocity_float;
	velocity(left_velocity, right_velocity);
		
	current_direction = 'N';
	current_coordinate[0] = 1;
	current_coordinate[1] = 1;
	int a[2];
	current_index=0;
	forward_mm(80);// this is to get the bots wheels on the horizontal line
	//******************************start***************************************
	//the bellow set of statements are hard coding for picking animal from A1 and droping it in its habitat if A1 is the first to be picked
	if(animal_pos[current_index][1]-48==1 && animal_pos[current_index][0]-64==1)
	{
		left_degrees(45);
		pick('L');
		right_degrees(45);
		if(atoi((char*)habitat_pos[current_index])!=1)
		go_to_cell_no (atoi((char*)habitat_pos[current_index]));
		droping();
		if(current_index!=j-1)
		{
			move_one_cell(1);
			if(current_direction=='N')
				current_coordinate[0]++;
			else if(current_direction=='S')
				current_coordinate[0]--;
			else if(current_direction=='W')
				current_coordinate[1]--;
			else
				current_coordinate[1]++;
		}	
		visited[atoi((char*)habitat_pos[current_index])-1]=1;
		current_index++;
	}
	//******************************end***************************************
	//*******************************start**************************************
	//the bellow while loop is main loop which traverse through the position arrays(habitat_pos & animal_pos) and pick and drop 
	//all the animals in their corresponding habitat using the above defined functions
	while(current_index<j){
		a[0]=animal_pos[current_index][1]-48;
		a[1]=animal_pos[current_index][0]-64;
		go_to_coordinate (a);
		
		pick_up();
	
		go_to_cell_no (atoi((char*)habitat_pos[current_index]));
		
		droping();
		if(current_index!=j-1)
		{
			move_one_cell(1);
			if(current_direction=='N')
				current_coordinate[0]++;
			else if(current_direction=='S')
				current_coordinate[0]--;
			else if(current_direction=='W')
				current_coordinate[1]--;
			else
				current_coordinate[1]++;
		}	
		visited[atoi((char*)habitat_pos[current_index])-1]=1;
		current_index++;
	}
	//********************************end*************************************
	buzzer_on();
	_delay_ms(5000);
	buzzer_off();
	servo_1_free();
	servo_2_free();
	
	
}
//Main Function end ########################################################################################
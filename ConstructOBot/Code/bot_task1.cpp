void forward_wls(unsigned char node)
{
	unsigned char left_sensor, center_sensor, right_sensor, nodeDetect = 0, flag = 0;
	printf("\nFORWARD BY %d NODE", node);
	while (flag==0){
		left_sensor = ADC_Conversion(1);
		center_sensor = ADC_Conversion(2);
		right_sensor = ADC_Conversion(3);

		forward();
		if (left_sensor != 255 and center_sensor == 255 and right_sensor != 255) {
			//forward();
			velocity(200, 200);
			//_delay_ms(50);
		}
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

		else if (left_sensor == 255 and center_sensor != 255 and right_sensor == 255) {  // -> WHITE LINE FOLLOWER for the H5 House
			printf("WHITE LINE FOLLOWER");
			while (1) {
				left_sensor = ADC_Conversion(1);
				center_sensor = ADC_Conversion(2);
				right_sensor = ADC_Conversion(3);
				if (left_sensor == 255 and center_sensor != 255 and right_sensor == 255) {
					velocity(150, 150);
					_delay_ms(50);
				}
				else if (left_sensor == 255 and center_sensor == 255 and right_sensor != 255) {
					//soft_right();
					velocity(150, 0);
					_delay_ms(50);
				}
				else if (left_sensor != 255 and center_sensor == 255 and right_sensor == 255) {
					//soft_left();
					velocity(0, 150);
					_delay_ms(50);
				}
				else if ((left_sensor != 255 and center_sensor != 255 and right_sensor != 255)) {
					nodeDetect++;

					if (nodeDetect == node) {
						forward();
						velocity(200, 200);
						_delay_ms(325);
						stop();
						flag = 1;
						break;
					}
					while ((left_sensor != 255 and center_sensor != 255 and right_sensor != 255)) {
						left_sensor = ADC_Conversion(1);
						center_sensor = ADC_Conversion(2);
						right_sensor = ADC_Conversion(3);
						forward();
					}
				}
				else if (left_sensor != 255 and center_sensor == 255 and right_sensor != 255)
					break;
			}
		}
		else if (left_sensor != 255 and center_sensor != 255 and right_sensor != 255) { // Proximity Sensor
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
		else if ((left_sensor == 255 and center_sensor == 255 and right_sensor == 255)) {
			nodeDetect++;

			if (nodeDetect == node) {
				forward();
				_delay_ms(260);
				stop();
				break;
			}
			while ((left_sensor == 255 and center_sensor == 255 and right_sensor == 255)) {
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
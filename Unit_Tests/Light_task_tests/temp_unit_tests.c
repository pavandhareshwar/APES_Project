/*************************************************************************************
*
* FileName        :    temp_unit_tests.c
* Description     :    This file contains necessary  test functions for temperature task.
					   
* File Author Name:    Sridhar Pavithrapu 
* Tools used      :    gcc, gedit, cmocka
* References      :    None
*
***************************************************************************************/

/* Headers Section */
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "light_sensor.h"

/* Macros section */


/**
​* ​ ​ @brief​ :  test function for checking gain of light sensor
​* ​ ​
​*
​* ​ ​ @param​ ​ state  ​ A pointer to the state
​*
​* ​ ​ @return​ ​None
​*/
void test_light_gain_check(void **state){

	/* Test case for checking gain of light sensor */	
	uint8_t cmd_tim_reg_val = read_timing_reg();
	uint8_t cmd_tim_field_val = 0XAF;

	write_timing_reg(cmd_tim_reg_val, 0x10, cmd_tim_field_val);
	
	uint8_t light_sen_tim_reg_val = read_timing_reg();
	assert_int_equal(light_sen_tim_reg_val, cmd_tim_field_val);	

}

/**
​* ​ ​ @brief​ :  test function for checking integration time of light sensor
​* ​ ​
​*
​* ​ ​ @param​ ​ state  ​ A pointer to the state
​*
​* ​ ​ @return​ ​None
​*/
void test_light_integration_time_check(void **state){

	/* Test case for checking integration time of light sensor */	
	uint8_t cmd_tim_reg_val = read_timing_reg();
	uint8_t cmd_tim_field_val = 0X1F;

	write_timing_reg(cmd_tim_reg_val, 0x3, cmd_tim_field_val);
	
	uint8_t light_sen_tim_reg_val = read_timing_reg();
	assert_int_equal(light_sen_tim_reg_val, cmd_tim_field_val);	
}

/**
​* ​ ​ @brief​ :  test function for checking low threshold of light sensor
​* ​ ​
​*
​* ​ ​ @param​ ​ state  ​ A pointer to the state
​*
​* ​ ​ @return​ ​None
​*/
void test_light_low_threshold_check(void **state){

	/* Test case for checking low threshold of light sensor */	
	uint16_t data = 0X9876;
	write_intr_low_thresh_reg(data);
	uint16_t *return_value;
	read_temp_high_low_register(return_value);
	assert_int_equal(return_value, data);	
}

/**
​* ​ ​ @brief​ : test function for checking high threshold of light sensor
​* ​ ​
​*
​* ​ ​ @param​ ​ state  ​ A pointer to the state
​*
​* ​ ​ @return​ ​None
​*/
void test_light_high_threshold_check(void **state){

	/* Test case for checking temperature high threshold of light sensor */	
	uint16_t data = 0X7676;
	write_intr_high_thresh_reg(data);
	uint16_t *return_value;
	read_temp_high_high_register(return_value);
	assert_int_equal(return_value, data);
	
}


/**
​* ​ ​ @brief​ : main function for all DLL test cases
​* ​ ​
​*
​* ​ ​ @return​ ​Pass and Fail test cases
​*/
int main(int argc, char **argv){

	/* Calling all DLL test case functions */
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_light_gain_check),
		cmocka_unit_test(test_light_integration_time_check),
		cmocka_unit_test(test_light_low_threshold_check),
		cmocka_unit_test(test_light_high_threshold_check),
		
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
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
#include "temperature_sensor.h"

/* Macros section */
#define TEST_NUM_ZERO (0)
#define TEST_NUM_ONE (1)
#define TEST_NUM_TWO (2)
#define TEST_NUM_TEN (10)
#define TEST_NUM_ELEVEN (11)
#define TEST_NUM_TWELVE (12)

/**
​* ​ ​ @brief​ : test function for writing and reading temperature 
*			  configuration register.
​* ​ ​
​*
​* ​ ​ @param​ ​ state  ​ A pointer to the state
​*
​* ​ ​ @return​ ​None
​*/
void test_temperature_config_register(void **state){

	/* Test case for checking temperature configuration register */	
	write_config_register_default()
	uint16_t return_value = read_temp_config_register();
	assert_int_equal(return_value, ((default_config_byte_one << 8) | default_config_byte_two));

}

/**
​* ​ ​ @brief​ : test function for checking temperature 
*			  high threshold value register.
​* ​ ​
​*
​* ​ ​ @param​ ​ state  ​ A pointer to the state
​*
​* ​ ​ @return​ ​None
​*/
void test_temperature_threshold_high(void **state){

	/* Test case for checking temperature high threshold value register */	
	uint16_t data = 0X9876
	write_temp_high_low_register(I2C_TEMP_SENSOR_THIGH_REG,data)
	uint16_t return_value = read_temp_high_low_register(I2C_TEMP_SENSOR_THIGH_REG);
	assert_int_equal(return_value, data);	
}

/**
​* ​ ​ @brief​ : test function for checking temperature 
*			  low threshold value register.
​* ​ ​
​*
​* ​ ​ @param​ ​ state  ​ A pointer to the state
​*
​* ​ ​ @return​ ​None
​*/
void test_temperature_threshold_low(void **state){

	/* Test case for checking temperature low threshold value register */	
	uint16_t data = 0X9876
	write_temp_high_low_register(I2C_TEMP_SENSOR_TLOW_REG,data)
	uint16_t return_value = read_temp_high_low_register(I2C_TEMP_SENSOR_TLOW_REG);
	assert_int_equal(return_value, data);	
}

/**
​* ​ ​ @brief​ : test function for checking em-bit of config register
​* ​ ​
​*
​* ​ ​ @param​ ​ state  ​ A pointer to the state
​*
​* ​ ​ @return​ ​None
​*/
void test_temperature_config_em(void **state){

	/* Test case for checking em-bit of config register */
	int data = 1;
	write_config_register_em(data)
	uint16_t return_value = read_config_register_em();
	assert_int_equal(return_value, data);
	
}

/**
​* ​ ​ @brief​ : test function for checking conversion rate of config register
​* ​ ​
​*
​* ​ ​ @param​ ​ state  ​ A pointer to the state
​*
​* ​ ​ @return​ ​None
​*/
void test_temperature_config_conversion_rate(void **state){

	/* Test case for checking conversion rate of config register */
	int data = 1;
	write_config_register_conversion_rate(data)
	uint16_t return_value = read_config_register_conversion_rate();
	assert_int_equal(return_value, data);
	
}

/**
​* ​ ​ @brief​ : test function for checking negative scenario conversion rate of config register
​* ​ ​
​*
​* ​ ​ @param​ ​ state  ​ A pointer to the state
​*
​* ​ ​ @return​ ​None
​*/
void test_temperature_config_conversion_rate_false(void **state){

	/* Test case for checking negative scenario conversion rate of config register */
	int data = 1;
	write_config_register_conversion_rate(data)
	uint16_t return_value = read_config_register_conversion_rate();
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
		cmocka_unit_test(test_temperature_config_register),
		cmocka_unit_test(test_temperature_threshold_high),
		cmocka_unit_test(test_temperature_threshold_low),
		cmocka_unit_test(test_temperature_config_em),
		cmocka_unit_test(test_temperature_config_conversion_rate),
		cmocka_unit_test(test_temperature_config_conversion_rate_false),
		
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
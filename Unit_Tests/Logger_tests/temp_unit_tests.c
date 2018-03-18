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
#include "logger_task.h"

/* Macros section */

/**
​* ​ ​ @brief​ :  test function for checking write to message queue
​* ​ ​
​*
​* ​ ​ @param​ ​ state  ​ A pointer to the state
​*
​* ​ ​ @return​ ​None
​*/
void test_write_message_queue(void **state){

	/* Test case for checking write to message queue */
	char buffer[MSG_MAX_LEN] = "This is a test case";
	int return_value = write_test_msg_to_logger(buffer);
	assert_int_equal(return_value, 0);	

}

/**
​* ​ ​ @brief​ :  test function for checking read to message queue
​* ​ ​
​*
​* ​ ​ @param​ ​ state  ​ A pointer to the state
​*
​* ​ ​ @return​ ​None
​*/
void test_read_message_queue(void **state){

	/* Test case for checking read to message queue */	
	char buffer[MSG_MAX_LEN] = "This is a test case";
	char buffer_output[MSG_MAX_LEN];
	int return_value = write_test_msg_to_logger(buffer);
	if(return_value == 0){
		read_test_msg_to_logger(buffer_output);
		assert_string_equal(buffer,buffer_output);
	}
	else{
		assert_int_equal(return_value, 0);	
	}
}

/**
​* ​ ​ @brief​ : main function for all DLL test cases
​* ​ ​
​*
​* ​ ​ @return​ ​Pass and Fail test cases
​*/
int main(int argc, char **argv){
	
	logger_task_init();

	/* Calling all DLL test case functions */
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_write_message_queue),
		cmocka_unit_test(test_read_message_queue),
		
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
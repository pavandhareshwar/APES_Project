/*************************************************************************
* Author:       Pavan Dhareshwar & Sridhar Pavithrapu
* Date:         03/07/2018
* File:         temperature_sensor.h
* Description:  Header file containing the macros, structs/enums, globals
                and function prototypes for source file temperature_sensor.c
*************************************************************************/

#ifndef _TEMPERATURE_SENSOR_TASK_H_
#define _TEMPERATURE_SENSOR_TASK_H_


/*---------------------------------- INCLUDES -------------------------------*/

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/socket.h>

#include <mqueue.h>

/*---------------------------------- GLOBALS --------------------------------*/
int temp_fd;
char i2c_name[10];
int sensor_thread_id, socket_thread_id;


/*----------------------------------- MACROS --------------------------------*/

#define I2C_SLAVE_ADDR				    0b01001000
#define I2C_SLAVE_DEV_NAME              "/dev/i2c-2"


#define I2C_TEMP_SENSOR_TEMP_DATA_REG	0b00000000	// Temperature data register (read-only)
#define I2C_TEMP_SENSOR_CONFIG_REG	    0b00000001	// command register
#define I2C_TEMP_SENSOR_TLOW_REG	    0b00000010	// T_low register
#define I2C_TEMP_SENSOR_THIGH_REG	    0b00000011	// T_high register

#define SERVER_PORT_NUM                 8081
#define SERVER_LISTEN_QUEUE_SIZE        5

#define MSG_BUFF_MAX_LEN                1024
#define MSG_MAX_LEN                     128

#define MSG_QUEUE_NAME                  "/logger_task_mq"
#define MSG_QUEUE_MAX_NUM_MSGS          5
#define MSG_QUEUE_MAX_MSG_SIZE          1024

#define SOCK_REQ_MSG_API_MSG_LEN        64

/*---------------------------- STRUCTURES/ENUMERATIONS ----------------------*/

typedef enum{
	
	TEMP_CELSIUS = 0,
	TEMP_KELVIN = 1,
	TEMP_FARENHEIT = 2
	
}tempformat_e;

enum _msg_type_                                                                                       
{                                                                                                     
    MSG_TYPE_TEMP_DATA,                                                                               
    MSG_TYPE_LUX_DATA                                                                                 
};                                                                                                    
                                                                                                      
struct _logger_msg_struct_                                                                            
{   
    char message[MSG_MAX_LEN];
    int msg_len;
    enum _msg_type_ logger_msg_type;                                                                  
};

enum _req_recipient_
{   
    REQ_RECP_TEMP_TASK, 
    REQ_RECP_LIGHT_TASK
};  
    
struct _socket_req_msg_struct_
{
    char req_api_msg[SOCK_REQ_MSG_API_MSG_LEN];
    enum _req_recipient_ req_recipient;
    void *ptr_param_list;
};

/*---------------------------- FUNCTION PROTOTYPES --------------------------*/
/**
 *  @brief Write pointer register of temperature sensor
 *  
 *  This function will open the i2c bus write operation of pointer register
 *  of Temperature sensor.
 *
 *  @param value	: value to be written into pointer register
 *
 *  @return void
*/
void write_pointer_register(int file_descriptor, uint8_t value);

/**
 *  @brief Write temperature high and low register of temperature sensor
 *  
 *  This function will open the i2c bus write operation of temperature high and
 *  low register of Temperature sensor.
 *
 *  @param sensor_register	: register address of either temperature high or low register
 *		   data			  	: value to be written into register
 *
 *  @return void
*/
void write_temp_high_low_register(int file_descriptor, int sensor_register, uint16_t data );

/**
 *  @brief Read temperature high and low register of temperature sensor
 *  
 *  This function will open the i2c bus for read of temperature high and
 *  low register of Temperature sensor.
 *
 *  @param sensor_register	: register address of either temperature high or low register
 *		   data			  	: value to be read from register
 *
 *  @return reg_val   : if register read is successful
 *          -1        : if register read fails
*/
uint16_t read_temp_high_low_register(int file_descriptor, int sensor_register);

/**
 *  @brief Read temperature config of temperature sensor
 *  
 *  This function will open the i2c bus for read config
 *  register of Temperature sensor.
 *
 *  @param void
 *
 *  @return reg_val   : if register read is successful
 *          -1        : if register read fails
*/
uint16_t read_temp_config_register(int file_descriptor);

/**
 *  @brief Read temperature data of temperature sensor
 *  
 *  This function will open the i2c bus for read temperature data
 *  register of Temperature sensor.
 *
 *  @param void
 *
 *  @return temp_value  : if register read is successful
            -1 : if sensor initialization fails
*/
float read_temperature_data_register(int file_descriptor,int format);

/**
 *  @brief Log the temperature value
 *  
 *  This function writes the temperature value calculated to logger message queue
 *
 *  @param temp_data     : temperature data to be logged
 *
 *  @return void
*/
void log_temp_data(float temp_data);

#endif // #ifndef _TEMPERATURE_SENSOR_TASK_H_

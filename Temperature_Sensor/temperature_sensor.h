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
char i2c_name[10];
int sensor_thread_id, socket_thread_id, socket_hb_thread_id;
int file_descriptor;
int default_config_byte_one = 0X50;
int default_config_byte_two = 0XA0;


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

#define SOCKET_HB_PORT_NUM              8650
#define SOCKET_HB_LISTEN_QUEUE_SIZE     5

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
void write_pointer_register(uint8_t value);

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
void write_temp_high_low_register(int sensor_register, uint16_t data );

/**
 *  @brief Write config register of temperature sensor
 *  
 *  This function will open the i2c bus write operation of config register of Temperature sensor.
 *
 *  @param data			  	: value to be written into register
 *
 *  @return void
*/
void write_config_register_on_off(uint8_t data );

/**
 *  @brief Write config register of temperature sensor
 *  
 *  This function will open the i2c bus write operation of config register for em bits of Temperature sensor.
 *
 *  @param data			  	: value to be written for em bits of config register
 *
 *  @return void
*/
void write_config_register_em(uint8_t data );

/**
 *  @brief Write config register of temperature sensor
 *  
 *  This function will open the i2c bus write operation of config register for conversion rate of Temperature sensor.
 *
 *  @param data			  	: value to be written for conversion rate of config register
 *
 *  @return void
*/
void write_config_register_conversion_rate(uint8_t data );

/**
 *  @brief Write config register of temperature sensor
 *  
 *  This function will open the i2c bus write operation of default values into config register of Temperature sensor.
 *
 *  @param data			  	: void
 *
 *  @return void
*/
void write_config_register_default( );

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
uint16_t read_temp_high_low_register(int sensor_register);

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
uint16_t read_temp_config_register();

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
float read_temperature_data_register(int format);


/**
 *  @brief Initialize the temperature sensor
 *  
 *  This function will open the i2c bus for read and write operation and 
 *  initialize the communication with the peripheral.
 *
 *  @param void
 *
 *  @return 0  : if sensor initialization is a success
            -1 : if sensor initialization fails
*/
int temp_sensor_init();

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




/**
 *  @brief Entry point and executing entity for sensor thread
 *  
 *  The sensor thread starts execution by invoking this function(start_routine)
 *
 *  @param arg : argument to start_routine
 *
 *  @return void
 *
 */
void *sensor_thread_func(void *arg);

/**
 *  @brief Entry point and executing entity for socket thread
 *  
 *  The socket thread starts execution by invoking this function(start_routine)
 *
 *  @param arg : argument to start_routine
 *
 *  @return void
 *
 */
void *socket_thread_func(void *arg);

/**
 *  @brief Entry point and executing entity for socket thread
 *  
 *  The socket thread for heartbeat starts execution by invoking this function(start_routine)
 *
 *  @param arg : argument to start_routine
 *
 *  @return void
 *
 */
void *socket_hb_thread_func(void *arg)

/**
 *  @brief Create sensor,socket and heartbeat threads for temperature task
 *  
 *  The temperature task is made multi-threaded with 
 *     1. sensor thread responsible for communicating via I2C interface 
 *        with the temperature sensor to get temperature data and a socket 
 *        thread.
 *     2. socket thread responsible for communicating with socket thread and
 *        serve request from external application forwarded via socket task.
 *	   3. socket heartbeat responsible for communicating with main task,
 *		  to log heartbeat every time its requested by main task.
 *
 *  @param void
 *
 *  @return 0  : thread creation success
 *          -1 : thread creation failed
 *
 */
int create_threads(void);

/**
 *  @brief Create the socket and initialize
 *  
 *  This function create the socket for the given socket id.
 *
 *  @param sock_fd     			: socket file descriptor
 *		   server_addr_struct	: server address of the socket
 *		   port_num  			: port number in which the socket is communicating
 *		   listen_qsize 		: number of connections the socket is accepting
 *
 *  @return void
*/
void init_sock(int *sock_fd, struct sockaddr_in *server_addr_struct, 
               int port_num, int listen_qsize);

#endif // #ifndef _TEMPERATURE_SENSOR_TASK_H_

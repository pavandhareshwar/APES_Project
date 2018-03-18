/*************************************************************************
* Author:       Pavan Dhareshwar & Sridhar Pavithrapu
* Date:         03/07/2018
* File:         light_sensor.h
* Description:  Header file containing the macros, structs/enums, globals
                and function prototypes for source file light_sensor.c
*************************************************************************/

#ifndef _LIGHT_SENSOR_TASK_H_
#define _LIGHT_SENSOR_TASK_H_

/*---------------------------------- INCLUDES -------------------------------*/
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include <mqueue.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "wrapper.h"

/*---------------------------------- INCLUDES -------------------------------*/

/*----------------------------------- MACROS --------------------------------*/

#define I2C_SLAVE_ADDR		         0b0111001       // Slave address - 0x39
#define I2C_DEV_NAME                 "/dev/i2c-2"

#define I2C_LIGHT_SENSOR_CMD_CTRL_REG                0x80
#define I2C_LIGHT_SENSOR_CMD_TIM_REG                 0x81

#define I2C_LIGHT_SENSOR_CMD_THRESH_LOW_LOW_REG      0x82
#define I2C_LIGHT_SENSOR_CMD_THRESH_LOW_HIGH_REG     0x83
#define I2C_LIGHT_SENSOR_CMD_THRESH_HIGH_LOW_REG     0x84
#define I2C_LIGHT_SENSOR_CMD_THRESH_HIGH_HIGH_REG    0x85

#define I2C_LIGHT_SENSOR_CMD_INT_REG                 0x86
#define I2C_LIGHT_SENSOR_CMD_ID_REG                  0x8A

#define I2C_LIGHT_SENSOR_CMD_DATA0LOW_REG            0x8C
#define I2C_LIGHT_SENSOR_CMD_DATA0HIGH_REG           0x8D
#define I2C_LIGHT_SENSOR_CMD_DATA1LOW_REG            0x8E
#define I2C_LIGHT_SENSOR_CMD_DATA1HIGH_REG           0x8F

#define I2C_LIGHT_SENSOR_CTRL_REG_VAL                0x3

#define MSG_QUEUE_NAME                               "/logger_task_mq"
#define MSG_QUEUE_MAX_NUM_MSGS                       5
#define MSG_QUEUE_MAX_MSG_SIZE                       1024

#define MSG_MAX_LEN                                  128

#define LIGHT_SENSOR_SERVER_PORT_NUM                 8086
#define LIGHT_SENSOR_LISTEN_QUEUE_SIZE               5

#define MSG_BUFF_MAX_LEN                             1024

#define SOCK_REQ_MSG_API_MSG_LEN                     64

#define SOCKET_HB_PORT_NUM                           8660
#define SOCKET_HB_LISTEN_QUEUE_SIZE                  5

#define MSG_TYPE_TEMP_DATA                           0
#define MSG_TYPE_LUX_DATA                            1
#define MSG_TYPE_SOCK_DATA                           2
#define MSG_TYPE_MAIN_DATA                           3

#define LOGGER_ATTR_LEN                              32

/*----------------------------------- MACROS --------------------------------*/

/*---------------------------------- GLOBALS --------------------------------*/
int i2c_light_sensor_fd;
int server_fd, accept_conn_id;
int sensor_thread_id, socket_thread_id, socket_hb_thread_id;

mqd_t logger_mq_handle;

sig_atomic_t g_sig_kill_sensor_thread, g_sig_kill_sock_thread, g_sig_kill_sock_hb_thread;
/*---------------------------------- GLOBALS --------------------------------*/

/*---------------------------- STRUCTURES/ENUMERATIONS ----------------------*/

struct _logger_msg_struct_
{
    char message[MSG_MAX_LEN];
    char logger_msg_src_id[LOGGER_ATTR_LEN];
    char logger_msg_level[LOGGER_ATTR_LEN];
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

#if 0
struct _int_thresh_reg_struct_
{
    uint8_t thresh_low_low;
    uint8_t thresh_low_high;
    uint8_t thresh_high_low;
    uint8_t thresh_high_high;
};
#endif
struct _int_thresh_reg_struct_
{
    uint16_t low_thresh;
    uint16_t high_thresh;
};

struct _light_sensor_tim_params
{
    uint8_t tim_reg_val;
    uint8_t tim_reg_field_to_set;
    uint8_t tim_reg_field_val;

};
/*---------------------------- STRUCTURES/ENUMERATIONS ----------------------*/

/*---------------------------- FUNCTION PROTOTYPES --------------------------*/
/**
 *  @brief Initialize the light sensor
 *  
 *  This function will open the i2c bus for read and write operation and 
 *  initialize the communication with the peripheral.
 *
 *  @param void
 *
 *  @return 0  : if sensor initialization is a success
            -1 : if sensor initialization fails
*/
int light_sensor_init();

/**
 *  @brief Power on the light sensor
 *  
 *  This function will configure the control register to power on the light 
 *  sensor.
 *
 *  @param void
 *
 *  @return void
 *
 */
void power_on_light_sensor(void);

/**
 *  @brief Create sensor,socket and hearbeat socket threads for light task
 *  
 *  The light task is made multi-threaded with 
 *     1. sensor thread responsible for communicating via I2C interface 
 *        with the light sensor to get light data and a socket 
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
 *  @brief Initialize light task socket
 *  
 *  This function will create, bind and make the socket listen for incoming
 *  connections.
 *
 *  @param sock_addr_struct : pointer to sockaddr_in structure
 *
 *  @return void
 *
 */
void init_light_socket(struct sockaddr_in *sock_addr_struct);

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
void *socket_hb_thread_func(void *arg);

/**
 *  @brief Get lux data from light sensor
 *  
 *  This function will get the illuminance (ambient light level) in lux and
 *  return this value.
 *
 *  @param void
 *
 *  @return float lux data
*/
float get_lux_data();

/**
 *  @brief Write light sensor register
 *  
 *  This function will write to light sensor data specifed by @param(
 *  cmd_reg_val) with a value specified by @param(target_reg_val)
 *
 *  @param cmd_reg_val     : command register value
 *  @param target_reg_val  : value to be written to target register
 *
 *  @return 0   : if register write is successful
 *          -1  : if register write fails 
*/
int write_light_sensor_reg(int cmd_reg_val, int target_reg_val);

/**
 *  @brief Read light sensor register
 *  
 *  This function will read light sensor data specifed by @param(
 *  read_reg_val)
 *
 *  @param read_reg_val     : register to be read
 *
 *  @return reg_val   : if register read is successful
 *          -1        : if register read fails 
*/
int8_t read_light_sensor_reg(uint8_t read_reg_val);

/**
 *  @brief Get the ADC channel data
 *  
 *  This function will read the ADC data for channel specified by @param(
 *  channel_num) and populate them @param(ch_data_low) and @param(ch_data_high)
 *
 *  @param channel_num     : ADC channel number to be read
 *  @param ch_data         : pointer to ADC data
 *
 *  @return void 
*/
void get_adc_channel_data(int channel_num, uint16_t *ch_data);

/**
 *  @brief Calculate the lux value
 *  
 *  This function calculates the illuminance value
 *
 *  @param ch0_data     : ADC channel 0 data
 *  @param ch1_data     : ADC channel 1 data
 *
 *  @return lux_val 
*/
float calculate_lux_value(uint16_t ch0_data, uint16_t ch1_data);

/**
 *  @brief Log the lux value
 *  
 *  This function writes the lux value calculated to logger message queue
 *
 *  @param lux_data     : lux_data
 *
 *  @return void
*/
void log_lux_data(float lux_data);

/**
 *  @brief Cleanup of the light sensor
 *  
 *  This function will close the i2c bus for read and write operation and 
 *  perform any cleanup required
 *
 *  @param void
 *
 *  @return void 
*/
void light_sensor_exit(void);

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

/**
 *  @brief Signal handler for temperature task
 *  
 *  This function handles the reception of SIGKILL and SIGINT signal to the 
 *  temperature task and terminates all the threads, closes the I2C file descriptor
 *  and logger message queue handle and exits.
 *
 *  @param sig_num              : signal number
 *
 *  @return void
*/

void sig_handler(int sig_num);

/**
 *  @brief Write command register of light sensor
 *
 *  This function will write to command register of light sensor
 *
 *  @param cmd_reg_val    : value to be written
 *
 *  @return 0   : success
 *          -1  : failure
 */
void write_cmd_reg(uint8_t cmd_reg_val);

/**
 *  @brief Read control register of light sensor
 *
 *  This function will read the control register of light sensor
 *
 *  @param void
 *
 *  @return ctrl_reg_val 
*/
uint8_t read_ctrl_reg(void);

/**
 *  @brief Write control register of light sensor
 *
 *  This function will write to control register of light sensor
 *
 *  @param ctrl_reg_val    : value to be written
 *
 *  @return 0   : success
 *          -1  : failure
 */
int write_ctrl_reg(uint8_t ctrl_reg_val);

/**
 *  @brief Read timing register of light sensor
 *
 *  This function will read the timing register of light sensor
 *
 *  @param void
 *
 *  @return tim_reg_val 
*/
uint8_t read_timing_reg(void);

/**
 *  @brief Write timing register of light sensor
 *
 *  This function will write to timing register of light sensor
 *
 *  @param tim_reg_val     : value to be written
 *  @param field_to_set    : timing register field to be set
 *  @param field_val       : field value
 *
 *  @return 0   : success
 *          -1  : failure
 */
int write_timing_reg(uint8_t tim_reg_val, uint8_t field_to_set, uint8_t field_val);

/**
 *  @brief Enable or disable interrupt register of light sensor
 *
 *  This function will enable or diable the interrupt control register of 
 *  light sensor
 *
 *  @param int_ctrl_reg_val     : value to be written
 *
 *  @return 0   : success
 *          -1  : failure
*/
int enable_disable_intr_ctrl_reg(uint8_t int_ctrl_reg_val);

/**
 *  @brief Read identification register of light sensor
 *
 *  This function will read the identification register of light sensor
 *
 *  @param void
 *
 *  @return tim_reg_val 
*/
uint8_t read_id_reg(void);

/**
 *  @brief Read interrupt threshold register of light sensor
 *
 *  This function will read the interrupt threshold register of light sensor
 *
 *  @param low_thresh            : pointer to low threshold value
 *  @param high_thresh           : pointer to high threshold value
 *
 *  @return void 
*/
void read_intr_thresh_reg(uint16_t *low_thresh, uint16_t *high_thresh);

/**
 *  @brief Write interrupt threshold register of light sensor
 *
 *  This function will write the interrupt threshold register of light sensor
 *
 *  @param low_thresh            : low threshold value to be written
 *  @param high_thresh           : high threshold value to be written
 *
 *  @return void 
*/
void write_intr_thresh_reg(uint16_t low_thresh, uint16_t high_thresh);

void write_intr_high_thresh_reg(uint16_t high_thresh);
void write_intr_low_thresh_reg(uint16_t low_thresh);
/*---------------------------- FUNCTION PROTOTYPES --------------------------*/

#endif

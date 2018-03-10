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

#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include <mqueue.h>

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

/*----------------------------------- MACROS --------------------------------*/

/*---------------------------------- GLOBALS --------------------------------*/
int i2c_light_sensor_fd;

/*---------------------------------- GLOBALS --------------------------------*/

/*---------------------------- STRUCTURES/ENUMERATIONS ----------------------*/
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
void get_adc_channel_data(int channel_num, int *ch_data);

/**
 *  @brief Calculate the lux value
 *  
 *  This function calculates the illuminance value
 *
 *  @param ch0_data     : ADC channel 0 data
 *  @param ch1_data     : ADC channel 1 data
 *
 *  @return void 
*/
float calculate_lux_value(int ch0_data, int ch1_data);

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
/*---------------------------- FUNCTION PROTOTYPES --------------------------*/

#endif

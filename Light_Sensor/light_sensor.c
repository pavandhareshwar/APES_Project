/*************************************************************************
* Author:       Pavan Dhareshwar & Sridhar Pavithrapu
* Date:         03/07/2018
* File:         light_sensor.c
* Description:  Source file describing the functionality and implementation
*               of light sensor task.
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "light_sensor.h"

int main(){

    int init_ret_val = light_sensor_init();
    if (init_ret_val == -1)
    {
        printf("Light sensor init failed\n");
        exit(1);
    }

    get_lux_data();

	light_sensor_exit();

	return 0;
}

int light_sensor_init(void)
{
	/* Open the i2c bus for read and write operation */
    printf("Opening i2c bus %s\n", I2C_DEV_NAME);
    if ((i2c_light_sensor_fd = open(I2C_DEV_NAME,O_RDWR)) < 0) {
		perror("Failed to open i2c bus.");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		return -1;
	}
	
	if (ioctl(i2c_light_sensor_fd,I2C_SLAVE,I2C_SLAVE_ADDR) < 0) {
		perror("Failed to acquire bus access and/or talk to slave.");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		return -1;
	}

    return 0;
}

int write_light_sensor_reg(int cmd_reg_val, int target_reg_val)
{

    /* Write the command register to specify the following two information 
    **       1. Target register address for subsequent write operation
    **       2. If I2C write operation is a word or byte operation 
    */
	int num_bytes_written = write(i2c_light_sensor_fd, &cmd_reg_val, 1);
	if (num_bytes_written != 1)
	{
		 perror("Failed to write to the i2c bus.");
         return -1;
    }
	
    if(write(i2c_light_sensor_fd, &target_reg_val, 1) != 1){
		 perror("Failed to write to the i2c bus.");	
         return -1;
    }

    return 0;
}

int8_t read_light_sensor_reg(uint8_t read_reg_val)
{
    /* Write the read register to specify the initiate a read operation */
	if(write(i2c_light_sensor_fd, &read_reg_val, 1) != 1){
		printf("Failed to write to the i2c bus.\n");
        return -1; 
    }

    /* Read the value */
    uint8_t read_val;
	if (read(i2c_light_sensor_fd, &read_val, 1) != 1) {
		perror("adc0 low data read error");
        return -1;
	}

    return read_val;
}

int get_lux_data(void)
{
	int cmd_ctrl_reg_val = I2C_LIGHT_SENSOR_CMD_CTRL_REG;
	
	int ctrl_reg_val = I2C_LIGHT_SENSOR_CTRL_REG_VAL;
	
    write_light_sensor_reg(cmd_ctrl_reg_val, ctrl_reg_val);

	uint8_t cmd_data0_low_reg = I2C_LIGHT_SENSOR_CMD_DATA0LOW_REG;
    int8_t cmd_data0_low_reg_val = read_light_sensor_reg(cmd_data0_low_reg);
    printf("data0_low : %d\n",cmd_data0_low_reg_val);

	uint8_t cmd_data0_high_reg = I2C_LIGHT_SENSOR_CMD_DATA0HIGH_REG;
    int8_t cmd_data0_high_reg_val = read_light_sensor_reg(cmd_data0_high_reg);
    printf("data0_high : %d\n",cmd_data0_high_reg_val);
	
	uint8_t cmd_data1_low_reg = I2C_LIGHT_SENSOR_CMD_DATA1LOW_REG;
    int8_t cmd_data1_low_reg_val = read_light_sensor_reg(cmd_data1_low_reg);
    printf("data1_low : %d\n",cmd_data1_low_reg_val);
	
	uint8_t cmd_data1_high_reg = I2C_LIGHT_SENSOR_CMD_DATA1HIGH_REG;
    int8_t cmd_data1_high_reg_val = read_light_sensor_reg(cmd_data1_high_reg);
    printf("data1_low : %d\n",cmd_data1_high_reg_val);
}

void light_sensor_exit(void)
{
    /* Close i2c bus */
    if (i2c_light_sensor_fd != -1)
        close(i2c_light_sensor_fd);
}

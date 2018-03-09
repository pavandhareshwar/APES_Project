#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define I2C_SLAVE_ADDR				    0b01001000
#define I2C_TEMP_SENSOR_TEMP_DATA_REG	0b00000000	// Temperature data register (read-only)
#define I2C_TEMP_SENSOR_CONFIG_REG	    0b00000001	// command register
#define I2C_TEMP_SENSOR_TLOW_REG	    0b00000010	// T_low register
#define I2C_TEMP_SENSOR_THIGH_REG	    0b00000011	// T_high register

typedef enum{
	
	TEMP_CELSIUS = 0,
	TEMP_KELVIN = 1,
	TEMP_FARENHEIT = 2
	
}tempformat_e;


void write_pointer_register(int file_descriptor, uint8_t value){

	printf("In write_pointer_register with value:%d\n",value);
	
	if (write(file_descriptor, &value, 1) != 1) {
		perror("Write pointer register error\n");
	}
}

void write_temp_high_low_register(int file_descriptor, int sensor_register, uint16_t data ){
	
	printf("In write_temp_low_register function \n");
	
	/* Writing to the pointer register for reading Tlow register */
	write_pointer_register(file_descriptor, sensor_register);
	
	/* Reading the Tlow register value */
	if (write(file_descriptor, &data, 2) != 2) {
		perror("T-low register write error");
	}
}

uint16_t read_temp_high_low_register(int sensor_register, int file_descriptor){
	
	printf("In read_temp_low_register function \n");
	
	uint16_t tlow_output_value;
	uint8_t data[1]={0};
	
	/* Writing to the pointer register for reading Tlow register */
	write_pointer_register(file_descriptor, sensor_register);
	
	/* Reading the Tlow register value */
	if (read(file_descriptor, data, 1) != 1) {
		perror("T-low register read error");
	}
	
	tlow_output_value = (data[0]<<4 | (data[1] >> 4 & 0XF));
	printf("T-low register value is: %f \n", tlow_output_value);
	return tlow_output_value;
	
}


uint16_t read_temp_config_register(int file_descriptor){
	
	printf("In read_temp_config_register function \n");
	
	uint16_t temp_config_value;
	uint8_t data[1]={0};
	
	/* Writing to the pointer register for reading THigh register */
	write_pointer_register(file_descriptor, I2C_TEMP_SENSOR_CONFIG_REG);
	
	/* Reading the THigh register value */
	if (read(file_descriptor, data, 1) != 1) {
		perror("Temperature configuration register read error");
	}
	
	temp_config_value = (data[0]<<8 | data[1]);
	printf("Temperature configuration register value is: %f \n", temp_config_value);
	return temp_config_value;
	
}

float read_temperature_data_register(int file_descriptor,int format){
	
	printf("In read_temperature_data_register function \n");
	
	float temperature_value;
	uint8_t data[1]={0};
	
	/* Writing to the pointer register for reading temperature data register */
	write_pointer_register(file_descriptor, I2C_TEMP_SENSOR_TEMP_DATA_REG);
	
	/* Reading the temperature data register value */
	if (read(file_descriptor, data, 1) != 1) {
		perror("Temperature data register read error");
	}
	
	if(format == TEMP_CELSIUS){
		temperature_value = (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.625;
		printf("Temperature value is: %f degree Celsius \n", temperature_value);
	}
	else if(format == TEMP_KELVIN){
		temperature_value = (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.625;
		temerature_value += 273.15;
		printf("Temperature value is: %f degree Kelvin \n", temperature_value);
	}
	else if(format == TEMP_FARENHEIT){
		temperature_value = (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.625;
		temerature_value = ((temerature_value * 9)/5 + 32);
		printf("Temperature value is: %f degree Fahrenheit \n", temperature_value);
		
	}
	else{
		printf("Invalid format\n");
	}
	return temperature_value;
	
}


int main(){

	char i2c_name[10];
	sprintf(i2c_name, "/dev/i2c-2");
	int temp_fd;
	
	if ((temp_fd = open(i2c_name,O_RDWR)) < 0) {
	printf("Failed to open the bus.");
	/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}
	else{
	
		printf("I2C open successful\n");
	}
	
	if (ioctl(temp_fd,I2C_SLAVE,I2C_SLAVE_ADDR) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}
	
	int config_byte[3];
	
	config_byte[0] = I2C_TEMP_SENSOR_CONFIG_REG;
	config_byte[1] = 0X50;
	config_byte[2] = 0XA0;
	
	if(write(temp_fd, config_byte, 3) != 3){
		 printf("Failed to write to the i2c bus.\n");	
	
	}
	
	uint8_t data[3];
	data[0] = I2C_TEMP_SENSOR_TEMP_REG;
	if (write(temp_fd, data, 1) != 1) {
		perror("temperature data register write error");
	}
	if (read(temp_fd, data, 2) != 2) {
		perror("temperature data register read error");
	}
	printf("data[0]:%d,data[1]:%d,data[2]:%d \n",data[0],data[1],data[2]);
	printf("temp value :%3.2f celsius\n", (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.0625);
	
	memset(data,0,sizeof(data));
	data[0] = I2C_TEMP_SENSOR_TLOW_REG;
	if (write(temp_fd, data, 1) != 1) {
		perror("temperature low register write error");
	}
	if (read(temp_fd, data, 1) != 1) {
		perror("temperature low register read error");
	}
	printf("data[0]:%d,data[1]:%d,data[2]:%d \n",data[0],data[1],data[2]);
	
	memset(data,0,sizeof(data));
	data[0] = I2C_TEMP_SENSOR_THIGH_REG;
	if (write(temp_fd, data, 1) != 1) {
		perror("temperature high register write error");
	}
	if (read(temp_fd, data, 1) != 1) {
		perror("temperature high register read error");
	}
	printf("data[0]:%d,data[1]:%d,data[2]:%d \n",data[0],data[1],data[2]);

	close(temp_fd);
		
	return 0;
}
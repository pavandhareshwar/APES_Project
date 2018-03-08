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

#define I2C_SLAVE_ADDR				0b01001000
#define I2C_TEMP_SENSOR_TEMP_REG	0b00000000	// Temperature data register (read-only)
#define I2C_TEMP_SENSOR_CONFIG_REG	0b00000001	// command register
#define I2C_TEMP_SENSOR_TLOW_REG	0b00000010	// T_low register
#define I2C_TEMP_SENSOR_THIGH_REG	0b00000011	// T_high register

int main(){

	char i2c_name[10];
	sprintf(i2c_name, "/dev/i2c-2");
	int file;
	
	if ((file = open(i2c_name,O_RDWR)) < 0) {
	printf("Failed to open the bus.");
	/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}
	else{
	
		printf("I2C open successful\n");
	}
	
	if (ioctl(file,I2C_SLAVE,I2C_SLAVE_ADDR) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}
	
	int config_byte[3];
	
	config_byte[0] = I2C_TEMP_SENSOR_CONFIG_REG;
	config_byte[1] = 0X50;
	config_byte[2] = 0XA0;
	
	if(write(file, config_byte, 3) != 3){
		 printf("Failed to write to the i2c bus.\n");	
	
	}
	
	uint8_t data[3];
	data[0] = I2C_TEMP_SENSOR_TEMP_REG;
	if (write(file, data, 1) != 1) {
		perror("temperature data register write error");
	}
	if (read(file, data, 2) != 2) {
		perror("temperature data register read error");
	}
	printf("data[0]:%d,data[1]:%d,data[2]:%d \n",data[0],data[1],data[2]);
	printf("temp value :%3.2f celsius\n", (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.0625);
	
	memset(data,0,sizeof(data));
	data[0] = I2C_TEMP_SENSOR_TLOW_REG;
	if (write(file, data, 1) != 1) {
		perror("temperature low register write error");
	}
	if (read(file, data, 1) != 1) {
		perror("temperature low register read error");
	}
	printf("data[0]:%d,data[1]:%d,data[2]:%d \n",data[0],data[1],data[2]);
	
	memset(data,0,sizeof(data));
	data[0] = I2C_TEMP_SENSOR_THIGH_REG;
	if (write(file, data, 1) != 1) {
		perror("temperature high register write error");
	}
	if (read(file, data, 1) != 1) {
		perror("temperature high register read error");
	}
	printf("data[0]:%d,data[1]:%d,data[2]:%d \n",data[0],data[1],data[2]);

	close(file);
		
	return 0;
}
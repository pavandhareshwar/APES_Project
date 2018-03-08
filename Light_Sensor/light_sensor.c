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

#define I2C_SLAVE_ADDR				0b0111001


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
	
	//int addr = (I2C_SLAVE_ADDR << 1) | 0;
	if (ioctl(file,I2C_SLAVE,I2C_SLAVE_ADDR) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}
	else
	{
		printf("ioctl success\n");
	}
	
	int config_byte;
	
	config_byte = 0X80;
	//config_byte[1] = 0X03;
	
	int num_bytes_written = write(file, &config_byte, 1);
	printf("num_bytes_written: %d\n", num_bytes_written);
	if (num_bytes_written != 1)
	{
		 perror("Failed to write to the i2c bus.\n");
	}
	
	config_byte = 0X3;
	//config_byte[1] = 0X03;
	
	if(write(file, &config_byte, 1) != 1){
		 printf("Failed to write to the i2c bus.\n");	
	}

	uint8_t data[2];
	memset(data,0,sizeof(data));
	data[0] = 0X8C;
	
	if(write(file, data, 1) != 1){
		 printf("Failed to write to the i2c bus.\n");	
	
	}
	if (read(file, data, 1) != 1) {
		perror("temperature data register read error");
	}
	printf("data0low : %d,\n",data[0]);
	
	memset(data,0,sizeof(data));
	data[0] = 0X8D;
	
	if(write(file, data, 1) != 1){
		 printf("Failed to write to the i2c bus.\n");	
	
	}
	if (read(file, data, 1) != 1) {
		perror("temperature data register read error");
	}
	printf("data0high : %d,\n",data[0]);
	
	memset(data,0,sizeof(data));
	data[0] = 0X8E;
	
	if(write(file, data, 1) != 1){
		 printf("Failed to write to the i2c bus.\n");	
	
	}
	if (read(file, data, 1) != 1) {
		perror("temperature data register read error");
	}
	printf("data1low : %d,\n",data[0]);
	
	memset(data,0,sizeof(data));
	data[0] = 0X8F;
	
	if(write(file, data, 1) != 1){
		 printf("Failed to write to the i2c bus.\n");	
	
	}
	if (read(file, data, 1) != 1) {
		perror("temperature data register read error");
	}
	printf("data1high : %d,\n",data[0]);

	close(file);
		
	return 0;
}
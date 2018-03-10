/*************************************************************************
* Author:       Pavan Dhareshwar & Sridhar Pavithrapu
* Date:         03/07/2018
* File:         temperature_sensor.c
* Description:  Source file describing the functionality and implementation
*               of temperature sensor task.
*************************************************************************/

#include "temperature_sensor.h"


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

uint16_t read_temp_high_low_register(int file_descriptor, int sensor_register){
	
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
		temperature_value += 273.15;
		printf("Temperature value is: %f degree Kelvin \n", temperature_value);
	}
	else if(format == TEMP_FARENHEIT){
		temperature_value = (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.625;
		temperature_value = ((temperature_value * 9)/5 + 32);
		printf("Temperature value is: %f degree Fahrenheit \n", temperature_value);
		
	}
	else{
		printf("Invalid format\n");
	}
	return temperature_value;
	
}

int temp_sensor_init()
{
	if ((temp_fd = open(i2c_name,O_RDWR)) < 0) {
        perror("Failed to open the bus.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
		return -1;
	}
	
	if (ioctl(temp_fd,I2C_SLAVE,I2C_SLAVE_ADDR) < 0) {
		perror("Failed to acquire bus access and/or talk to slave");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		return -1;
	}

    return 0;
}

void *sensor_thread_func(void *arg)
{
	int config_byte[3];
	
	config_byte[0] = I2C_TEMP_SENSOR_CONFIG_REG;
	config_byte[1] = 0X50;
	config_byte[2] = 0XA0;
	
	if(write(temp_fd, config_byte, 3) != 3){
		 printf("Failed to write to the i2c bus.\n");	
	
	}
	
	uint8_t data[3];
	memset(data,0,sizeof(data));
	data[0] = I2C_TEMP_SENSOR_TLOW_REG;
	if (write(temp_fd, data, 1) != 1) {
		perror("temperature low register write error");
	}
	if (read(temp_fd, data, 1) != 1) {
		perror("temperature low register read error");
	}
	printf("data[0]:%d,data[1]:%d \n",data[0],data[1]);
	
	memset(data,0,sizeof(data));
	data[0] = I2C_TEMP_SENSOR_THIGH_REG;
	if (write(temp_fd, data, 1) != 1) {
		perror("temperature high register write error");
	}
	if (read(temp_fd, data, 1) != 1) {
		perror("temperature high register read error");
	}
	printf("data[0]:%d,data[1]:%d,data[2]:%d \n",data[0],data[1],data[2]);
	
   
    while (1)
    {
        memset(data,0,sizeof(data));
        data[0] = I2C_TEMP_SENSOR_TEMP_DATA_REG;
        if (write(temp_fd, data, 1) != 1) {
            perror("temperature data register write error");
        }
	
        if (read(temp_fd, data, 2) != 2) {
            perror("temperature data register read error");
        }
        printf("data[0]:%d,data[1]:%d \n",data[0],data[1]);
        printf("temp value :%3.2f celsius\n", (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.0625);
        
        sleep(2);
    } 
}

void *socket_thread_func(void *arg)
{
    int server_fd;
    struct sockaddr_in server_address;
    int serv_addr_len = sizeof(server_address);

    /* Create the socket */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket creation failed");
        pthread_exit(NULL); // Change these return values from pthread_exit
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(SERVER_PORT_NUM);

    if (bind(server_fd, (struct sockaddr *)&server_address,
								sizeof(server_address))<0)
    {
        perror("bind failed");
        pthread_exit(NULL);
    }

    if (listen(server_fd, SERVER_LISTEN_QUEUE_SIZE) < 0)
    {
        perror("listen failed");
        pthread_exit(NULL);
    }

    int accept_conn_id;
    if ((accept_conn_id = accept(server_fd, (struct sockaddr *)&server_address,
                    (socklen_t*)&serv_addr_len)) < 0)
    {
        perror("accept failed");
        pthread_exit(NULL);
    }

    char recv_buffer[MSG_BUFF_MAX_LEN];
    memset(recv_buffer, '\0', sizeof(recv_buffer));
    
    size_t num_read_bytes = read(accept_conn_id, &recv_buffer, sizeof(recv_buffer));
    printf("Message received: %s\n", recv_buffer);

}

int create_threads()
{
    int sensor_thread_id, socket_thread_id;

    int sens_t_creat_ret_val = pthread_create(&sensor_thread_id, NULL, &sensor_thread_func, NULL);
    if (sens_t_creat_ret_val)
    {
        perror("Sensor thread creation failed");
        return -1;
    }

    int sock_t_creat_ret_val = pthread_create(&socket_thread_id, NULL, &socket_thread_func, NULL);
    if (sock_t_creat_ret_val)
    {
        perror("Socket thread creation failed");
        return -1;
    }

    return 0;
}

int main()
{

	sprintf(i2c_name, "/dev/i2c-2");

    int temp_sensor_init_status = temp_sensor_init();
    if (temp_sensor_init_status == -1)
    {
        printf("Temperature sensor init failed\n");
        exit(1);
    }
    else
    {
        printf("Temperature sensor init success\n");
    }

    int thread_create_status = create_threads();

	close(temp_fd);
		
	return 0;
}

/*************************************************************************
* Author:       Pavan Dhareshwar & Sridhar Pavithrapu
* Date:         03/07/2018
* File:         temperature_sensor.c
* Description:  Source file describing the functionality and implementation
*               of temperature sensor task.
*************************************************************************/

#include "temperature_sensor.h"


void write_pointer_register(int file_descriptor, uint8_t value){

	if (write(file_descriptor, &value, 1) != 1) {
		perror("Write pointer register error\n");
	}
}

void write_temp_high_low_register(int file_descriptor, int sensor_register, uint16_t data ){
	
	/* Writing to the pointer register for reading Tlow register */
	write_pointer_register(file_descriptor, sensor_register);
	
	/* Reading the Tlow register value */
	if (write(file_descriptor, &data, 2) != 2) {
		perror("T-low register write error");
	}
}

uint16_t read_temp_high_low_register(int file_descriptor, int sensor_register){
	
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
	
	float temperature_value;
	uint8_t data[3]={0};

    data[0] = I2C_TEMP_SENSOR_TEMP_DATA_REG;
	/* Writing to the pointer register for reading temperature data register */
	write_pointer_register(file_descriptor, data);
	
	/* Reading the temperature data register value */
	if (read(file_descriptor, data, 2) != 2) {
		perror("Temperature data register read error");
	}
	
	if(format == TEMP_CELSIUS){
		temperature_value = (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.625;
		printf("Temperature value is: %3.2f degree Celsius \n", temperature_value);
	}
	else if(format == TEMP_KELVIN){
		temperature_value = (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.625;
		temperature_value += 273.15;
		printf("Temperature value is: %3.2f degree Kelvin \n", temperature_value);
	}
	else if(format == TEMP_FARENHEIT){
		temperature_value = (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.625;
		temperature_value = ((temperature_value * 9)/5 + 32);
		printf("Temperature value is: %3.2f degree Fahrenheit \n", temperature_value);
		
	}
	else{
		printf("Invalid format\n");
	}
	return temperature_value;
	
}

int temp_sensor_init()
{
	if ((temp_fd = open(I2C_SLAVE_DEV_NAME, O_RDWR)) < 0) {
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

        float temp_data = (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.0625;

        //log_temp_data(temp_data);

        sleep(2);
    } 
}

void log_temp_data(float temp_data)
{
    int msg_priority;

    /* Set the message queue attributes */
    struct mq_attr logger_mq_attr = { .mq_flags = 0,
                                        .mq_maxmsg = MSG_QUEUE_MAX_NUM_MSGS,  // Max number of messages on queue
                                        .mq_msgsize = MSG_QUEUE_MAX_MSG_SIZE  // Max. message size
    };

    mqd_t logger_mq_handle = mq_open(MSG_QUEUE_NAME, O_RDWR, S_IRWXU, &logger_mq_attr);

    char temp_data_msg[MSG_MAX_LEN];
    memset(temp_data_msg, '\0', sizeof(temp_data_msg));

    sprintf(temp_data_msg, "Temp Value: %3.2f", temp_data);

    struct _logger_msg_struct_ logger_msg = {0};
    strcpy(logger_msg.message, temp_data_msg);
    logger_msg.msg_len = strlen(temp_data_msg);
    logger_msg.logger_msg_type = MSG_TYPE_TEMP_DATA;

    msg_priority = 2;
    int num_sent_bytes = mq_send(logger_mq_handle, (char *)&logger_msg, 
            sizeof(logger_msg), msg_priority);
    if (num_sent_bytes < 0)
        perror("mq_send failed");
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

    int option = 1;
    if(setsockopt(server_fd,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(void *)&option,sizeof(option)) < 0)
    {
        perror("setsockopt failed");
        pthread_exit(NULL);
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

    while (1)
    {
        int accept_conn_id;
        printf("Waiting for request...\n");
        if ((accept_conn_id = accept(server_fd, (struct sockaddr *)&server_address,
                        (socklen_t*)&serv_addr_len)) < 0)
        {
            perror("accept failed");
            //pthread_exit(NULL);
        }
        char recv_buffer[MSG_BUFF_MAX_LEN];
        memset(recv_buffer, '\0', sizeof(recv_buffer));

        size_t num_read_bytes = read(accept_conn_id, &recv_buffer, sizeof(recv_buffer));

        printf("[Temp_Task] Message req api: %s, req recp: %s, req api params: %s\n",
                (((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg),
                ((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_recipient)
                 == REQ_RECP_TEMP_TASK ? "Temp Task" : "Light Task"),
                (((struct _socket_req_msg_struct_ *)&recv_buffer)->ptr_param_list != NULL ?
                 ((struct _socket_req_msg_struct_ *)&recv_buffer)->ptr_param_list :"NULL"));
    
        if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "get_temp_data"))
        {
            float temp_data = read_temperature_data_register(temp_fd, TEMP_CELSIUS);
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            sprintf(temp_data_msg, "Temp Data: %3.2f", temp_data);

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }

    }
}

int create_threads()
{
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
    if (thread_create_status)
    {
        printf("Thread creation failed\n");
    }
    else
    {
        printf("Thread creation success\n");
    }

    pthread_join(sensor_thread_id, NULL);
    pthread_join(socket_thread_id, NULL);

    close(temp_fd);

    return 0;
}

/*************************************************************************
* Author:       Pavan Dhareshwar & Sridhar Pavithrapu
* Date:         03/07/2018
* File:         temperature_sensor.c
* Description:  Source file describing the functionality and implementation
*               of temperature sensor task.
*************************************************************************/

#include "temperature_sensor.h"
#include "wrapper.h"

void write_pointer_register(uint8_t value){

	if (wrapper_write(file_descriptor, &value, 1) != 1) {
		perror("wrapper_write pointer register error\n");
	}
}

void write_temp_high_low_register(int sensor_register, int16_t data ){
	
	/* Writing to the pointer register for reading T_High/T_low register */
	write_pointer_register(sensor_register);
	
	/* Writing the T_High/T_low register value */
	if (wrapper_write(file_descriptor, &data, 2) != 2) {
		perror("T-low register wrapper_write error");
	}
}


void write_config_register_on_off(uint8_t data ){
	
	/* Writing to the pointer register for configuration register */
	write_pointer_register(I2C_TEMP_SENSOR_CONFIG_REG);
	if((data == 0) || (data == 1)){
		default_config_byte_one |= data;
		
		/* Writing data to the configuration register */
		if (wrapper_write(file_descriptor, &default_config_byte_one, 1) != 1) {
			perror("Configuration register wrapper_write error for first byte");
		}
		
		if (wrapper_write(file_descriptor, &default_config_byte_two, 1) != 1) {
			perror("Configuration register wrapper_write error for second byte");
		}
	}
}

void write_config_register_em(uint8_t data ){
	
	/* Writing to the pointer register for configuration register */
	write_pointer_register(I2C_TEMP_SENSOR_CONFIG_REG);
	if((data == 0) || (data == 1)){
        
        uint16_t config_reg_data;
        config_reg_data = read_temp_config_register();
        printf("CONFIG_REG_DATA: %d\n", config_reg_data);
        
        config_reg_data = config_reg_data & (uint16_t)(~0x10);

        config_reg_data |= (uint16_t)(data << 4);

        uint8_t config_high_data = (uint8_t)(config_reg_data >> 8);
        uint8_t config_low_data = (uint8_t)(config_reg_data & 0XFF);

		//default_config_byte_two |= (data << 4);
		
		/* Writing data to the configuration register */
		if (wrapper_write(file_descriptor, &config_high_data, 1) != 1) {
			perror("Configuration register wrapper_write error for first byte");
		}
		
		if (wrapper_write(file_descriptor, &config_low_data, 1) != 1) {
			perror("Configuration register wrapper_write error for second byte");
		}
	}
}

void write_config_register_conversion_rate(uint8_t data ){
	
	/* Writing to the pointer register for configuration register */
	write_pointer_register(I2C_TEMP_SENSOR_CONFIG_REG);
	if((data >= 0) || (data <= 3)){
        uint16_t config_reg_data;
        config_reg_data = read_temp_config_register();
        config_reg_data = config_reg_data & (uint16_t)(~0xC0);
		
        config_reg_data |= (uint16_t)(data << 6);
        
        uint8_t config_high_data = (uint8_t)(config_reg_data >> 8);
        uint8_t config_low_data = (uint8_t)(config_reg_data & 0XFF);
		
		/* Writing data to the configuration register */
		if (wrapper_write(file_descriptor, &config_high_data, 1) != 1) {
			perror("Configuration register wrapper_write error for first byte");
		}
		
		if (wrapper_write(file_descriptor, &config_low_data, 1) != 1) {
			perror("Configuration register wrapper_write error for second byte");
		}
	}
}

void write_config_register_fault_bits(uint8_t data ){
	
	/* Writing to the pointer register for configuration register */
	write_pointer_register(I2C_TEMP_SENSOR_CONFIG_REG);
	if((data >= 0) || (data <= 3)){
        uint16_t config_reg_data;
        config_reg_data = read_temp_config_register();
        config_reg_data = config_reg_data & (uint16_t)(~0x1800);
		
        config_reg_data |= (uint16_t)(data << 11);
        
        uint8_t config_high_data = (uint8_t)(config_reg_data >> 8);
        uint8_t config_low_data = (uint8_t)(config_reg_data & 0XFF);
		
		/* Writing data to the configuration register */
		if (wrapper_write(file_descriptor, &config_high_data, 1) != 1) {
			perror("Configuration register wrapper_write error for first byte");
		}
		
		if (wrapper_write(file_descriptor, &config_low_data, 1) != 1) {
			perror("Configuration register wrapper_write error for second byte");
		}
	}
}

uint8_t read_config_register_fault_bits(){

    /* Reading fault bits of temperature config register */
    uint16_t config_value = read_temp_config_register();
    uint8_t return_value = (uint8_t)((config_value & 0x1800) >> 11);
    return return_value;

}

uint8_t read_config_register_em(){

    /* Reading em-bit of temperature config register */
    uint16_t config_value = read_temp_config_register();
#define TEMP_CONF_REG_EM_BM       0x10

    uint8_t return_value = (config_value & TEMP_CONF_REG_EM_BM) >> 4;
    return return_value;

}

uint8_t read_config_register_conversion_rate(){

    /* Reading conversion rate of temperature config register */
    uint16_t config_value = read_temp_config_register();
    uint8_t return_value = (uint8_t)((config_value & 0x00C0) >> 6);
    return return_value;
}

void write_config_register_default( ){
	
	/* Writing to the pointer register for configuration register */
	write_pointer_register(I2C_TEMP_SENSOR_CONFIG_REG);
	
	/* Writing data to the configuration register */
	if (wrapper_write(file_descriptor, &default_config_byte_one, 1) != 1) {
		perror("Configuration register wrapper_write error for first byte");
	}
	
	if (wrapper_write(file_descriptor, &default_config_byte_two, 1) != 1) {
		perror("Configuration register wrapper_write error for second byte");
	}
}

int16_t read_temp_high_low_register(int sensor_register){
	
	int16_t tlow_output_value;
    int8_t *ptr_tlow_val = (int8_t *)&tlow_output_value;
	int8_t data[2]={0};
	
	/* Writing to the pointer register for reading Tlow register */
	write_pointer_register(sensor_register);
	
	/* Reading the Tlow register value */
	if (wrapper_read(file_descriptor, data, 1) != 1) {
		perror("T-low register wrapper_read error");
	}

    printf("data[0]: %d, data[1]:%d\n", data[0], data[1]);

	tlow_output_value = ((int16_t)data[0] | ((int16_t)((data[1] & 0XF) << 8)));
	printf("T-low register value is: %d \n", tlow_output_value);

	return tlow_output_value;
	
}

uint16_t read_temp_config_register(){
	
	uint16_t temp_config_value;
	uint8_t data[2]={0};
	
	/* Writing to the pointer register for reading THigh register */
	write_pointer_register(I2C_TEMP_SENSOR_CONFIG_REG);
	
	/* Reading the THigh register value */
	if (wrapper_read(file_descriptor, data, 2) != 2) {
		perror("Temperature configuration register wrapper_read error");
	}

    printf("data[0]: %d, data[1]:%d\n", data[0], data[1]);
	
	temp_config_value = (((int16_t)data[0])<<8 | ((int16_t)data[1]));
	printf("Temperature configuration register value is: %d \n", temp_config_value);
	return temp_config_value;
	
}

float read_temperature_data_register(int format){
	
	float temperature_value;
	uint8_t data[3]={0};

	/* Writing to the pointer register for reading temperature data register */
	write_pointer_register(I2C_TEMP_SENSOR_TEMP_DATA_REG);
	
	/* Reading the temperature data register value */
	if (wrapper_read(file_descriptor, data, 2) != 2) {
		perror("Temperature data register wrapper_read error");
	}
	
	if(format == TEMP_CELSIUS){
		temperature_value = (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.0625;
		printf("Temperature value is: %3.2f degree Celsius \n", temperature_value);
	}
	else if(format == TEMP_KELVIN){
		temperature_value = (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.0625;
		temperature_value += 273.15;
		printf("Temperature value is: %3.2f degree Kelvin \n", temperature_value);
	}
	else if(format == TEMP_FARENHEIT){
		temperature_value = (data[0]<<4 | (data[1] >> 4 & 0XF)) * 0.0625;
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
	if ((file_descriptor = open(I2C_SLAVE_DEV_NAME, O_RDWR)) < 0) {
        perror("Failed to open the bus.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
		return -1;
	}
	
	if (ioctl(file_descriptor,I2C_SLAVE,I2C_SLAVE_ADDR) < 0) {
		perror("Failed to acquire bus access and/or talk to slave");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		return -1;
	}

    if (temp_sensor_initialized == 0)
        temp_sensor_initialized = 1;

    return 0;
}

void *sensor_thread_func(void *arg)
{
	
	write_config_register_default();
	float temp_value;
   
    while (!g_sig_kill_sensor_thread)
    {
        temp_value = read_temperature_data_register(TEMP_CELSIUS);

        log_temp_data(temp_value);

        sleep(10);
    } 

    pthread_exit(NULL);

    return NULL;
}

void log_temp_data(float temp_data)
{
    int msg_priority;

    /* Set the message queue attributes */
    struct mq_attr logger_mq_attr = { .mq_flags = 0,
                                        .mq_maxmsg = MSG_QUEUE_MAX_NUM_MSGS,  // Max number of messages on queue
                                        .mq_msgsize = MSG_QUEUE_MAX_MSG_SIZE  // Max. message size
    };

    logger_mq_handle = mq_open(MSG_QUEUE_NAME, O_RDWR, S_IRWXU, &logger_mq_attr);

    char temp_data_msg[MSG_MAX_LEN];
    memset(temp_data_msg, '\0', sizeof(temp_data_msg));

    sprintf(temp_data_msg, "Temp Value: %3.2f", temp_data);

    struct _logger_msg_struct_ logger_msg;
    memset(&logger_msg, '\0', sizeof(logger_msg));
    strcpy(logger_msg.message, temp_data_msg);
    strncpy(logger_msg.logger_msg_src_id, "Temp", strlen("Temp"));
    logger_msg.logger_msg_src_id[strlen("Temp")] = '\0';
    strncpy(logger_msg.logger_msg_level, "Info", strlen("Info"));
    logger_msg.logger_msg_level[strlen("Info")] = '\0';

    msg_priority = 2;
    int num_sent_bytes = mq_send(logger_mq_handle, (char *)&logger_msg, 
            sizeof(logger_msg), msg_priority);
    if (num_sent_bytes < 0)
        perror("mq_send failed");
}

void init_sock(int *sock_fd, struct sockaddr_in *server_addr_struct, 
               int port_num, int listen_qsize)
{
    int serv_addr_len = sizeof(struct sockaddr_in);

    /* Create the socket */
    if ((*sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket creation failed");
        pthread_exit(NULL); // Change these return values from pthread_exit
    }

    int option = 1;
    if(setsockopt(*sock_fd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (void *)&option, sizeof(option)) < 0)
    {
        perror("setsockopt failed");
        pthread_exit(NULL);
    }

    server_addr_struct->sin_family = AF_INET;
    server_addr_struct->sin_addr.s_addr = INADDR_ANY;
    server_addr_struct->sin_port = htons(port_num);

    if (bind(*sock_fd, (struct sockaddr *)server_addr_struct,
								sizeof(struct sockaddr_in))<0)
    {
        perror("bind failed");
        pthread_exit(NULL);
    }

    if (listen(*sock_fd, listen_qsize) < 0)
    {
        perror("listen failed");
        pthread_exit(NULL);
    }

}

void *socket_thread_func(void *arg)
{
    int server_fd;
    struct sockaddr_in server_address;
    int serv_addr_len = sizeof(server_address);

    init_sock(&server_fd, &server_address, SERVER_PORT_NUM, SERVER_LISTEN_QUEUE_SIZE);

    int accept_conn_id;
    printf("Waiting for request...\n");
    if ((accept_conn_id = accept(server_fd, (struct sockaddr *)&server_address,
                    (socklen_t*)&serv_addr_len)) < 0)
    {
        perror("accept failed");
        //pthread_exit(NULL);
    }
    
    char recv_buffer[MSG_BUFF_MAX_LEN];
    
    while (!g_sig_kill_sock_thread)
    {
        memset(recv_buffer, '\0', sizeof(recv_buffer));

        size_t num_read_bytes = read(accept_conn_id, &recv_buffer, sizeof(recv_buffer));

        printf("[Temp_Task] Message req api: %s, req recp: %s, req api params: %d\n",
                (((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg),
                ((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_recipient)
                 == REQ_RECP_TEMP_TASK ? "Temp Task" : "Light Task"),
                (int)(((struct _socket_req_msg_struct_ *)&recv_buffer)->params));

    
        if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "get_temp_data"))
        {
            float temp_data = read_temperature_data_register(TEMP_CELSIUS);
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            sprintf(temp_data_msg, "Temp Data: %3.2f", temp_data);

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
		else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "get_temp_low_data"))
        {
            int16_t temp_data = read_temp_high_low_register(I2C_TEMP_SENSOR_TLOW_REG);
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            sprintf(temp_data_msg, "Tlow Data: %d", temp_data);

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
		else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "get_temp_high_data"))
        {
            int16_t temp_data = read_temp_high_low_register(I2C_TEMP_SENSOR_THIGH_REG);
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            sprintf(temp_data_msg, "T_High Data: %d", temp_data);

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
		else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "get_temp_em"))
        {
            uint8_t temp_data = read_config_register_em();
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            sprintf(temp_data_msg, "Temp EM data: %d", temp_data);

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
		else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "get_temp_conversion_rate"))
        {
            uint8_t temp_data = read_config_register_conversion_rate();
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            sprintf(temp_data_msg, "T_High Data: %d", temp_data);

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
		else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "get_temp_conf_data"))
        {
            uint16_t temp_data = read_temp_config_register();
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            sprintf(temp_data_msg, "Conf Data: %d", temp_data);

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
		else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "set_temp_on_off"))
        {
            uint8_t data = (uint8_t)(((struct _socket_req_msg_struct_ *)&recv_buffer)->params);
            write_config_register_on_off(data);
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            sprintf(temp_data_msg, "%s", "Set success");

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
		else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "set_temp_em"))
        {
            uint8_t data = (uint8_t)(((struct _socket_req_msg_struct_ *)&recv_buffer)->params);
            write_config_register_em(data);
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            sprintf(temp_data_msg, "%s", "Set success");

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
		else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "set_temp_conversion_rate"))
        {
            uint8_t data = (uint8_t)(((struct _socket_req_msg_struct_ *)&recv_buffer)->params);
            write_config_register_conversion_rate(data);
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            sprintf(temp_data_msg, "%s", "Set success");

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
		else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "set_temp_high_data"))
        {
            int16_t data = (int16_t)(((struct _socket_req_msg_struct_ *)&recv_buffer)->params);
            write_temp_high_low_register(I2C_TEMP_SENSOR_THIGH_REG,data);
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            sprintf(temp_data_msg, "%s", "Set success");

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
		else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "set_temp_low_data"))
        {
            int16_t data = (int16_t)(((struct _socket_req_msg_struct_ *)&recv_buffer)->params);
            write_temp_high_low_register(I2C_TEMP_SENSOR_TLOW_REG,data);
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            sprintf(temp_data_msg, "%s", "Set success");

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
        else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "set_temp_fault_bits"))
        {
            uint8_t data = (uint8_t)(((struct _socket_req_msg_struct_ *)&recv_buffer)->params);
            write_config_register_fault_bits(data);
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            sprintf(temp_data_msg, "%s", "Set success");

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
		else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "get_temp_fault_bits"))
        {
            uint8_t temp_data = read_config_register_fault_bits();
            char temp_data_msg[64];
            memset(temp_data_msg, '\0', sizeof(temp_data_msg));

            printf("Fault Bits: %d", temp_data);
            sprintf(temp_data_msg, "Fault Bits: %d", temp_data);

            ssize_t num_sent_bytes = send(accept_conn_id, temp_data_msg, strlen(temp_data_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
    }

    printf("Calling pthread_exit in sock thread\n");
    pthread_exit(NULL);

    return NULL;
}

void *socket_hb_thread_func(void *arg)
{
    int sock_hb_fd;
    struct sockaddr_in sock_hb_address;
    int sock_hb_addr_len = sizeof(sock_hb_address);

    init_sock(&sock_hb_fd, &sock_hb_address, SOCKET_HB_PORT_NUM, SOCKET_HB_LISTEN_QUEUE_SIZE);

    int accept_conn_id;
    printf("Waiting for request...\n");
    if ((accept_conn_id = accept(sock_hb_fd, (struct sockaddr *)&sock_hb_address,
                    (socklen_t*)&sock_hb_addr_len)) < 0)
    {
        perror("accept failed");
        //pthread_exit(NULL);
    }
    
    char recv_buffer[MSG_BUFF_MAX_LEN];
    char send_buffer[20];
    memset(send_buffer, '\0', sizeof(send_buffer));
    
    while (!g_sig_kill_sock_hb_thread)
    {
        memset(recv_buffer, '\0', sizeof(recv_buffer));

        size_t num_read_bytes = read(accept_conn_id, &recv_buffer, sizeof(recv_buffer));
    
        if (!strcmp(recv_buffer, "heartbeat"))
        {
            strcpy(send_buffer, "Alive");
			ssize_t num_sent_bytes = send(accept_conn_id, send_buffer, strlen(send_buffer), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
        else if (!strcmp(recv_buffer, "startup_check"))
        {
            /* For the sake of start-up check, because we have the temperature sensor initialized
            ** by the time this thread is spawned. So we perform a "get_temp_data" call to see if
            ** everything is working fine */
            if (temp_sensor_initialized == 1)
                strcpy(send_buffer, "Initialized");
            else
                strcpy(send_buffer, "Uninitialized");

			ssize_t num_sent_bytes = send(accept_conn_id, send_buffer, strlen(send_buffer), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
    }

    printf("Calling pthread_exit in sock hb thread\n");
    pthread_exit(NULL);

    return NULL;
}

void sig_handler(int sig_num)
{
	char buffer[MSG_BUFF_MAX_LEN];
	memset(buffer, '\0', sizeof(buffer));

	if (sig_num == SIGINT || sig_num == SIGUSR1)
	{
        if (sig_num == SIGINT)
            printf("Caught signal %s in temperature task\n", "SIGINT");
        else if (sig_num == SIGUSR1)
            printf("Caught signal %s in temperature task\n", "SIGKILL");
   
        g_sig_kill_sensor_thread = 1;
        g_sig_kill_sock_thread = 1;
        g_sig_kill_sock_hb_thread = 1;
	
        //pthread_join(sensor_thread_id, NULL);
        //pthread_join(socket_thread_id, NULL);
        //pthread_join(socket_hb_thread_id, NULL);

        mq_close(logger_mq_handle);     

        close(file_descriptor);

        exit(0);
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
    
    int sock_hb_t_creat_ret_val = pthread_create(&socket_hb_thread_id, NULL, &socket_hb_thread_func, NULL);
    if (sock_hb_t_creat_ret_val)
    {
        perror("Socket heartbeat thread creation failed");
        return -1;
    }

    return 0;
}

int main()
{
    temp_sensor_initialized = 0;

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

    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("SigHandler setup for SIGINT failed\n");
    
    if (signal(SIGUSR1, sig_handler) == SIG_ERR)
        printf("SigHandler setup for SIGKILL failed\n");

    g_sig_kill_sensor_thread = 0;
    g_sig_kill_sock_thread = 0;
    g_sig_kill_sock_hb_thread = 0;

    pthread_join(sensor_thread_id, NULL);
    pthread_join(socket_thread_id, NULL);
    pthread_join(socket_hb_thread_id, NULL);

    close(file_descriptor);

    return 0;
}

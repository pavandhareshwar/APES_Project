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

int main(void){

    int init_ret_val = light_sensor_init();
    if (init_ret_val == -1)
    {
        printf("Light sensor init failed\n");
        exit(1);
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

    /* Power on the APDS-9301 device */
    power_on_light_sensor();

    return 0;
}

void power_on_light_sensor(void)
{
    int cmd_ctrl_reg_val = I2C_LIGHT_SENSOR_CMD_CTRL_REG;
	
	int ctrl_reg_val = I2C_LIGHT_SENSOR_CTRL_REG_VAL;
	
    write_light_sensor_reg(cmd_ctrl_reg_val, ctrl_reg_val);

}

int create_threads(void)
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

void init_light_socket(struct sockaddr_in *sock_addr_struct)
{
    
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

    sock_addr_struct->sin_family = AF_INET;
    sock_addr_struct->sin_addr.s_addr = INADDR_ANY;
    sock_addr_struct->sin_port = htons(LIGHT_SENSOR_SERVER_PORT_NUM);

    if (bind(server_fd, (struct sockaddr *)sock_addr_struct,
								sizeof(struct sockaddr_in))<0)
    {
        perror("bind failed");
        pthread_exit(NULL);
    }

    if (listen(server_fd, LIGHT_SENSOR_LISTEN_QUEUE_SIZE) < 0)
    {
        perror("listen failed");
        pthread_exit(NULL);
    }
}

void *socket_thread_func(void *arg)
{
    struct sockaddr_in server_address;
    int serv_addr_len = sizeof(server_address);

    init_light_socket(&server_address);

    char recv_buffer[MSG_BUFF_MAX_LEN];
    
    int accept_conn_id;
    printf("Waiting for request...\n");
    if ((accept_conn_id = accept(server_fd, (struct sockaddr *)&server_address,
                    (socklen_t*)&serv_addr_len)) < 0)
    {
        perror("accept failed");
        //pthread_exit(NULL);
    }

    while (1)
    {
        memset(recv_buffer, '\0', sizeof(recv_buffer));

        size_t num_read_bytes = read(accept_conn_id, &recv_buffer, sizeof(recv_buffer));

        printf("[Light_Task] Message req api: %s, req recp: %s, req api params: %s\n",
                (((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg),
                ((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_recipient)
                 == REQ_RECP_TEMP_TASK ? "Temp Task" : "Light Task"),
                (((struct _socket_req_msg_struct_ *)&recv_buffer)->ptr_param_list != NULL ?
                 ((struct _socket_req_msg_struct_ *)&recv_buffer)->ptr_param_list :"NULL"));

        char light_sensor_rsp_msg[64];
        if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "get_lux_data"))
        {
            float lux_data = get_lux_data();
            memset(light_sensor_rsp_msg, '\0', sizeof(light_sensor_rsp_msg));

            sprintf(light_sensor_rsp_msg, "Lux Data: %3.2f", lux_data);
        
            ssize_t num_sent_bytes = send(accept_conn_id, light_sensor_rsp_msg, strlen(light_sensor_rsp_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");

        }
        else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "get_light_sensor_id"))
        {
            uint8_t cmd_id_reg = I2C_LIGHT_SENSOR_CMD_ID_REG;
        
            int8_t light_sen_id_reg_val = read_light_sensor_reg(cmd_id_reg);
            printf("id reg val : %d\n", light_sen_id_reg_val);

            memset(light_sensor_rsp_msg, '\0', sizeof(light_sensor_rsp_msg));

            sprintf(light_sensor_rsp_msg, "ID reg val: 0x%x", light_sen_id_reg_val);
            
            ssize_t num_sent_bytes = send(accept_conn_id, light_sensor_rsp_msg, strlen(light_sensor_rsp_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");

        }
        else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "get_light_sensor_ctrl_reg"))
        {
            uint8_t cmd_ctrl_reg = I2C_LIGHT_SENSOR_CMD_CTRL_REG;
        
            int8_t light_sen_ctrl_reg_val = read_light_sensor_reg(cmd_ctrl_reg);
            printf("ctrl reg val : %d\n", light_sen_ctrl_reg_val);

            memset(light_sensor_rsp_msg, '\0', sizeof(light_sensor_rsp_msg));

            sprintf(light_sensor_rsp_msg, "Ctrl reg val: 0x%x", light_sen_ctrl_reg_val);
            
            ssize_t num_sent_bytes = send(accept_conn_id, light_sensor_rsp_msg, strlen(light_sensor_rsp_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
        else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "set_light_sensor_ctrl_reg"))
        {
            if (((struct _socket_req_msg_struct_ *)&recv_buffer)->ptr_param_list != NULL)
            {
                uint8_t cmd_ctrl_reg = I2C_LIGHT_SENSOR_CMD_CTRL_REG;
                
                uint8_t cmd_ctrl_reg_val = *(uint8_t *)(((struct _socket_req_msg_struct_ *)&recv_buffer)->ptr_param_list);
                
                if (write_light_sensor_reg(cmd_ctrl_reg, cmd_ctrl_reg_val) == 0)
                {
                    memset(light_sensor_rsp_msg, '\0', sizeof(light_sensor_rsp_msg));

                    sprintf(light_sensor_rsp_msg, "OK");
                }
                
                ssize_t num_sent_bytes = send(accept_conn_id, light_sensor_rsp_msg, strlen(light_sensor_rsp_msg), 0);
                if (num_sent_bytes < 0)
                    perror("send failed");
            }
        }
        else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "get_light_sensor_tim_reg"))
        {
            uint8_t cmd_tim_reg = I2C_LIGHT_SENSOR_CMD_TIM_REG;
        
            int8_t light_sen_tim_reg_val = read_light_sensor_reg(cmd_tim_reg);
            printf("tim reg val : %d\n", light_sen_tim_reg_val);

            memset(light_sensor_rsp_msg, '\0', sizeof(light_sensor_rsp_msg));

            sprintf(light_sensor_rsp_msg, "Ctrl reg val: 0x%x", light_sen_tim_reg_val);
            
            ssize_t num_sent_bytes = send(accept_conn_id, light_sensor_rsp_msg, strlen(light_sensor_rsp_msg), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
        else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "set_light_sensor_tim_reg"))
        {
            if (((struct _socket_req_msg_struct_ *)&recv_buffer)->ptr_param_list != NULL)
            {
                uint8_t cmd_tim_reg = I2C_LIGHT_SENSOR_CMD_TIM_REG;
                
                uint8_t cmd_tim_reg_val = *(uint8_t *)(((struct _socket_req_msg_struct_ *)&recv_buffer)->ptr_param_list);
                
                if (write_light_sensor_reg(cmd_tim_reg, cmd_tim_reg_val) == 0)
                {
                    memset(light_sensor_rsp_msg, '\0', sizeof(light_sensor_rsp_msg));

                    sprintf(light_sensor_rsp_msg, "OK");
                    ssize_t num_sent_bytes = send(accept_conn_id, light_sensor_rsp_msg, strlen(light_sensor_rsp_msg), 0);
                    if (num_sent_bytes < 0)
                        perror("send failed");
                    }
            }
        }
        else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "get_light_sensor_int_thresh_reg"))
        {
            uint8_t cmd_thresh_low_low_reg = I2C_LIGHT_SENSOR_CMD_THRESH_LOW_LOW_REG;
            uint8_t cmd_thresh_low_high_reg = I2C_LIGHT_SENSOR_CMD_THRESH_LOW_HIGH_REG;
            uint8_t cmd_thresh_high_low_reg = I2C_LIGHT_SENSOR_CMD_THRESH_HIGH_LOW_REG;
            uint8_t cmd_thresh_high_high_reg = I2C_LIGHT_SENSOR_CMD_THRESH_HIGH_HIGH_REG;
        
            int8_t light_sen_thresh_low_low_reg_val = read_light_sensor_reg(cmd_thresh_low_low_reg);
            printf("thresh low low reg val : %d\n", light_sen_thresh_low_low_reg_val);
            
            int8_t light_sen_thresh_low_high_reg_val = read_light_sensor_reg(cmd_thresh_low_high_reg);
            printf("thresh low high reg val : %d\n", light_sen_thresh_low_high_reg_val);
            
            int8_t light_sen_thresh_high_low_reg_val = read_light_sensor_reg(cmd_thresh_high_low_reg);
            printf("thresh high low reg val : %d\n", light_sen_thresh_high_low_reg_val);
            
            int8_t light_sen_thresh_high_high_reg_val = read_light_sensor_reg(cmd_thresh_high_high_reg);
            printf("thresh high high reg val : %d\n", light_sen_thresh_high_high_reg_val);

            memset(light_sensor_rsp_msg, '\0', sizeof(light_sensor_rsp_msg));

            struct _int_thresh_reg_struct_ int_thresh_reg_struct;
            int_thresh_reg_struct.thresh_low_low = light_sen_thresh_low_low_reg_val;
            int_thresh_reg_struct.thresh_low_high = light_sen_thresh_low_high_reg_val;
            int_thresh_reg_struct.thresh_high_low = light_sen_thresh_high_low_reg_val;
            int_thresh_reg_struct.thresh_high_high = light_sen_thresh_high_high_reg_val;
            
            ssize_t num_sent_bytes = send(accept_conn_id, &int_thresh_reg_struct, 
                                            sizeof(struct _int_thresh_reg_struct_), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
        else if (!strcmp((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg), "set_light_sensor_int_thresh_reg"))
        {
            if (((struct _socket_req_msg_struct_ *)&recv_buffer)->ptr_param_list != NULL)
            {
                struct _int_thresh_reg_struct_ *p_int_thresh_reg_struct = 
                    (struct _int_thresh_reg_struct_ *)(((struct _socket_req_msg_struct_ *)&recv_buffer)->ptr_param_list);

                uint8_t cmd_thresh_low_low_reg = I2C_LIGHT_SENSOR_CMD_THRESH_LOW_LOW_REG;
                uint8_t cmd_thresh_low_low_reg_val = (uint8_t)p_int_thresh_reg_struct->thresh_low_low;
                write_light_sensor_reg(cmd_thresh_low_low_reg, cmd_thresh_low_low_reg_val);
                
                uint8_t cmd_thresh_low_high_reg = I2C_LIGHT_SENSOR_CMD_THRESH_LOW_HIGH_REG;
                uint8_t cmd_thresh_low_high_reg_val = (uint8_t)p_int_thresh_reg_struct->thresh_low_high;
                write_light_sensor_reg(cmd_thresh_low_high_reg, cmd_thresh_low_high_reg_val);
                
                uint8_t cmd_thresh_high_low_reg = I2C_LIGHT_SENSOR_CMD_THRESH_HIGH_LOW_REG;
                uint8_t cmd_thresh_high_low_reg_val = (uint8_t)p_int_thresh_reg_struct->thresh_high_low;
                write_light_sensor_reg(cmd_thresh_high_low_reg, cmd_thresh_high_low_reg_val);
                
                uint8_t cmd_thresh_high_high_reg = I2C_LIGHT_SENSOR_CMD_THRESH_HIGH_HIGH_REG;
                uint8_t cmd_thresh_high_high_reg_val = (uint8_t)p_int_thresh_reg_struct->thresh_high_high;
                write_light_sensor_reg(cmd_thresh_high_high_reg, cmd_thresh_high_high_reg_val);

                sprintf(light_sensor_rsp_msg, "OK");
                ssize_t num_sent_bytes = send(accept_conn_id, light_sensor_rsp_msg, strlen(light_sensor_rsp_msg), 0);
                if (num_sent_bytes < 0)
                    perror("send failed");
            }
        }
        else
        {
            printf("Invalid request from socket task\n");
        }
    }
}

void *sensor_thread_func(void *arg)
{
    while (1)
    {
        float sensor_lux_data = get_lux_data();

        printf("Sensor lux data: %3.2f\n", sensor_lux_data);
        
        //log_lux_data(sensor_lux_data);
        
        sleep(5);
    }
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
    char send_buffer[] = "Alive";
    
    while (1)
    {
        memset(recv_buffer, '\0', sizeof(recv_buffer));

        size_t num_read_bytes = read(accept_conn_id, &recv_buffer, sizeof(recv_buffer));
    
        if (!strcmp(recv_buffer, "heartbeat"))
        {
			ssize_t num_sent_bytes = send(accept_conn_id, send_buffer, strlen(send_buffer), 0);
            if (num_sent_bytes < 0)
                perror("send failed");
        }
    }

}

float get_lux_data(void)
{
    float sensor_lux_val = 0;

    int adc_ch0_data, adc_ch1_data; 

    get_adc_channel_data(0, &adc_ch0_data);
    get_adc_channel_data(1, &adc_ch1_data);

    sensor_lux_val = calculate_lux_value(adc_ch0_data, adc_ch1_data);

    printf("Sensor lux value: %3.2f\n", sensor_lux_val);

    return sensor_lux_val;
}

void get_adc_channel_data(int channel_num, int *ch_data)
{
    if (channel_num == 0)
    {
        uint8_t cmd_data0_low_reg = I2C_LIGHT_SENSOR_CMD_DATA0LOW_REG;
        uint8_t cmd_data0_high_reg = I2C_LIGHT_SENSOR_CMD_DATA0HIGH_REG;
        
        int8_t ch_data_low = read_light_sensor_reg(cmd_data0_low_reg);
        //printf("data0_low : %d\n", ch_data_low);

        int8_t ch_data_high = read_light_sensor_reg(cmd_data0_high_reg);
        //printf("data0_high : %d\n", ch_data_high);

        *ch_data = ch_data_high << 8 | ch_data_low;
    }
    else if (channel_num == 1)
    {
        uint8_t cmd_data1_low_reg = I2C_LIGHT_SENSOR_CMD_DATA1LOW_REG;
        uint8_t cmd_data1_high_reg = I2C_LIGHT_SENSOR_CMD_DATA1HIGH_REG;
        
        int8_t ch_data_low = read_light_sensor_reg(cmd_data1_low_reg);
        //printf("data1_low : %d\n", ch_data_low);
	
        int8_t ch_data_high = read_light_sensor_reg(cmd_data1_high_reg);
        //printf("data1_high : %d\n", ch_data_high);
        
        *ch_data = ch_data_high << 8 | ch_data_low;
    }
    else 
    {
        printf("Channel number %d invalid\n", channel_num);
    }
}

float calculate_lux_value(int ch0_data, int ch1_data)
{
    float sensor_lux_val = 0;
    
    if (ch0_data == 0 || ch1_data == 0)
        return 0;

    /* Mapping between ADC channel data and the sensor lux formula used 
    **       CH1/CH0                                Sensor lux formula
    ** 
    **  0 < CH1/CH0 ≤ 0.50           Sensor Lux = (0.0304 x CH0) – (0.062 x CH0 x ((CH1/CH0)^1.4))
    **  0.50 < CH1/CH0 ≤ 0.61        Sensor Lux = (0.0224 x CH0) – (0.031 x CH1)
    **  0.61 < CH1/CH0 ≤ 0.80        Sensor Lux = (0.0128 x CH0) – (0.0153 x CH1)
    **  0.80 < CH1/CH0 ≤ 1.30        Sensor Lux = (0.00146 x CH0) – (0.00112 x CH1)
    **  CH1/CH0>1.30                 Sensor Lux = 0
    ** 
    */

    float adc_count_ratio = (float)ch1_data/ch0_data;
    if ( 0 < adc_count_ratio <= 0.5)
    {
        sensor_lux_val = ((0.0304 * ch0_data) - (0.062 * ch0_data * pow(adc_count_ratio, 1.4)));
    }
    else if (0.5 < adc_count_ratio <= 0.61)
    {
        sensor_lux_val = ((0.0224 * ch0_data) - (0.031 * ch1_data));
    }
    else if (0.61 < adc_count_ratio <= 0.8)
    {
        sensor_lux_val = ((0.0128 * ch0_data) - (0.0153 * ch1_data));
    }
    else if (0.8 < adc_count_ratio <= 1.3)
    {
        sensor_lux_val = ((0.00146 * ch0_data) - (0.00112 * ch1_data));
    }
    else if (adc_count_ratio > 1.3)
    {
        sensor_lux_val = 0;
    }

    return sensor_lux_val;
}

int write_light_sensor_reg(int cmd_reg_val, int target_reg_val)
{

    /* Write the command register to specify the following two information 
    **       1. Target register address for subsequent write operation
    **       2. If I2C write operation is a word or byte operation 
    */
	if (write(i2c_light_sensor_fd, &cmd_reg_val, 1) != 1)
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
    int read_val;
	if (read(i2c_light_sensor_fd, &read_val, 1) != 1) {
		perror("adc data read error");
        return -1;
	}
    //printf("**** read val for %d: %d\n", read_reg_val, read_val);

    int8_t ret_val = (int8_t)read_val;

    return ret_val;
}

void log_lux_data(float lux_data)
{
    int msg_priority;

    /* Set the message queue attributes */
    struct mq_attr logger_mq_attr = { .mq_flags = 0,
                                      .mq_maxmsg = MSG_QUEUE_MAX_NUM_MSGS,  // Max number of messages on queue
                                      .mq_msgsize = MSG_QUEUE_MAX_MSG_SIZE  // Max. message size
                                    };

    mqd_t logger_mq_handle = mq_open(MSG_QUEUE_NAME, O_RDWR, S_IRWXU, &logger_mq_attr);

    char lux_data_msg[128];
    memset(lux_data_msg, '\0', sizeof(lux_data_msg));

    sprintf(lux_data_msg, "Lux Value: %3.2f", lux_data);
    
    struct _logger_msg_struct_ logger_msg = {0};
    strcpy(logger_msg.message, lux_data_msg);
    logger_msg.msg_len = strlen(lux_data_msg);
    logger_msg.logger_msg_type = MSG_TYPE_LUX_DATA;

    msg_priority = 1;
    int num_sent_bytes = mq_send(logger_mq_handle, (char *)&logger_msg, 
                            sizeof(logger_msg), msg_priority);
    if (num_sent_bytes < 0)
        perror("mq_send failed");
}

void light_sensor_exit(void)
{
    /* Close i2c bus */
    if (i2c_light_sensor_fd != -1)
        close(i2c_light_sensor_fd);
}

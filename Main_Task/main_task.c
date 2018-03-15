/***************************************************************************
* Author:       Pavan Dhareshwar & Sridhar Pavithrapu
* Date:         03/11/2018
* File:         main_task.c
* Description:  Source file containing the functionality and implementation
*               of the main task
***************************************************************************/

#include "main_task.h"

int main(void)
{
	char buffer[BUFF_SIZE];

    //create_sub_processes();

    /* Create and initialize temperature task socket */
    initialize_sub_task_socket(&temp_task_sockfd, &temp_task_sock_addr, TEMP_TASK_PORT_NUM);

    if (connect(temp_task_sockfd, (struct sockaddr *)&temp_task_sock_addr, sizeof(temp_task_sock_addr)) < 0)
    {
        printf("\nConnection Failed for temp task \n");
        return -1;
    }

#if 0
    /* Create and initialize light task socket */
    initialize_sub_task_socket(&light_task_sockfd, &light_task_sock_addr, LIGHT_TASK_PORT_NUM);

    if (connect(light_task_sockfd, (struct sockaddr *)&light_task_sock_addr, sizeof(light_task_sock_addr)) < 0)
    {
        printf("\nConnection Failed for light task \n");
        return -1;
    }
    
    /* Create and initialize socket task socket */
    initialize_sub_task_socket(&socket_task_sockfd, &socket_task_sock_addr, SOCKET_TASK_PORT_NUM);

    if (connect(socket_task_sockfd, (struct sockaddr *)&socket_task_sock_addr, sizeof(socket_task_sock_addr)) < 0)
    {
        printf("\nConnection Failed for socket task \n");
        return -1;
    }

    /* Create and initialize logger task socket */
    initialize_sub_task_socket(&logger_task_sockfd, &logger_task_sock_addr, LOGGER_TASK_PORT_NUM);

    if (connect(logger_task_sockfd, (struct sockaddr *)&logger_task_sock_addr, sizeof(logger_task_sock_addr)) < 0)
    {
        printf("\nConnection Failed for logger task \n");
        return -1;
    }
#endif 
    
    //perform_startup_test();

    while(1)
    {
        //check_status_of_sub_tasks();

        sleep(10);
    }

	return 0;	
}

void create_sub_processes(void)
{
    char sub_process_name[32];

#if 0
    /* Creating temperature sensor task */
    memset(sub_process_name, '\0', sizeof(sub_process_name));
    strcpy(sub_process_name, "temperature"); 
    create_sub_process(sub_process_name);
    
    /* Creating light sensor task */
    memset(sub_process_name, '\0', sizeof(sub_process_name));
    strcpy(sub_process_name, "light"); 
    create_sub_process(sub_process_name);
    
    /* Creating socket task */
    memset(sub_process_name, '\0', sizeof(sub_process_name));
    strcpy(sub_process_name, "socket"); 
    create_sub_process(sub_process_name);
#endif

    /* Creating logger task */
    memset(sub_process_name, '\0', sizeof(sub_process_name));
    strcpy(sub_process_name, "logger"); 
    create_sub_process(sub_process_name);
}

void create_sub_process(char *process_name)
{
    pid_t child_pid;

    child_pid = fork();

    if (child_pid == 0)
    {
        /* Child Process */
        if (!strcmp(process_name, "temperature"))
        {    
            char *args[]={TEMP_SENSOR_TASK_EXEC_NAME, NULL};
            execvp(args[0],args);
        }
        else if (!strcmp(process_name, "light"))
        {    
            char *args[]={LIGHT_SENSOR_TASK_EXEC_NAME, NULL};
            execvp(args[0],args);
        }
        else if (!strcmp(process_name, "socket"))
        {    
            char *args[]={SOCKET_TASK_EXEC_NAME, NULL};
            execvp(args[0],args);
        }
        else if (!strcmp(process_name, "logger"))
        {    
            printf("Creating logger task\n");
            //char *args[]={LOGGER_TASK_EXEC_NAME, NULL};
            char *args[]={"./logger_task", NULL};
            execvp(args[0],args);
        }
    }
    else if (child_pid > 0)
    {
        /* Parent Process */
        /* We are just returning back to main function in the parent process */
    }
    else 
    {
        printf("fork failed while creating child process for %s task\n", process_name);
        perror("fork failed");
    }
  
    return;

}

void initialize_sub_task_socket(int *sock_fd, struct sockaddr_in *sock_addr_struct, int port_num)
{
	memset(sock_addr_struct, '0', sizeof(struct sockaddr_in));
	sock_addr_struct->sin_family = AF_INET;
	sock_addr_struct->sin_port = htons(port_num);

    if ((*sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    int option = 1;
    if(setsockopt(*sock_fd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR),
                (char*)&option,sizeof(option)) < 0)
    {
        perror("setsockopt for socket reusability failed");
        close(*sock_fd);
        exit(EXIT_FAILURE);
    }

    struct timeval rcv_timeout;
    rcv_timeout.tv_sec = 5;
    if (setsockopt(*sock_fd, SOL_SOCKET, (SO_SNDTIMEO | SO_RCVTIMEO), (struct timeval *)&rcv_timeout,sizeof(struct timeval)) < 0)
    {
        perror("setsockopt for recv timeout set failed");
        close(*sock_fd);
        exit(EXIT_FAILURE);
    }

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, SENSOR_TASK_SOCK_IP_ADDR, &(sock_addr_struct->sin_addr))<=0)
    {
        perror("inet_pton failed");
        printf("\nInvalid address/ Address not supported for temperature task\n");
        exit(EXIT_FAILURE);
    }
}

void check_status_of_sub_tasks(void)
{
    /* Check if temperature task is alive */
    check_subtask_status(temp_task_sockfd, "Temperature"); 

    /* Check if light task is alive */
    check_subtask_status(light_task_sockfd, "Light"); 

    /* Check if socket task is alive */
    check_subtask_status(socket_task_sockfd, "Socket"); 

    /* Check if logger task is alive */
    check_subtask_status(logger_task_sockfd, "Logger");
}

void check_subtask_status(int sock_fd, char *task_name)
{
    char recv_buffer[BUFF_SIZE];
    char send_buffer[] = "heartbeat";
    
    memset(recv_buffer, '\0', sizeof(recv_buffer));

    ssize_t num_sent_bytes = send(sock_fd, send_buffer, sizeof(send_buffer), 0);
    if (num_sent_bytes < 0)
        perror("send failed");

    ssize_t num_recv_bytes = recv(sock_fd, recv_buffer, sizeof(recv_buffer), 0);
    if (num_recv_bytes < 0)
        perror("recv failed");

    if (!strcmp(recv_buffer, "Alive"))
        printf("%s task alive\n", task_name);
    else
    {
        if (!strcmp(task_name, "Temperature"))
        {
            temp_task_unalive_count++;

            if (temp_task_unalive_count >= TEMP_TASK_UNALIVE_CNT_LOG_LIMIT)
                log_task_unalive_msg_to_log_file(task_name);
        }
        else if (!strcmp(task_name, "Light"))
        {
            light_task_unalive_count++;

            if (light_task_unalive_count >= LIGHT_TASK_UNALIVE_CNT_LOG_LIMIT)
                log_task_unalive_msg_to_log_file(task_name);
        }
        else if (!strcmp(task_name, "Logger"))
        {
            logger_task_unalive_count++;

            if (logger_task_unalive_count >= LOGGER_TASK_UNALIVE_CNT_LOG_LIMIT)
                log_task_unalive_msg_to_log_file(task_name);
        }
        else if (!strcmp(task_name, "Socket"))
        {
            socket_task_unalive_count++;

            if (socket_task_unalive_count >= SOCK_TASK_UNALIVE_CNT_LOG_LIMIT)
                log_task_unalive_msg_to_log_file(task_name);
        }
    }
}

void perform_startup_test(void)
{
    /* The startup test will validate whether the hardware and software is in
    ** working order.
    **
    ** Specifically, it checks the following things: 
    **
    ** 1. Communication with the temperature sensor to confirm that I2C interface
    **    works and the hardware is working.
    ** 2. Communication with the light sensor to confirm that I2C interface
    **    works and the hardware is working.
    ** 3. Communication to the sub processes to make sure they have all started
    **    and are up and running 
    */

    /* Check the temperature sensor task, hardware and I2C */
    int temp_task_st_status = perform_sub_task_startup_test(temp_task_sockfd);
    if (temp_task_st_status != 0)
        stop_entire_system();

#if 0
    /* Check the light sensor task, hardware and I2C */
    int light_task_st_status = perform_sub_task_startup_test(light_task_sockfd);     
    if (light_task_st_status != 0)
        stop_entire_system();
    
    /* Check the logger task */
    int logger_task_st_status = perform_sub_task_startup_test(logger_task_sockfd);
    if (logger_task_st_status != 0)
        stop_entire_system();
    
    /* Check the socket task */
    int socket_task_st_status = perform_sub_task_startup_test(socket_task_sockfd);
    if (socket_task_st_status != 0)
        stop_entire_system();
#endif

}

int perform_sub_task_startup_test(int sock_fd)
{
    char recv_buffer[BUFF_SIZE];
    char send_buffer[] = "startup_check";
    
    memset(recv_buffer, '\0', sizeof(recv_buffer));

    ssize_t num_sent_bytes = send(sock_fd, send_buffer, sizeof(send_buffer), 0);
    if (num_sent_bytes < 0)
        perror("send failed");

    ssize_t num_recv_bytes = recv(sock_fd, recv_buffer, sizeof(recv_buffer), 0);
    if (num_recv_bytes < 0)
        perror("recv failed");

    if (!strcmp(recv_buffer, "Initialized"))
    {
        printf("Temperature sensor is initialized\n");
        return 0;
    }
    else if (!strcmp(recv_buffer, "Uninitialized"))
    {
        printf("Temperature sensor isn't initalized\n");
        return -1;
    }
    else
    {
        printf("Message received on socket : %s unknown\n", recv_buffer);
        return -1;
    }
}

void stop_entire_system(void)
{
    kill_already_created_processes();

    /* Turn on USR led to indicate that a failure has occurred */
    turn_on_usr_led();

    exit(1);
}

void kill_already_created_processes(void)
{
    printf("Killing already created processes\n");
}

void turn_on_usr_led(void)
{
    printf("Turning on USR led\n");
}

void log_task_unalive_msg_to_log_file(char *task_name)
{
    int msg_priority;

    /* Set the message queue attributes */
    struct mq_attr logger_mq_attr = { .mq_flags = 0,
                                      .mq_maxmsg = MSG_QUEUE_MAX_NUM_MSGS,  // Max number of messages on queue
                                      .mq_msgsize = MSG_QUEUE_MAX_MSG_SIZE  // Max. message size
                                    };

    mqd_t logger_mq_handle = mq_open(MSG_QUEUE_NAME, O_RDWR, S_IRWXU, &logger_mq_attr);

    char main_task_data_msg[MSG_MAX_LEN];
    memset(main_task_data_msg, '\0', sizeof(main_task_data_msg));

    sprintf(main_task_data_msg, "%s task not alive", task_name);
    
    struct _logger_msg_struct_ logger_msg = {0};
    strcpy(logger_msg.message, main_task_data_msg);
    logger_msg.msg_len = strlen(main_task_data_msg);
    logger_msg.logger_msg_type = MSG_TYPE_MAIN_DATA;

    msg_priority = 1;
    int num_sent_bytes = mq_send(logger_mq_handle, (char *)&logger_msg, 
                            sizeof(logger_msg), msg_priority);
    if (num_sent_bytes < 0)
        perror("mq_send failed");
}

#include "socket_task.h"

int main(void)
{
	/* Creating a socket that is exposed to the external application */
    initialize_server_socket(&server_addr, SERVER_PORT_NUM, SERVER_LISTEN_QUEUE_SIZE);

    initialize_sensor_task_socket(&temp_sockfd, &temp_sock_addr, TEMPERATURE_TASK_PORT_NUM);

    if (connect(temp_sockfd, (struct sockaddr *)&temp_sock_addr, sizeof(temp_sock_addr)) < 0)
    {
        perror("connect failed");
        printf("\nConnection Failed for temperature task \n");
        return -1;
    }
   
    initialize_sensor_task_socket(&light_sockfd, &light_sock_addr, LIGHT_TASK_PORT_NUM);

    if (connect(light_sockfd, (struct sockaddr *)&light_sock_addr, sizeof(light_sock_addr)) < 0)
    {
        printf("\nConnection Failed for light task \n");
        return -1;
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

    g_sig_kill_sock_thread = 0;
    g_sig_kill_sock_hb_thread = 0;

    pthread_join(socket_thread_id, NULL);
    pthread_join(socket_hb_thread_id, NULL);

	return 0;
}

void initialize_server_socket(struct sockaddr_in *sock_addr_struct, 
                                int port_num, int listen_queue_size)
{
	/* Create server socket */ 
	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}	

    int option = 1;
    if(setsockopt(server_sockfd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR),
                    (char*)&option,sizeof(option)) < 0)
    {
        printf("setsockopt failed\n");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }

    sock_addr_struct->sin_family = AF_INET;
	sock_addr_struct->sin_addr.s_addr = INADDR_ANY;
	sock_addr_struct->sin_port = htons(port_num);

	if (bind(server_sockfd, (struct sockaddr *)sock_addr_struct, 
							sizeof(struct sockaddr_in))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(server_sockfd, listen_queue_size) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

}

void initialize_sensor_task_socket(int *sock_fd, struct sockaddr_in *sock_addr_struct, int port_num)
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
        perror("setsockopt failed");
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

int create_threads(void)
{
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

void *socket_thread_func(void *args)
{
	int serv_addr_len = sizeof(server_addr);
	char buffer[BUFF_SIZE];
    
    int accept_conn_id;
    /* Wait for request from external application */
    if ((accept_conn_id = accept(server_sockfd, (struct sockaddr *)&server_addr, 
                    (socklen_t *)&serv_addr_len)) < 0)
    {
        perror("accept");
    }

    char recv_buffer[BUFF_SIZE];
	while(!g_sig_kill_sock_thread)
    {
        memset(recv_buffer, '\0', sizeof(recv_buffer));
        int num_recv_bytes = recv(accept_conn_id, recv_buffer, sizeof(recv_buffer), 0);
        if (num_recv_bytes < 0)
        {
            printf("recv failed in socket task\n");
            perror("recv failed");
        }
        else
        {

            if (*(((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg) != '\0')
            {
                printf("Message req api: %s, req recp: %s, req api params: %s\n",
                        (((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg),
                        ((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_recipient) 
                         == REQ_RECP_TEMP_TASK ? "Temp Task" : "Light Task"),
                        (((struct _socket_req_msg_struct_ *)&recv_buffer)->ptr_param_list != NULL ?
                         "Non NULL" : "NULL"));

                //log_req_msg((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg));

                memset(buffer, '\0', sizeof(buffer));
                //strncpy(buffer, "Hello!", strlen("Hello!"));
   
                size_t sent_bytes;
                if ((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_recipient) 
                        == REQ_RECP_TEMP_TASK)
                {
                    printf("Sending request to temperature task\n");
                    sent_bytes = send(temp_sockfd, recv_buffer, sizeof(recv_buffer), 0);

                    ssize_t num_recv_bytes = recv(temp_sockfd, buffer, sizeof(buffer), 0);
                    if (num_recv_bytes < 0)
                        perror("recv failed");
                    //strncpy(buffer, "Hello!", strlen("Hello!"));
                    sent_bytes = send(accept_conn_id, buffer, strlen(buffer), 0);
                }
                else
                {
                    printf("Sending request to light task\n");
                    sent_bytes = send(light_sockfd, recv_buffer, sizeof(recv_buffer), 0);

                    ssize_t num_recv_bytes = recv(light_sockfd, buffer, sizeof(buffer), 0);
                    if (num_recv_bytes < 0)
                        perror("recv failed");
                    //strncpy(buffer, "Hello!", strlen("Hello!"));
                    sent_bytes = send(accept_conn_id, buffer, strlen(buffer), 0);
                }
                
                //sent_bytes = send(accept_conn_id, buffer, sizeof(buffer), 0 );
            }
        }
	}

    pthread_exit(NULL);
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
    
    while (!g_sig_kill_sock_hb_thread)
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

    pthread_exit(NULL);

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

void log_req_msg(char *req_msg)
{
    int msg_priority;

    /* Set the message queue attributes */
    struct mq_attr logger_mq_attr = { .mq_flags = 0,
                                      .mq_maxmsg = MSG_QUEUE_MAX_NUM_MSGS,  // Max number of messages on queue
                                      .mq_msgsize = MSG_QUEUE_MAX_MSG_SIZE  // Max. message size
                                    };

    logger_mq_handle = mq_open(MSG_QUEUE_NAME, O_RDWR, S_IRWXU, &logger_mq_attr);

    char sock_data_msg[MSG_MAX_LEN];
    memset(sock_data_msg, '\0', sizeof(sock_data_msg));

    sprintf(sock_data_msg, "Req Msg: %s", req_msg);

    struct _logger_msg_struct_ logger_msg = {0};
    strcpy(logger_msg.message, sock_data_msg);
    logger_msg.msg_len = strlen(sock_data_msg);
    logger_msg.logger_msg_type = MSG_TYPE_SOCK_DATA;

    msg_priority = 1;
    int num_sent_bytes = mq_send(logger_mq_handle, (char *)&logger_msg,
                            sizeof(logger_msg), msg_priority);
    if (num_sent_bytes < 0)
        perror("mq_send failed");
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

        g_sig_kill_sock_thread = 1;
        g_sig_kill_sock_hb_thread = 1;

        //pthread_join(sensor_thread_id, NULL);
        //pthread_join(socket_thread_id, NULL);
        //pthread_join(socket_hb_thread_id, NULL);

        mq_close(logger_mq_handle);

        exit(0);
    }
}

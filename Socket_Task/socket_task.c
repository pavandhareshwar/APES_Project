#include "socket_task.h"

int main(void)
{
	int serv_addr_len = sizeof(server_addr);
	char buffer[BUFF_SIZE];

	/* Creating a socket that is exposed to the external application */
    initialize_server_socket(&server_addr, SERVER_PORT_NUM, SERVER_LISTEN_QUEUE_SIZE);

    initialize_sensor_task_socket(&temp_sockfd, &temp_sock_addr, TEMPERATURE_TASK_PORT_NUM);

    if (connect(temp_sockfd, (struct sockaddr *)&temp_sock_addr, sizeof(temp_sock_addr)) < 0)
    {
        perror("connect failed");
        printf("\nConnection Failed for temperature task \n");
        return -1;
    }
   
#if 0
    initialize_sensor_task_socket(&light_sockfd, &light_sock_addr, LIGHT_TASK_PORT_NUM);

    if (connect(light_sockfd, (struct sockaddr *)&light_sock_addr, sizeof(light_sock_addr)) < 0)
    {
        printf("\nConnection Failed for light task \n");
        return -1;
    }
#endif

	int accept_conn_id;
	while(1){
	
        /* Wait for request from external application */
		if ((accept_conn_id = accept(server_sockfd, (struct sockaddr *)&server_addr, 
					   (socklen_t *)&serv_addr_len)) < 0)
		{
			perror("accept");
		}
	
        char recv_buffer[BUFF_SIZE];
        memset(recv_buffer, '\0', sizeof(recv_buffer));
        int num_recv_bytes = recv(accept_conn_id, recv_buffer, sizeof(recv_buffer), 0);
        if (num_recv_bytes < 0)
        {
            printf("recv failed in socket task\n");
            perror("recv failed");
        }
        else
        {
            printf("Message req api: %s, req recp: %s, req api params: %s\n",
                    (((struct _socket_req_msg_struct_ *)&recv_buffer)->req_api_msg),
                    ((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_recipient) 
                     == REQ_RECP_TEMP_TASK ? "Temp Task" : "Light Task"),
                    (((struct _socket_req_msg_struct_ *)&recv_buffer)->ptr_param_list != NULL ?
                     ((struct _socket_req_msg_struct_ *)&recv_buffer)->ptr_param_list : "NULL"));
            
            memset(buffer, '\0', sizeof(buffer));
            strncpy(buffer, "Hello!", strlen("Hello!"));
            
            size_t sent_bytes;
            if ((((struct _socket_req_msg_struct_ *)&recv_buffer)->req_recipient) 
                        == REQ_RECP_TEMP_TASK)
            {
                printf("Sending request to temperature task\n");
                sent_bytes = send(temp_sockfd, recv_buffer, sizeof(recv_buffer), 0);

                ssize_t num_recv_bytes = recv(temp_sockfd, buffer, sizeof(buffer), 0);
                if (num_recv_bytes < 0)
                    perror("recv failed");

                sent_bytes = send(accept_conn_id, buffer, strlen(buffer), 0);
            }
            else
            {
            }

            //sent_bytes = send(light_sockfd, buffer , sizeof(buffer) , 0);
            //sent_bytes = send(accept_conn_id, buffer, sizeof(buffer), 0 );

        }
	}
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

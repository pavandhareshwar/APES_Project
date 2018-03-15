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

    /* Create and initialize temperature task socket */
    initialize_sub_task_socket(&temp_task_sockfd, &temp_task_sock_addr, TEMP_TASK_PORT_NUM);

    if (connect(temp_task_sockfd, (struct sockaddr *)&temp_task_sock_addr, sizeof(temp_task_sock_addr)) < 0)
    {
        printf("\nConnection Failed for temp task \n");
        return -1;
    }

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

    while(1)
    {
        check_status_of_sub_tasks();

        sleep(10);
    }

	return 0;	
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

}

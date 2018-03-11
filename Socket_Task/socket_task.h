/*************************************************************************
* Author:       Pavan Dhareshwar & Sridhar Pavithrapu
* Date:         03/07/2018
* File:         light_sensor.h
* Description:  Header file containing the macros, structs/enums, globals
                and function prototypes for source file light_sensor.c
*************************************************************************/

#ifndef _SOCKET_TASK_H_
#define _SOCKET_TASK_H_

/*---------------------------------- INCLUDES -------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

/*---------------------------------- INCLUDES -------------------------------*/

/*----------------------------------- MACROS --------------------------------*/
#define SERVER_PORT_NUM 			        8500
#define SERVER_LISTEN_QUEUE_SIZE	        100

#define TEMPERATURE_TASK_PORT_NUM 			8081
#define TEMPERATURE_TASK_QUEUE_SIZE			100

#define LIGHT_TASK_PORT_NUM 			    8085
#define LIGHT_TASK_QUEUE_SIZE			    100

#define SENSOR_TASK_SOCK_IP_ADDR            "127.0.0.1"   

#define BUFF_SIZE                           1024

#define SOCK_REQ_MSG_API_MSG_LEN            64

/*----------------------------------- MACROS --------------------------------*/

/*---------------------------------- GLOBALS --------------------------------*/
int server_sockfd, temp_sockfd, light_sockfd;
struct sockaddr_in server_addr, temp_sock_addr, light_sock_addr;

/*---------------------------------- GLOBALS --------------------------------*/

/*---------------------------- STRUCTURES/ENUMERATIONS ----------------------*/
enum _req_recipient_
{
    REQ_RECP_TEMP_TASK,
    REQ_RECP_LIGHT_TASK
};

struct _socket_req_msg_struct_
{
    char req_api_msg[SOCK_REQ_MSG_API_MSG_LEN];
    enum _req_recipient_ req_recipient;
    void *ptr_param_list;
};


/*---------------------------- STRUCTURES/ENUMERATIONS ----------------------*/

/*---------------------------- FUNCTION PROTOTYPES --------------------------*/

/**
 *  @brief Initialize server socket
 *  
 *  For an external application to communicate with the system, the socket 
 *  task creates and listens on a socket for messages exposed to the external
 *  application. This function creates, binds and makes the socket task listen 
 *  on this socket for messages from external application
 *
 *  @param sock_addr_struct        : sockaddr_in structre pointer
 *  @param port_num                : port number associated with the socket
 *  @param listen_queue_size       : backlog argument for listen system call
 *
 *  @return void
 *
 */
void initialize_server_socket(struct sockaddr_in *sock_addr_struct, 
                                int port_num, int listen_queue_size);

/**
 *  @brief Initialize sensor interface socket
 *  
 *  For the socket task to forward the request from the external application, the 
 *  socket task creates and uses sockets with temperature and light task to send
 *  the requests and get the response back from the respective sensor task. This 
 *  function creates and initializes the socket listen on this socket on the 
 *  socket task side for communication with the respective sensor task.
 *
 *  @param sock_fd                 : pointer socket file descriptor
 *  @param sock_addr_struct        : sockaddr_in structure pointer
 *  @param port_num                : port number associated with the socket
 *
 *  @return void
 *
 */
void initialize_sensor_task_socket(int *sock_fd, struct sockaddr_in *sock_addr_struct, 
                                    int port_num);

/*---------------------------- FUNCTION PROTOTYPES --------------------------*/

#endif // _SOCKET_TASK_H_


/*************************************************************************
* Author:       Pavan Dhareshwar & Sridhar Pavithrapu
* Date:         03/11/2018
* File:         main_task.h
* Description:  Header file containing the macros, structs/enums, globals
                and function prototypes for source file main_task.c
*************************************************************************/

#ifndef _MAIN_TASK_H_
#define _MAIN_TASK_H_

/*---------------------------------- INCLUDES -------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <netinet/in.h>
#include <arpa/inet.h>

/*---------------------------------- INCLUDES -------------------------------*/

/*----------------------------------- MACROS --------------------------------*/
#define TEMP_TASK_PORT_NUM 			        8650
#define TEMP_TASK_QUEUE_SIZE			    10

#define LIGHT_TASK_PORT_NUM 			    8660
#define LIGHT_TASK_QUEUE_SIZE			    10

#define SOCKET_TASK_PORT_NUM 			    8670
#define SOCKET_TASK_QUEUE_SIZE			    10

#define LOGGER_TASK_PORT_NUM 			    8680
#define LOGGER_TASK_QUEUE_SIZE			    10

#define SENSOR_TASK_SOCK_IP_ADDR            "127.0.0.1"   

#define BUFF_SIZE                           1024

/*----------------------------------- MACROS --------------------------------*/

/*---------------------------------- GLOBALS --------------------------------*/
int temp_task_sockfd, light_task_sockfd; 
int socket_task_sockfd, logger_task_sockfd;

struct sockaddr_in temp_task_sock_addr, light_task_sock_addr;
struct sockaddr_in socket_task_sock_addr, logger_task_sock_addr;

/*---------------------------------- GLOBALS --------------------------------*/

/*---------------------------- STRUCTURES/ENUMERATIONS ----------------------*/

/*---------------------------- STRUCTURES/ENUMERATIONS ----------------------*/

/*---------------------------- FUNCTION PROTOTYPES --------------------------*/

/**
 *  @brief Initialize sub tasks interface socket
 *  
 *  For the main task to check the status of each of the remaining tasks, it 
 *  sends a heartbeat message to each of these tasks and when it receives a
 *  reply, it knows that the task is alive. For the main task to check the 
 *  status, it uses socket as an IPC mechanism.
 *
 *  This function creates a socket between main task and the sensor task for
 *  communication.
 *
 *  @param sock_fd                 : pointer socket file descriptor
 *  @param sock_addr_struct        : sockaddr_in structure pointer
 *  @param port_num                : port number associated with the socket
 *
 *  @return void
 *
 */
void initialize_sub_task_socket(int *sock_fd, struct sockaddr_in *sock_addr_struct, 
                                int port_num);

/**
 *  @brief Check status of a specified sub task 
 *  
 *  This function checks the status of the specified subtask to see if it 
 *  is alive
 *
 *  @param sock_fd                 : socket file descriptor for the task 
 *  @param task_name               : name of the subtask
 *
 *  @return void
 *
 */
void check_subtask_status(int sock_fd, char *task_name);

/**
 *  @brief Check status of sub tasks 
 *  
 *  This function checks the status of each of the subtasks to see if they 
 *  are alive
 *
 *  @param void
 *
 *  @return void
 *
 */
void check_status_of_sub_tasks(void);

/*---------------------------- FUNCTION PROTOTYPES --------------------------*/

#endif // _MAIN_TASK_H_

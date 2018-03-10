/* Headers Section */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define SERVER_PORT_NUM 			        8080
#define SERVER_LISTEN_QUEUE_SIZE	        100

#define TEMPERATURE_TASK_PORT_NUM 			8081
#define TEMPERATURE_TASK_QUEUE_SIZE			100

#define LIGHT_TASK_PORT_NUM 			    8085
#define LIGHT_TASK_QUEUE_SIZE			    100

#define BUFF_SIZE                           1024

/* Global variables */
int server_sockfd, temp_sockfd, light_sockfd;
struct sockaddr_in server_addr, temp_sock_addr, light_sock_addr;

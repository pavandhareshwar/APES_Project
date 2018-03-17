/*************************************************************************
* Author:       Pavan Dhareshwar & Sridhar Pavithrapu
* Date:         03/08/2018
* File:         logger_task.h
* Description:  Header file containing the macros, structs/enums, globals
                and function prototypes for source file logger_task.c
*************************************************************************/

#ifndef _LOGGER_TASK_H_
#define _LOGGER_TASK_H_

/*---------------------------------- INCLUDES -------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#include <mqueue.h>

#include <netinet/in.h>
#include <arpa/inet.h>
/*---------------------------------- INCLUDES -------------------------------*/

/*----------------------------------- MACROS --------------------------------*/
// Message queue attribute macros 
#define MSG_QUEUE_MAX_NUM_MSGS               5
#define MSG_QUEUE_MAX_MSG_SIZE               1024
#define MSG_QUEUE_NAME                       "/logger_task_mq"

#define LOGGER_FILE_PATH                     "./"
#define LOGGER_FILE_NAME                     "logger_file.txt"

#define LOG_MSG_PAYLOAD_SIZE                 256
#define MSG_MAX_LEN                          128

#define MSG_BUFF_MAX_LEN                     1024

#define LOGGER_FILE_PATH_LEN                 256           
#define LOGGER_FILE_NAME_LEN                 64           

#define SOCKET_HB_PORT_NUM                   8680
#define SOCKET_HB_LISTEN_QUEUE_SIZE          10

#define MSG_TYPE_TEMP_DATA                   0
#define MSG_TYPE_LUX_DATA                    1
#define MSG_TYPE_SOCK_DATA                   2
#define MSG_TYPE_MAIN_DATA                   3

#define LOGGER_ATTR_LEN                      32

/*----------------------------------- MACROS --------------------------------*/

/*---------------------------------- GLOBALS --------------------------------*/
mqd_t logger_mq_handle;
int logger_fd;
pthread_t logger_thread_id, socket_hb_thread_id;

sig_atomic_t g_sig_kill_logger_thread, g_sig_kill_sock_hb_thread;

/*---------------------------------- GLOBALS --------------------------------*/

/*---------------------------- STRUCTURES/ENUMERATIONS ----------------------*/
struct _logger_msg_struct_                                                                            
{                                                                                                     
    char message[MSG_MAX_LEN]; 
    char logger_msg_src_id[LOGGER_ATTR_LEN];
    char logger_msg_level[LOGGER_ATTR_LEN];
};

/*---------------------------- STRUCTURES/ENUMERATIONS ----------------------*/

/*---------------------------- FUNCTION PROTOTYPES --------------------------*/
/**
 *  @brief Initialize the logger task
 *  
 *  This funcition will create the message queue for logger task and
 *  open a file handle of logger file for writing. (If the logger file
 *  already exists, it is deleted and a fresh one is created). 
 *
 *  @param void
 *
 *  @return 0  : if sensor initialization is a success
            -1 : if sensor initialization fails
*/
int logger_task_init();

int read_logger_conf_file(char *file);

int create_threads(void);

void *logger_thread_func(void *arg);

void *socket_hb_thread_func(void *arg);

void init_sock(int *sock_fd, struct sockaddr_in *server_addr_struct,
               int port_num, int listen_qsize);

void write_test_msg_to_logger();

void read_from_logger_msg_queue();

void logger_task_exit();

/**
 *  @brief Signal handler for temperature task
 *
 *  This function handles the reception of SIGKILL and SIGINT signal to the
 *  temperature task and terminates all the threads, closes the I2C file descriptor
 *  and logger message queue handle and exits.
 *
 *  @param sig_num              : signal number
 *
 *  @return void
*/

void sig_handler(int sig_num);
#endif // _LOGGER_TASK_H_

/*************************************************************************
* Author:       Pavan Dhareshwar & Sridhar Pavithrapu
* Date:         03/08/2018
* File:         logger_task.c
* Description:  Source file describing the functionality and implementation
*               of logger task.
*************************************************************************/

#include "logger_task.h"

int main(void)
{
    int init_status = logger_task_init();
    if (init_status == -1)
    {
        printf("logger task initialization failed\n");
        exit(1);
    }

    //write_test_msg_to_logger();

    read_from_logger_msg_queue();

    logger_task_exit();

    return 0;
}

int logger_task_init()
{
    /* In the logger task init function, we create the message queue */

    /* Set the message queue attributes */
    struct mq_attr logger_mq_attr = { .mq_flags = 0,
                                      .mq_maxmsg = MSG_QUEUE_MAX_NUM_MSGS,  // Max number of messages on queue
                                      .mq_msgsize = MSG_QUEUE_MAX_MSG_SIZE  // Max. message size 
                                    };


    logger_msg_queue = mq_open(MSG_QUEUE_NAME, O_CREAT | O_RDWR, S_IRWXU, &logger_mq_attr);
    if (logger_msg_queue < 0)
    {
        perror("Logger message queue create failed");
        return -1;
    }

    printf("Logger message queue successfully created\n");
    
    char filename[100];
    memset(filename, '\0', sizeof(filename));
    sprintf(filename, "%s%s", LOGGER_FILE_PATH, LOGGER_FILE_NAME);

    if (open(filename, O_RDONLY) != -1)
    {
        printf("Logger file exists. Deleting existing file.\n");
        remove(filename);
        sync();
    }
   
    printf("Trying to create file %s\n", filename);
    logger_fd = creat(filename ,(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
    if (logger_fd == -1)
    {
        perror("Logger file open failed");
        return -1;
    }
    else
    {
        printf("Logger file open success\n");
    }

    return 0;
}

void write_test_msg_to_logger()
{
    struct _logger_msg_struct_ logger_msg = {0};

    const char test_msg[] = "Testing if msg queue comm works";
    strcpy(logger_msg.message, test_msg);
    logger_msg.msg_len = strlen(test_msg);

    logger_msg.logger_msg_type = MSG_TYPE_TEMP_DATA;

    int msg_priority = 1;
    int num_sent_bytes = mq_send(logger_msg_queue, (char *)&logger_msg, 
                                    sizeof(logger_msg), msg_priority);

    if (num_sent_bytes < 0)
        perror("mq_send failed");
}

void read_from_logger_msg_queue()
{
    char recv_buffer[MSG_MAX_LEN];
    memset(recv_buffer, '\0', sizeof(recv_buffer));

    int msg_priority;
    
    int num_recv_bytes;
    while (num_recv_bytes = mq_receive(logger_msg_queue, (char *)&recv_buffer,
                                    MSG_QUEUE_MAX_MSG_SIZE, &msg_priority) != -1)
    {
        if (num_recv_bytes < 0)
            perror("mq_receive failed");

        printf("Message received: %s, msg_type: %d, message type: %s\n", 
            (((struct _logger_msg_struct_ *)&recv_buffer)->message),
            (((struct _logger_msg_struct_ *)&recv_buffer)->logger_msg_type),
            (((((struct _logger_msg_struct_ *)&recv_buffer)->logger_msg_type) == MSG_TYPE_TEMP_DATA) ?
             "Temp Data" : ((((struct _logger_msg_struct_ *)&recv_buffer)->logger_msg_type) 
                 == MSG_TYPE_LUX_DATA) ? "Lux Data" : "Sock Data"));

        char msg_to_write[LOG_MSG_PAYLOAD_SIZE];
        memset(msg_to_write, '\0', sizeof(msg_to_write));
        sprintf(msg_to_write, "Message: %s | Message_Type: %s\n", 
            (((struct _logger_msg_struct_ *)&recv_buffer)->message),
            (((((struct _logger_msg_struct_ *)&recv_buffer)->logger_msg_type) == MSG_TYPE_TEMP_DATA) ?
             "TEMP_DATA" : ((((struct _logger_msg_struct_ *)&recv_buffer)->logger_msg_type) == MSG_TYPE_LUX_DATA) ?
             "LUX_DATA" : "SOCK_DATA"));

        printf("Message to write: %s\n", msg_to_write);
        int num_written_bytes = write(logger_fd, msg_to_write, strlen(msg_to_write));
        printf("num_written_bytes: %d\n", num_written_bytes);
    }

}

void logger_task_exit()
{
    int mq_close_status = mq_close(logger_msg_queue);
    if (mq_close_status == -1)
        perror("Logger message queue close failed");

    if (logger_fd)
        close(logger_fd);

}

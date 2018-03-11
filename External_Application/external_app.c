/***************************************************************************
* Author:       Pavan Dhareshwar & Sridhar Pavithrapu
* Date:         03/10/2018
* File:         external_app.c
* Description:  Source file containing the functionality and implementation
*               of external application
***************************************************************************/

#include "external_app.h"

int main(void)
{
    int client_sock;
    struct sockaddr_in serv_addr;
    
    char buffer[BUFF_SIZE];
    memset(buffer, '\0', sizeof(buffer));
    
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT_NUM);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(client_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    struct _socket_req_msg_struct_ ext_app_req_msg = {0};
    strcpy(ext_app_req_msg.req_api_msg, "get_lux_data");
    ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
    ext_app_req_msg.ptr_param_list = NULL;

    printf("Sending %s request to socket task\n", ext_app_req_msg.req_api_msg);
    ssize_t num_sent_bytes = send(client_sock, &ext_app_req_msg, 
            sizeof(struct _socket_req_msg_struct_), 0);
    if (num_sent_bytes < 0)
    {
        perror("send failed");
    }
    else
    {
        /* Receiving message from parent process */
        size_t num_read_bytes = read(client_sock, buffer, sizeof(buffer));
        printf("Message received in external app : %s\n", buffer);
    }
   
    sleep(2);

    strcpy(ext_app_req_msg.req_api_msg, "get_light_sensor_int_thresh_reg");
    ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
    ext_app_req_msg.ptr_param_list = NULL;

    printf("Sending %s request to socket task\n", ext_app_req_msg.req_api_msg);
    num_sent_bytes = send(client_sock, &ext_app_req_msg, 
            sizeof(struct _socket_req_msg_struct_), 0);
    if (num_sent_bytes < 0)
    {
        perror("send failed");
    }
    else
    {
        /* Receiving message from parent process */
        size_t num_read_bytes = read(client_sock, buffer, sizeof(buffer));
        printf("[External App] : Int Thresh Reg Val:: lowlow: 0x%x, lowhigh: 0x%x, highlow: 0x%x, highhigh: 0x%x\n", 
                ((struct _int_thresh_reg_struct_ *)&buffer)->thresh_low_low, ((struct _int_thresh_reg_struct_ *)&buffer)->thresh_low_high, 
                ((struct _int_thresh_reg_struct_ *)&buffer)->thresh_high_low, ((struct _int_thresh_reg_struct_ *)&buffer)->thresh_high_high);
    }
  
    sleep(2);

    struct _int_thresh_reg_struct_ int_thresh_reg_struct = {0};
    int_thresh_reg_struct.thresh_low_low = 5;
    int_thresh_reg_struct.thresh_low_high = 0;
    int_thresh_reg_struct.thresh_high_low = 20;
    int_thresh_reg_struct.thresh_high_high = 0;

    strcpy(ext_app_req_msg.req_api_msg, "set_light_sensor_int_thresh_reg");
    ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
    ext_app_req_msg.ptr_param_list = &int_thresh_reg_struct;

    printf("Sending %s request to socket task\n", ext_app_req_msg.req_api_msg);
    num_sent_bytes = send(client_sock, &ext_app_req_msg, 
            sizeof(struct _socket_req_msg_struct_), 0);
    if (num_sent_bytes < 0)
    {
        perror("send failed");
    }
    else
    {
        /* Receiving message from parent process */
        size_t num_read_bytes = read(client_sock, buffer, sizeof(buffer));
        printf("Message received in external app : %s\n", buffer);
    }

    sleep(2);
    
    strcpy(ext_app_req_msg.req_api_msg, "get_light_sensor_int_thresh_reg");
    ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
    ext_app_req_msg.ptr_param_list = NULL;

    printf("Sending %s request to socket task\n", ext_app_req_msg.req_api_msg);
    num_sent_bytes = send(client_sock, &ext_app_req_msg, 
            sizeof(struct _socket_req_msg_struct_), 0);
    if (num_sent_bytes < 0)
    {
        perror("send failed");
    }
    else
    {
        /* Receiving message from parent process */
        size_t num_read_bytes = read(client_sock, buffer, sizeof(buffer));
        printf("[External App] : Int Thresh Reg Val:: lowlow: 0x%x, lowhigh: 0x%x, highlow: 0x%x, highhigh: 0x%x\n", 
                ((struct _int_thresh_reg_struct_ *)&buffer)->thresh_low_low, ((struct _int_thresh_reg_struct_ *)&buffer)->thresh_low_high, 
                ((struct _int_thresh_reg_struct_ *)&buffer)->thresh_high_low, ((struct _int_thresh_reg_struct_ *)&buffer)->thresh_high_high);
    }
	
	return 0;	
}

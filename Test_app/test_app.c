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
	int user_option = 0;
	
	/* Loop for getting input from the user for different operation */
	while(1)
	{
		/* Menu for the user */
		printf("\n/********************************************************************/\n");
		printf("You can enter the option to perform the following operations using this application:\n");
		printf("Enter (1) to get temperature sensor data.\n");
		printf("Enter (2) to get T-Low value of temperature sensor. \n");
		printf("Enter (3) to get T-High value of temperature sensor.\n");
		printf("Enter (4) to get temperature sensor configuration register data.\n");
		printf("Enter (5) to get temperature sensor em.\n");
		printf("Enter (6) to get temperature sensor conversion rate.\n");
		printf("Enter (7) to get temperature sensor fault bits.\n");
		printf("Enter (8) to control temperature sensor (ON-0 / OFF-1).\n");
		printf("Enter (9) to set extended mode operation of temperature sensor (Normal Mode-0 / Extended mode-1).\n");
		printf("Enter (10) to set conversion rate of temperature sensor (0.2Hz-0 , 1Hz-1, 4Hz(Default)-2, 8hz-3).\n");
		printf("Enter (11) to set T-Low value of temperature sensor. \n");
		printf("Enter (12) to set T-High value of temperature sensor.\n");
		printf("Enter (13) to set fault bits of temperature sensor.\n");
        printf("/********************************************************************/\n");
		printf("/*********************LIGHT TASK OPERATIONS**************************/\n");
		printf("/********************************************************************/\n");
		printf("You can enter the option to perform the following operations using this application for temperature task:\n");
		printf("Enter (14) to get light sensor control register.\n");
		printf("Enter (15) to get light sensor lux data.\n");
		printf("Enter (16) to get light sensor ID.\n");
		printf("Enter (17) to get light sensor timing register.\n");
		printf("Enter (18) to get light sensor interrupt threshold register.\n");
		printf("Enter (19) to set light sensor control register.\n");
		printf("Enter (20) to set light sensor integration time.\n");
		printf("Enter (21) to set light sensor gain.\n");
		printf("Enter (22) to set light sensor interrupt threshold low.\n");
		printf("Enter (23) to set light sensor interrupt threshold high.\n");
		printf("/********************************************************************/\n");
		printf("/******************************EXIT**********************************/\n");
		printf("/********************************************************************/\n");
		printf("Enter (24) to exit the external application.\n");
		printf("/********************************************************************/\n");

		printf("\nEnter the option number you want to select:\n");
		scanf("%d",&user_option);
		
		if((user_option > 0) || (user_option < 25)){
            memset(buffer, '\0', sizeof(buffer));
			
			if(user_option == 1){
				
				strcpy(ext_app_req_msg.req_api_msg, "get_temp_data");
				ext_app_req_msg.req_recipient = REQ_RECP_TEMP_TASK;
				ext_app_req_msg.params = -1;

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
				
			}
			else if(user_option == 2){
				
				strcpy(ext_app_req_msg.req_api_msg, "get_temp_low_data");
				ext_app_req_msg.req_recipient = REQ_RECP_TEMP_TASK;
				ext_app_req_msg.params = -1;

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
				
			}
			else if(user_option == 3){
				
				strcpy(ext_app_req_msg.req_api_msg, "get_temp_high_data");
				ext_app_req_msg.req_recipient = REQ_RECP_TEMP_TASK;
				ext_app_req_msg.params = -1;

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
				
			}
			else if(user_option == 4){
				
				strcpy(ext_app_req_msg.req_api_msg, "get_temp_conf_data");
				ext_app_req_msg.req_recipient = REQ_RECP_TEMP_TASK;
				ext_app_req_msg.params = -1;

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
				
			}
			else if(user_option == 5){
				
				strcpy(ext_app_req_msg.req_api_msg, "get_temp_em");
				ext_app_req_msg.req_recipient = REQ_RECP_TEMP_TASK;
				ext_app_req_msg.params = -1;

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
				
			}
			else if(user_option == 6){
				
				strcpy(ext_app_req_msg.req_api_msg, "get_temp_conversion_rate");
				ext_app_req_msg.req_recipient = REQ_RECP_TEMP_TASK;
				ext_app_req_msg.params = -1;

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
				
			}
			else if(user_option == 7){
				
				strcpy(ext_app_req_msg.req_api_msg, "get_temp_fault_bits");
				ext_app_req_msg.req_recipient = REQ_RECP_TEMP_TASK;
				ext_app_req_msg.params = -1;

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
				
			}
			else if(user_option == 8){
				
				strcpy(ext_app_req_msg.req_api_msg, "set_temp_on_off");
				ext_app_req_msg.req_recipient = REQ_RECP_TEMP_TASK;
				
				int temp_control = 0;
				printf("Enter option to control temperature sensor (ON-0 / OFF-1)\n");
				scanf("%d",&temp_control);
				ext_app_req_msg.params = temp_control;

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
			}
			else if(user_option == 9){
				
				strcpy(ext_app_req_msg.req_api_msg, "set_temp_em");
				ext_app_req_msg.req_recipient = REQ_RECP_TEMP_TASK;
				
				uint8_t temp_conversion = 0;
				printf("Enter option to set extended mode operation of temperature sensor (Normal Mode-0 / Extended mode-1)\n");
				scanf("%d", &temp_conversion);
                printf("Temp_Conv: %d\n", temp_conversion);
				ext_app_req_msg.params = temp_conversion;

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
			}
			else if(user_option == 10){
				
				strcpy(ext_app_req_msg.req_api_msg, "set_temp_conversion_rate");
				ext_app_req_msg.req_recipient = REQ_RECP_TEMP_TASK;
				
				int temp_conversion=0;
				printf("Enter option to set conversion rate of temperature sensor (0.2Hz-0 , 1Hz-1, 4Hz(Default)-2, 8hz-3)\n");
				scanf("%d",&temp_conversion);
				ext_app_req_msg.params = temp_conversion;

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
			}
			else if(user_option == 11){
				
				strcpy(ext_app_req_msg.req_api_msg, "set_temp_low_data");
				ext_app_req_msg.req_recipient = REQ_RECP_TEMP_TASK;
				
				int16_t temp_low=0;
				printf("Enter option to set low threshold of temperature sensor\n");
				scanf("%d",&temp_low);
				ext_app_req_msg.params = temp_low;

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
			}
			else if(user_option == 12){
				strcpy(ext_app_req_msg.req_api_msg, "set_temp_high_data");
				ext_app_req_msg.req_recipient = REQ_RECP_TEMP_TASK;
				
				int16_t temp_high=0;
				printf("Enter option to set high threshold of temperature sensor \n");
				scanf("%d",&temp_high);
				ext_app_req_msg.params = temp_high;

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
				
			}
			else if(user_option == 13){
				
				strcpy(ext_app_req_msg.req_api_msg, "set_temp_fault_bits");
				ext_app_req_msg.req_recipient = REQ_RECP_TEMP_TASK;
				
				uint8_t temp_fault=0;
				printf("Enter option to set fault bits of temperature sensor\n");
				scanf("%d",&temp_fault);
				ext_app_req_msg.params = temp_fault;

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
			}
			else if(user_option == 14){
				
				strcpy(ext_app_req_msg.req_api_msg, "get_light_sensor_ctrl_reg");
				ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
				ext_app_req_msg.params = -1;

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
				
			}
			else if(user_option == 15){
				
				strcpy(ext_app_req_msg.req_api_msg, "get_lux_data");
				ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
				ext_app_req_msg.params = -1;

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
				
			}
			else if(user_option == 16){
				
				strcpy(ext_app_req_msg.req_api_msg, "get_light_sensor_id");
				ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
				ext_app_req_msg.params = -1;

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
				
			}
			else if(user_option == 17){
				
				strcpy(ext_app_req_msg.req_api_msg, "get_light_sensor_tim_reg");
				ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
				ext_app_req_msg.params = -1;

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
				
			}
			else if(user_option == 18){
				
				strcpy(ext_app_req_msg.req_api_msg, "get_light_sensor_int_thresh_reg");
				ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
				ext_app_req_msg.params = -1;

				printf("Sending %s request to socket task\n", ext_app_req_msg.req_api_msg);
				ssize_t num_sent_bytes = send(client_sock, &ext_app_req_msg, 
						sizeof(struct _socket_req_msg_struct_), 0);
				if (num_sent_bytes < 0)
				{
					perror("send failed");
				}
				else
				{
                    printf("WAITING ON READ\n");
					/* Receiving message from parent process */
					struct _int_thresh_reg_struct_ int_thresh_reg_struct = {0};
                    size_t num_read_bytes = read(client_sock, &int_thresh_reg_struct, sizeof(struct _int_thresh_reg_struct_));
					//size_t num_read_bytes = read(client_sock, buffer, sizeof(buffer));
                    printf("READ SUCCESS\n");

                    printf("Low Threshold : %d\n", int_thresh_reg_struct.low_thresh);
                    printf("High Threshold : %d\n", int_thresh_reg_struct.high_thresh);
				}
			}
			else if(user_option == 19){
				
				strcpy(ext_app_req_msg.req_api_msg, "set_light_sensor_ctrl_reg");
				ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
				
				uint8_t reg_value=0;
				printf("Enter option to set control register value of light sensor\n");
				scanf("%d",&reg_value);
				ext_app_req_msg.params = (int )reg_value;

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
			}
			else if(user_option == 20){
				
				strcpy(ext_app_req_msg.req_api_msg, "set_light_sensor_integration_time");
				ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
				
				uint8_t integration_time=0;
				printf("Enter option to set integration time of light sensor\n");
				scanf("%d",&integration_time);
				ext_app_req_msg.params = (int )integration_time;

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
			}
			else if(user_option == 21){
				
				strcpy(ext_app_req_msg.req_api_msg, "set_light_sensor_gain");
				ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
				
				uint8_t gain_value=0;
				printf("Enter option to set gain value of light sensor\n");
				scanf("%d",&gain_value);
				ext_app_req_msg.params = (int )gain_value;

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
			}
			else if(user_option == 22){
				
				strcpy(ext_app_req_msg.req_api_msg, "set_interrupt_low_threshold");
				ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
				
				uint16_t low_threshold_value=0;
				printf("Enter option to set low interrupt threshold of light sensor\n");
				scanf("%d",&low_threshold_value);
				ext_app_req_msg.params = (int )low_threshold_value;

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
			}
			else if(user_option == 23){
				
				strcpy(ext_app_req_msg.req_api_msg, "set_interrupt_high_threshold");
				ext_app_req_msg.req_recipient = REQ_RECP_LIGHT_TASK;
				
				uint16_t high_threshold_value=0;
				printf("Enter option to set low interrupt threshold of light sensor\n");
				scanf("%d",&high_threshold_value);
				ext_app_req_msg.params = (int )high_threshold_value;

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
			}
			else if(user_option == 24){
				
				exit(0);
			}
            
            else{
				printf("Invalid option selected, please select the correct option.\n");
			}
			
		}
		else{
			printf("Invalid option selected, please select the correct option.\n");
		}
		
	}
	
	return 0;	
}

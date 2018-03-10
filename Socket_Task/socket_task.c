#include "socket_task.h"

int main(void)
{
	/* Parent process */
	int serv_addr_len = sizeof(server_addr);
	char buffer[BUFF_SIZE];

	// Creating a socket
	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}	

    int option = 1;
    if(setsockopt(server_sockfd,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
        printf("setsockopt failed\n");
        close(server_sockfd);
        exit(2);
    }

    server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SERVER_PORT_NUM);

	if (bind(server_sockfd, (struct sockaddr *)&server_addr, 
							sizeof(server_addr))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(server_sockfd, SERVER_LISTEN_QUEUE_SIZE) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	
	memset(&temp_sock_addr, '0', sizeof(temp_sock_addr));
	temp_sock_addr.sin_family = AF_INET;
	temp_sock_addr.sin_port = htons(TEMPERATURE_TASK_PORT_NUM);
    
    if ((temp_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation for temperature task error \n");
        return -1;
    }
    
    if(setsockopt(temp_sockfd,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
        printf("setsockopt failed\n");
        close(temp_sockfd);
        exit(2);
    }
	
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &temp_sock_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported for temperature task\n");
        return -1;
    }
    
    if (connect(temp_sockfd, (struct sockaddr *)&temp_sock_addr, sizeof(temp_sock_addr)) < 0)
    {
        printf("\nConnection Failed for temperature task \n");
        return -1;
    }
    
	memset(&light_sock_addr, '0', sizeof(light_sock_addr));
	light_sock_addr.sin_family = AF_INET;
	light_sock_addr.sin_port = htons(LIGHT_TASK_PORT_NUM);
    
    if ((light_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation for temperature task error \n");
        return -1;
    }
    
    if(setsockopt(light_sockfd,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
        printf("setsockopt failed\n");
        close(light_sockfd);
        exit(2);
    }

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &light_sock_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported for temperature task\n");
        return -1;
    }
    
    if (connect(light_sockfd, (struct sockaddr *)&light_sock_addr, sizeof(light_sock_addr)) < 0)
    {
        printf("\nConnection Failed for light task \n");
        return -1;
    }

	int accept_conn_id;
	while(1){
		
		if ((accept_conn_id = accept(server_sockfd, (struct sockaddr *)&server_addr, 
					   (socklen_t*)&serv_addr_len)) < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
			
		}
		
		memset(buffer, '\0', sizeof(buffer));
		strncpy(buffer, "Hello!", strlen("Hello!"));
		size_t sent_bytes = send(temp_sockfd, buffer , sizeof(buffer) , 0);
		sent_bytes = send(light_sockfd, buffer , sizeof(buffer) , 0);
		sent_bytes = send(accept_conn_id, buffer , sizeof(buffer) , 0 );
		
	}
	return 0;
}

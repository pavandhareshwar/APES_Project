#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT_NUM 			8500
#define SERVER_LISTEN_QUEUE_SIZE	5

#define BUFF_SIZE			1024

int main(void)
{

	pid_t child_pid;
	child_pid = fork();
	if (child_pid == 0)
	{
		/* Child Process */
		/* Child process will be made client */
		
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
		

		/* Receiving message from parent process */
		size_t num_read_bytes = read(client_sock, buffer, sizeof(buffer));
		printf("Message received in child : %s\n", buffer);
		exit(0);
		
	}
	
	return 0;	
}

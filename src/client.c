#include <netdb.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>

#include "payload.h"
 
#define PORT 8080 
#define SA struct sockaddr 
#define USR_MENU "\n0.EXIT\n1.INSERT\n2.GET\n3.UPDATE\n4.DELETE\n"

void communicate(int32_t sockfd) 
{ 
	payload_t payload;
    uchar_t buffer[sizeof(payload_t)]; 

	char_t input = 0;
	char_t loop_flag = 1;
	char_t will_send = 1;

	/* loop untill user inputs a 0 into the menu */  
    while(loop_flag != 0) 
	{ 
        memset(buffer, 0, sizeof(buffer)); 
		will_send = 1;

		printf(USR_MENU);
		input = getchar();
		switch(input)
		{
			case '0':
				payload.command = EXIT;
				memset(payload.str, 0, STR_SIZE);
				payload.num = -1;
				loop_flag = 0;
				break;
			case '1':
				payload.command = INSERT;
				printf("\nEnter string to insert: ");
				fgets(payload.str, STR_SIZE - 1, stdin);
				printf("\nEnter value to insert: ");
				scanf("%d", &(payload.num));
				break;
			case '2':
				payload.command = GET;
				printf("\nEnter string to get: ");
				fgets(payload.str, STR_SIZE - 1, stdin);
				payload.num = -1;
				break;
			case '3':
				payload.command = UPDATE;
				printf("\nEnter string to update: ");
				fgets(payload.str, STR_SIZE - 1, stdin);
				printf("\nEnter value to update: ");
				scanf("%d", &(payload.num));
				break;
			case '4':
				payload.command = DELETE;
				printf("\nEnter string to delete: ");
				fgets(payload.str, STR_SIZE - 1, stdin);
				payload.num = -1;
				break;
			default:
				printf("\nInvalid option. Try again:\n");
				will_send = 0;
				break;
		}

		if(will_send == 1)
		{
			/* serialize client message */
			(void)serialize_payload(buffer, &payload);

			/* send client message */
			//TODO Check return value of write
        	write(sockfd, buffer, sizeof(buffer)); 

			/* read response into buffer */
			memset(buffer, 0, sizeof(buffer));
			//TODO Check return value of read
			read(sockfd, buffer, sizeof(buffer)); 
			
			/* print response message */
			printf("\nResponse is: %s\n", buffer);
		}
    } 
} 
  
int32_t main() 
{ 
    int32_t sockfd = 0, connfd = 0; 
    struct sockaddr_in servaddr, cli; 
  
    /* create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) 
	{ 
        printf("Socket creation failed...\n"); 
        exit(1); 
    } 
    else
	{
		printf("Socket created...\n"); 
	}

    memset(&servaddr, 0, sizeof(servaddr)); 
  
    /* assign IP and PORT */
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    /* connect to the server socket */
    if ( connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0 ) 
	{ 
        printf("Connection failed...\n"); 
        exit(1); 
    } 
    else
	{
		printf("Connected to server..\n"); 
	}
  
    /* facilitate communication */
    communicate(sockfd); 
  
    /* resource cleanup */
    close(sockfd); 
}

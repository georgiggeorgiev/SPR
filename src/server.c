#include <netdb.h> 
#include <netinet/in.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "trie.h"
#include "payload.h"

#define PORT 8080 
#define SA struct sockaddr 
#define FILE_NAME "data.bin"

void communicate(int32_t sockfd) 
{ 
	trie_t *head = trie_new_node();
	if(head == NULL)
	{
		printf("Memory error...\n");
		return;
	}	

	/* fetch file content */
	int fd = open(FILE_NAME, O_RDONLY);
	char_t read_res = trie_from_file(fd, &head);

	/* no data was read */
	if(read_res == 0)
	{
		printf("File is empty or does not exist. Empty trie was initialized\n\n.");
	}

	/* close file */
	close(fd);
	
	trie_t *temp = NULL;
	payload_t payload;
    uchar_t buffer[sizeof(payload_t)];
	char_t loop_flag = 1;

    /* loop untill exit command is received */ 
    while(loop_flag == 1)
	{ 
    	memset(buffer, 0, sizeof(buffer)); 
  
        /* read client message into buffer */
		//TODO Check return value of read
        read(sockfd, buffer, sizeof(buffer)); 

		/* deserialize client message */
		(void)deserialize_payload(buffer, &payload);

        /* print message */
		printf("Payload content is: %d %s %d\n", payload.command, payload.str, payload.num);

		memset(buffer, 0, sizeof(buffer));

		switch(payload.command)
		{
			case INSERT:
				//TODO Check return value of insert
				trie_insert(&head, payload.str, payload.num);
				sprintf(buffer, "Inserted string %s with value %d.", payload.str, payload.num);
				break;
			case GET:
				temp = trie_search(head, payload.str);
				if(temp != NULL)
				{
					sprintf(buffer, "String: %s Value: %d.", payload.str, temp->value);
				}
				else 
				{
					sprintf(buffer, "No such value.");
				}
				break;
			case UPDATE:
				if( trie_update_value(head, payload.str, payload.num) == 1)
				{
					sprintf(buffer, "Updated string %s with value %d.", payload.str, payload.num);
				}
				else
				{
					sprintf(buffer, "Update failed. Make sure the string you are trying to update already exists.");
				}
				break;
			case DELETE:
				(void)trie_delete(&head, payload.str);
				sprintf(buffer, "Deleted string %s.", payload.str, payload.num);
				break;
			case EXIT:
				sprintf(buffer, "Server shutting down.");
				loop_flag = 0;
				break;
			default:
				break;
		}

		/* send response to client */
		//TODO Check return value of write
		write(sockfd, buffer, sizeof(buffer));
    } 

	/* save changes */
	fd = open(FILE_NAME, O_WRONLY | O_CREAT);
	char_t str[STR_SIZE];
	trie_to_file(fd, head, str, 0);

	/* resource cleanup */
	close(fd);
	trie_delete_all(&head);
} 

int32_t main() 
{ 
    int32_t sockfd = 0, connfd = 0, len = 0; 
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    /* bind socket */
    if ( (bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0 ) 
	{ 
        printf("Socket bind failed...\n"); 
        exit(1); 
    } 
    else
	{
		printf("Socket bound...\n"); 
	}
  
    /* start listening */
    if ( (listen(sockfd, 5)) != 0 ) 
	{ 
        printf("Error in listen...\n"); 
        exit(0); 
    } 
    else
	{
		printf("Server listening...\n"); 
	}

    len = sizeof(cli); 
  
    /* accept packet */
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) 
	{ 
        printf("Acccept failed...\n"); 
        exit(0); 
    } 
    else
	{
		printf("Client accepted...\n"); 	
	}
  
    /* facilitate communication */
    communicate(connfd); 
  
    /* resource cleanup */
    close(sockfd); 
} 

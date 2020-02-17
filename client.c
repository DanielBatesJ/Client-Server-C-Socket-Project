#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#define SOCK_PATH "my_socket"


int main(void)
{
	int MAXLENGTH = 1024;
	int s, t, len;
	struct sockaddr_in servaddr;
	char str[MAXLENGTH];
	char quitChar[] = "quit\n";
	int exitCase = 0;
	// Create the socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	bzero(&servaddr,sizeof(servaddr));  
	// Define the socket's infromation
	printf("Trying to connect...\n");
	servaddr.sin_family=AF_INET;    
	servaddr.sin_port=htons(22000);
	/* Convert IPv4 and IPv6 addresses from text to binary form */    
	inet_pton(AF_INET,"129.120.151.94",&(servaddr.sin_addr));
	// Try connection
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		perror("connect");
		exit(1);
	}

	printf("Connected.\n");
	// Loop for the program
	while(printf("> "), fgets(str, MAXLENGTH, stdin), !feof(stdin))
	{	// Check to see if the user wrote "quit", if so, set the exit condition
		if(strcmp(quitChar, str) == 0)
		{
			exitCase = 1;
		}
		// Send the string to the server
		if (send(s, str, strlen(str), 0) == -1)
		{
			perror("send");
			exit(1);
		}
		// Recieve the string from the server.
		if ((t=recv(s, str, MAXLENGTH, 0)) > 0)
		{
			str[t] = '\0';
			printf("%s", str);
		}
		else
		{
			if (t < 0)
			{
				perror("recv");
			}
			else
			{
				printf("Server closed connection\n");
			}

			exit(1);
		}
		// If the exit condition is met, leave after recieving the 'cya' from the server.
		if(exitCase == 1){
			exit(1);
		}
	}

	close(s);

	return 0;
}

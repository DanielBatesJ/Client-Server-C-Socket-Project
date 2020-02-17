#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>
#define SOCK_PATH "my_socket"


int main(void)
{
	int MAXLENGTH = 1024;
	int s, s2, t, len, i, vowelCnt, wordCnt;
	struct sockaddr_in servaddr, remote;
	char newStr[MAXLENGTH];
	char str[MAXLENGTH];
	char quitChar[] = "quit\n";
	// Creation of the socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	// Zeros out the struct, assigned correct values/
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;    
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);    
    servaddr.sin_port = htons(22000);
    // Binds the socket
	if (bind(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{
		perror("bind");
		exit(1);
	}
	// Listens on that socket
	if (listen(s, 5) == -1)
	{
		perror("listen");
		exit(1);
	}
	for(;;)
	{
		int done, n;
		printf("Waiting for a connection...\n");
		t = sizeof(remote);
		// Accept the connection
		if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1)
		{
			perror("accept");
			exit(1);
		}
		// Print success message in server.
		printf("Connected.\n");
		done = 0;
		// Program loop
		do
		{
			// Get input from client
			n = recv(s2, str, MAXLENGTH, 0);
			if (n > 0)
			{
				str[n] = '\0';
			}
			// Client disconnection handling
			else if (n <= 0)
			{
				if (n < 0) perror("recv");
				done = 1;
			}
			// Processes input, sends message to client
			if (!done)
			{
				wordCnt = 0;
				vowelCnt = 0; 
				int charCnt = 0;
				// Go through the string
				for(i = 0; i < sizeof(str); i++)
				{
					// Condition for finding the ending character.
					if(str[i] == '\0'){
						break;
					}
					// Iterate the number of characters
					charCnt++;
					// Check if it's a letter, if it is, check if it's a vowel.
					if(isalpha(str[i]))
					{
						if (str[i] == 'a' || str[i] == 'e' || str[i] == 'i' ||
				            str[i] == 'o' || str[i] == 'u' || str[i] == 'A' ||
				            str[i] == 'E' || str[i] == 'I' || str[i] == 'O' ||
				            str[i] == 'U') 
						{
							vowelCnt++;
						}
						// Lowercase letters
						str[i] = tolower(str[i]);
					}
					// If you find a space, that shouldn't count towards characters, decrement, and add a word count.
					if(str[i] == ' ')
					{
						charCnt--;
						// If it's the first space, then there has already been one word.
						if(wordCnt == 0)
						{
							wordCnt++;
						}
						wordCnt++;
					}
				}
				// Check for the quit condition.
				if(strcmp(quitChar, str) == 0)
				{
					// Send the goodbye message
					char goodbye[] = "cya!\n";
					if (send(s2, goodbye, 5, 0) < 0)
					{
						perror("send");
						done = 1;
					}
					// Close the socket and exit
					close(s2);
					close(s);
					exit(1);
				}
				// Create a string to return with the information needed
				sprintf(newStr, "%sWords: %d\nCharacters: %d\nVowels: %d\n", str, wordCnt, charCnt-1, vowelCnt);
				n = 0;
				// Calculate length
				for(i = 0; i < MAXLENGTH; i++){
					if(newStr[i] != '\0')
					{
						n++;
					}
				}
				// Send the new string to the client.
				if (send(s2, newStr, n, 0) < 0)
				{
					perror("send");
					done = 1;
				}
			}
		} while (!done);

		close(s2);
	}

	close(s);

	return 0;
}
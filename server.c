#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

// APPLICATION CONSTANTS
#define BUF_SIZE 1024
#define CLK_DELAY 0

// COMMANDS
#define EXT_CMND "cicl"
#define CLK_CMND "clic"

// MESSAGES
#define UNK_MSSG "Unknown command. Use '"CLK_CMND"' to send click message to clients or '"EXT_CMND"' to exit.\n"
#define HLP_MSSG "Usage: %s [-p tcp_port] [-d click_delay]\n(default tcp_port is %d and default click_delay is %d)\n"

// NETWORK CONSTANTS
#define MAX_CONCUR_CLIENTS 10
#define PORT 29900


void sendClickMessage(int connfd){
	write(connfd, "click", 5);
}


int main(int argc, char* argv[]){

	if(argc == 2 && (strcmp(argv[1], "-h")==0)){
		printf(HLP_MSSG, argv[0], PORT, CLK_DELAY);
		exit(EXIT_SUCCESS);
	}

  int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
   
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
   
    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    // Now server is ready to listen and verification
    if ((listen(sockfd, MAX_CONCUR_CLIENTS)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
   
    // Accept the data packet from client and verification
    connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");




	int pipefd[2];
	pid_t currentPid;

	pipe(pipefd); // create the pipe

	currentPid = fork(); // duplicate the current process

	if (currentPid == 0) // if I am the child then
	{
		char fromParent[BUF_SIZE];
		close(pipefd[1]); // close the write-end of the pipe, I'm not going to use it
		int bytesRead;
		while (bytesRead = read(pipefd[0], fromParent, BUF_SIZE) > 0) { // read while EOF
			write(STDOUT_FILENO, "sending click to clients...\n", 28);
			sendClickMessage(connfd);
		}
		close(sockfd);	// After ending close the server socket
		close(pipefd[0]); // close the read-end of the pipe
		exit(EXIT_SUCCESS);
	}
	else // if I am the parent then
	{
		close(pipefd[0]); // close the read-end of the pipe, I'm not going to use it

		char userInput[BUF_SIZE];
		while (1) {
			scanf("%s", userInput);
			if (strcmp(userInput, EXT_CMND) == 0) {
				close(pipefd[1]);
				break;
			}
			else if (strcmp(userInput, CLK_CMND) == 0) {
				write(pipefd[1], userInput, strlen(userInput));
			}
			else {
				write(STDOUT_FILENO, UNK_MSSG, strlen(UNK_MSSG));
			}
		}

		wait(NULL); // wait for the child process to exit before I do the same
		exit(EXIT_SUCCESS);
	}
	return 0;
}
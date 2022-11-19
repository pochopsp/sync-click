#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../macros.h"
#include "./network_functions.h"

// APPLICATION CONSTANTS
#define BUF_SIZE 1024
#define CLI_COUNT 2

// MESSAGES
#define UNK_MSSG "Unknown command. Use '"CLK_CMND"' to send click message to clients or '"EXT_CMND"' to exit.\n"
#define HLP_MSSG "Usage: %s [-p tcp_port] [-d click_clients_no]\n(default tcp_port is %d and default click_delay is %d)\n"

// NETWORK CONSTANTS
#define MAX_PENDING_CLIENTS 

void sendClickMessage(int connfd){
	write(connfd, CLK_CMND, strlen(CLK_CMND));
}


int main(int argc, char* argv[]){

	if(argc == 2 && (strcmp(argv[1], "-h")==0)){
		printf(HLP_MSSG, argv[0], TCP_DEF_PORT, CLI_COUNT);
		exit(EXIT_SUCCESS);
	}
	// TODO recuperare anche il numero di client che bisogna collegare
	inputClickClients = -1;

	const int clickClients = inputClickClients > 2 ? inputClickClients : CLI_COUNT;

	// TODO validazione e settaggio parametri

	char *localIp = (char*)malloc(16*sizeof(char));
	memset(localIp, '\0', 16);
	getLocalMachineIp(localIp);

	int serverSockFD = setupServerSocket(MAX_PENDING_CLIENTS, localIp, PORT);
   
	// TODO
	// salva i connfd in una struttura dinamica, gestiscili in thread separati
	// in ogni thread, PRIMA di restare in attesa per mandare i messaggi click, bisogna aspettare
	// che tutti i client si siano connessi, e che per essi si sia salvato in una struttura
	// il Round-trip time di tutti, e poi si setta in generale quello massimo.
	// INFINE, in attesa di ricevere il messaggio che porta alla scrittura del click

	int *clientsFDs = (int*)malloc(clickClients*sizeof(int));

	int connectedClients = 0;
	while(connectedClients < clickClients){
		struct sockaddr_in cli;
		int len = sizeof(cli);
		// Accept the data packet from client
		int connfd = accept(serverSockFD, (struct sockaddr*)&cli, &len);
		if(connfd < 0){
			printf("server accept failed...\n");
			exit(EXIT_FAILURE);
		}else
			printf("server accept the client...\n");
	}

	

	// ############################################################################################


	int pipefd[2];
	pid_t currentPid;

	pipe(pipefd); // create the pipe

	currentPid = fork(); // duplicate the current process

	if(currentPid != 0) // I am the parent
	{
		close(pipefd[0]); // close the read-end of the pipe, I'm not going to use it

		char userInput[BUF_SIZE];
		while(1){
			scanf("%s", userInput);
			if(strcmp(userInput, EXT_CMND) == 0){
				close(pipefd[1]);
				break;
			}
			else if(strcmp(userInput, CLK_CMND) == 0){
				write(pipefd[1], userInput, strlen(userInput));
			}
			else {
				write(STDOUT_FILENO, UNK_MSSG, strlen(UNK_MSSG));
			}
		}

		wait(NULL); // wait for the child process to exit before I do the same
		exit(EXIT_SUCCESS);
	}
	else // I am the child
	{
		// TODO
		// fai in modo che i thread ALLO STESSO MOMENTO, mandino il messaggio ai client

		char fromParent[BUF_SIZE];
		close(pipefd[1]); // close the write-end of the pipe, I'm not going to use it
		int bytesRead;
		while(bytesRead = read(pipefd[0], fromParent, BUF_SIZE) > 0){ // read while EOF
			write(STDOUT_FILENO, "sending click to clients...\n", 28);
			sendClickMessage(connfd);
		}
		close(serverSockFD);	// After ending close the server socket
		close(pipefd[0]); // close the read-end of the pipe
		exit(EXIT_SUCCESS);
	}
	return 0;
}
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "../macros.h"
#include "./network_functions.h"


// TODO implementare gestione segnali da terminale (ctrl+c ecc)


//a structure we use to pass arguments to threads created for clients connections.
struct ClientThreadArgs{
	int clientSockFD;
    unsigned long clientRTT;
}
typedef ClientThreadArgs ClientThreadArgs;

// APPLICATION CONSTANTS
#define BUF_SIZE 1024
#define DEF_CLI_COUNT 2

// MESSAGES
#define UNK_MSSG "Unknown command. Use '"CLK_CMND"' to send click message to clients or '"EXT_CMND"' to exit.\n"
#define HLP_MSSG "Usage: %s [-p tcp_port] [-d click_clients_no]\n(default tcp_port is %d and default click_clients_no is %d)\n"

// NETWORK CONSTANTS
#define MAX_PENDING_CLIENTS

void sendClickMessageToClients(){
	// TODO fai in modo che i thread ALLO STESSO MOMENTO, mandino il messaggio ai client
	// per fare ciò, devono tutti aspettare su una condition variable e qui si fa in modo che si svegliano
	write(connfd, CLK_CMD, strlen(CLK_CMD));
	// DOPO DI QUESTO DEVONO RIMETTERSI A DORMIRE
}


int main(int argc, char* argv[]){

	if(argc == 2 && (strcmp(argv[1], "-h")==0)){
		printf(HLP_MSSG, argv[0], TCP_DEF_PORT, DEF_CLI_COUNT);
		exit(EXIT_SUCCESS);
	}
	// TODO recuperare anche il numero di client che bisogna collegare
	inputClickClients = -1;

	const int clickClients = inputClickClients > 2 ? inputClickClients : DEF_CLI_COUNT;

	// TODO validazione e settaggio parametri, esempi sotto:
	/*if(argc != 3){
      fprintf(stderr, "usage: %s <ip_address> <port>\n", argv[0]);
      return 1;
    }
    char *ipAddress = argv[1];
    if(!isValidIpAddress(ipAddress)){
      fprintf(stderr, "<ip_address> must be a valid IPv4 address.\n");
      return 2;
    }
    uint16_t port;
    if(stringToUint16(&port, argv[2]) < 0 || port <= 1023){
      fprintf(stderr, "<port> must be a number between 1024 and 65535.\n");
      return 3;
    } */

	char *localIp = (char*)malloc(16*sizeof(char));
	memset(localIp, '\0', 16);
	getLocalMachineIp(localIp);

	int serverSockFD = setupServerSocket(MAX_PENDING_CLIENTS, localIp, PORT);
   
	bool allClientsConnected = false;
	unsigned short connectedClients = 0;
	unsigned long maxRTT = -1;

	while(!allClientsConnected){

		struct sockaddr_in cli;
		int len = sizeof(cli);
		
		int clientSockFd = accept(serverSockFD, (struct sockaddr*)&cli, &len);
		if(clientSockFd < 0){
			perror("socket accept failed for %s", cli.ipAddress), exit(EXIT_FAILURE);
		}else
			printf("server accept the client...\n");

		unsigned long clientRTT = getSocketRTT(clientSockFd);
		if(clientRTT > maxRTT) maxRTT = clientRTT;

		ClientThreadArgs *args = malloc(sizeof(ClientThreadArgs));
		args->clientSockFD = clientSockFD;
		args->clientRTT = clientRTT;

		// TODO appena entrato nella clientHandlerFunction comunicare il client rtt usando il messaggio "MY_RTT_CMD <rtt>"
		pthread_t tid;
		// handle the new client connection
		pthread_create(&tid, NULL, clientHandlerFunction, (void *)args);
		pthread_detach(tid);

		++connectedClients;
		if(connectedClients == clickClients) 
			allClientsConnected = true;
	}

	// TODO 1) sveglia i thread che stanno aspettando sulla condition variable legata ad allClientsConnected
	// TODO 2) comunica ai client maxRTT (magari farla var globale?), usando il messaggio "MAX_RTT_CMD <rtt>"


	// ############################################################################################

	int pipefd[2];
	pid_t currentPid;

	pipe(pipefd); // create the pipe

	currentPid = fork(); // duplicate the current process

	if(currentPid != 0){ // I am the parent
	
		close(pipefd[0]); // close the read-end of the pipe, I'm not going to use it

		char userInput[BUF_SIZE];
		while(true){
			scanf("%s", userInput);
			if(strcmp(userInput, EXT_CMD) == 0){
				close(pipefd[1]);
				break;
			}
			else if(strcmp(userInput, CLK_CMD) == 0){
				write(pipefd[1], userInput, strlen(userInput));
			}
			else {
				printf("%s", UNK_MSSG);
			}
		}

		wait(NULL); // wait for the child process to exit before I do the same
		exit(EXIT_SUCCESS);

	}else{ // I am the child

		char fromParent[BUF_SIZE];
		close(pipefd[1]); // close the write-end of the pipe, I'm not going to use it
		int bytesRead;
		while(bytesRead = read(pipefd[0], fromParent, BUF_SIZE) > 0){ // read while EOF
			printf("sending click to clients...\n");
			sendClickMessageToClients(connfd);
		}
		close(serverSockFD);	// After ending close the server socket
		close(pipefd[0]); // close the read-end of the pipe
		exit(EXIT_SUCCESS);
	}
	return 0;
}
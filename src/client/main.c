#include <winsock2.h>

#include <windows.h>
#include <time.h>

#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>

#include "./string_util.h"
#include "./left_click.h"
#include "../macros.h"

// NETWORK CONSTANTS
#define TCP_MIN_PORT 1025
#define TCP_MAX_PORT 65535

// APPLICATION CONSTANTS
#define BUF_SIZE 1024
#define CLK_DELAY 0

// MESSAGES
#define UNK_MSSG "Unknown command received.\n"


// TODO implementare gestione segnali da terminale (ctrl+c ecc)

// TODO unit testing
long getRTTFromMessage(char *message){
	char **splitMessage = splitString(message, ' ');

	int retrievedRTT = -1;

    if (splitMessage){
		// TODO qualcosa di meglio di atoi? necessario controllare errori?
		retrievedRTT = atoi(*(splitMessage + 1));

        for (unsigned char i = 0; *(splitMessage + i); i++){
            free(*(splitMessage + i));
        }
        free(splitMessage);
    }

	return retrievedRTT;
}

long myRTT;
long maxRTT;

void handleConnection(int sockFD){ 
	int readBytes;
	char buff[BUF_SIZE];

	while ((readBytes = recv(sockFD, buff, BUF_SIZE, 0)) > 0) {
		if ((strncmp(buff, MY_RTT_CMD, strlen(MY_RTT_CMD))) == 0) {
			myRTT = getRTTFromMessage(buff);
			printf("setting my rtt of %d...\n", myRTT);
			break;
		}
		if ((strncmp(buff, MAX_RTT_CMD, strlen(MAX_RTT_CMD))) == 0) {
			maxRTT = getRTTFromMessage(buff);
			printf("setting max rtt of %d...\n", maxRTT);
			break;
		}
		else if ((strncmp(buff, CLK_CMD, strlen(CLK_CMD))) == 0) {
			doDelayedLeftClick(0);
			printf("click received!\n");
		}
		else if ((strncmp(buff, EXT_CMD, strlen(EXT_CMD))) == 0) {
			printf("client exit...\n");
			break;
		}
		else
			printf("%s", UNK_MSSG);
	}
}

void printHelp(char *programName){
	char *helpMessage = "Usage: %s -a ip_address [-p tcp_port]"
				"  \n  -a sets the given ip_address (ipv4 in dot notation) for the socket we want to connect to"
				"  \n  -p sets the given tcp_port (from %d to %d, default is %d) for the socket we want to connect to"
				"  \n  -h prints this help message and exit";
	printf(helpMessage, programName, TCP_MIN_PORT, TCP_MAX_PORT, DEF_TCP_PORT);
}


void initWindowsSocketLib(){
	static WSADATA wsaData;
	int wsaerr = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (wsaerr)
	exit(EXIT_FAILURE);
}


void cleanWindowsSocketLib(){
	WSACleanup();
}

int setupSocketToServer(char *dottedIp, unsigned short port){
	
	struct sockaddr_in servaddr;
 
	// socket create and verification
	int sockFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockFD == -1) {
		perror("socket creation failed...\n"), exit(EXIT_FAILURE);
	}
	printf("Socket successfully created..\n");
	
	// assign IP, PORT
	servaddr.sin_family = AF_INET;        
	servaddr.sin_addr.s_addr = inet_addr(dottedIp);
	servaddr.sin_port = htons(DEF_TCP_PORT);

	// connect the client socket to server socket
	if (connect(sockFD, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
		perror("connection with the server failed...\n"), exit(EXIT_FAILURE);
	}
	printf("connected to the server at ip=%s and port=%s\n", dottedIp, port);

	return sockFD;
}

int main(int argc, char* argv[]){

	if(argc == 2 && (strcmp(argv[1], "-h")==0)){
		printHelp(argv[0]);
		exit(EXIT_SUCCESS);
	}
	// TODO parte di controllo argc, validazione e settaggio dell'input

	char *ip = "0.0.0.0"; // TODO recuperare i veri ip e porta
	unsigned short customPort = -1;

	initWindowsSocketLib();

	unsigned short port = customPort == -1 ? DEF_TCP_PORT : customPort;

	int sockFD = setupSocketToServer(ip, port);
	handleConnection(sockFD);

	// close the socket
	close(sockFD);

	cleanWindowsSocketLib();  
	return 0;
}
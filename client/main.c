#include <winsock2.h>

#include <windows.h>
#include <time.h>

#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>

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


char** splitString(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add +1 space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = NULL;
    }

    return result;
}

getRTTfromMessage(char *message){
	char **splitMessage = splitString(message, ' ')[1];

	int retrievedRTT = -1;

    if (splitMessage){
		// TODO qualcosa di meglio di atoi? necessario controllare errori?
		retrievedRTT = atoi(*(splitMessage + 1));

        int i;
        for (i = 0; *(splitMessage + i); i++){
            free(*(splitMessage + i));
        }
        free(splitMessage);
    }

	return retrievedRTT;
}

int myRTT;
int maxRTT;

void handleConnection(int sockFD){ 
	int readBytes;
	char buff[BUF_SIZE];

	while ((readBytes = recv(sockFD, buff, BUF_SIZE, 0)) > 0) {
		if ((strncmp(buff, MY_RTT_CMD, strlen(MY_RTT_CMD))) == 0) {
			myRTT = getRTTfromMessage(buff);
			printf("setting my rtt of %d...\n", myRTT);
			break;
		}
		if ((strncmp(buff, MAX_RTT_CMD, strlen(MAX_RTT_CMD))) == 0) {
			maxRTT = getRTTfromMessage(buff);
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
	char *helpMessage = "Usage: %s -a ip_address [-p tcp_port] [-h]"
				"  \n  -a sets the given ip_address (ipv4 in dot notation) for the socket we want to connect to"
				"  \n  -p sets the given tcp_port (from %d to %d, default is %d) for the socket we want to connect to"
				"  \n  -h prints this help message";
	printf(helpMessage, programName, TCP_MIN_PORT, TCP_MAX_PORT, TCP_DEF_PORT);
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

setupSocketToServer(char *dottedIp, unsigned short port){
	
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
	servaddr.sin_port = htons(TCP_DEF_PORT);

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

	unsigned short port = customPort == -1 ? TCP_DEF_PORT : customPort;

	int sockFD = setupSocketToServer(ip, port);
	handleConnection(sockFD);

	// close the socket
	close(sockFD);

	cleanWindowsSocketLib();  
	return 0;
}
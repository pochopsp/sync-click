#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>


#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h>

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
#define UNK_MSSG "Unknown command received"


// TODO implementare gestione segnali da terminale (ctrl+c ecc)

// TODO unit testing
long get_rtt_from_message(char *message){
	char **split_message = splitString(message, ' ');

	int retrieved_rtt = -1;

    if (split_message){
		// TODO qualcosa di meglio di atoi? necessario controllare errori?
		retrieved_rtt = atoi(*(split_message + 1));

        for (unsigned char i = 0; *(split_message + i); i++){
            free(*(split_message + i));
        }
        free(split_message);
    }

	return retrieved_rtt;
}

long my_rtt;
long max_rtt;

void handle_connection(SOCKET sockfd){
	int readBytes;
	char buff[BUF_SIZE];

	while ((readBytes = recv(sockfd, buff, BUF_SIZE, 0)) > 0) {
		if ((strncmp(buff, MY_RTT_CMD, strlen(MY_RTT_CMD))) == 0) {
			my_rtt = get_rtt_from_message(buff);
			printf("setting my rtt of %d...\n", my_rtt);
		}
		else if ((strncmp(buff, MAX_RTT_CMD, strlen(MAX_RTT_CMD))) == 0) {
			max_rtt = get_rtt_from_message(buff);
			printf("setting max rtt of %d...\n", max_rtt);
		}
		else if ((strncmp(buff, CLK_CMD, strlen(CLK_CMD))) == 0) {
			doDelayedLeftClick(max_rtt - my_rtt);
			printf("click received!\n");
		}
		else if ((strncmp(buff, EXT_CMD, strlen(EXT_CMD))) == 0) {
			printf("client exit...\n");
			break;
		}
		else
			printf("%s: %s\n", UNK_MSSG, buff);
	}
}

void printhelp(char *program_name){
	char *help_msg = "Usage: %s -a ip_address [-p tcp_port]"
				"  \n  -a sets the given ip_address (ipv4 in dot notation) for the socket we want to connect to"
				"  \n  -p sets the given tcp_port (from %d to %d, default is %d) for the socket we want to connect to"
				"  \n  -h prints this help message and exit";
	printf(help_msg, program_name, TCP_MIN_PORT, TCP_MAX_PORT, DEF_TCP_PORT);
}



SOCKET setup_socket_to_server(char *dotted_ip, unsigned short port){

	int err;
	struct sockaddr_in servaddr;

	// socket create and verification
	SOCKET sockfd = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (sockfd == INVALID_SOCKET) {
		err = WSAGetLastError();
		fprintf(stderr, "socket() failed, error %d\n", err);
		WSACleanup();
		return -1;
	}
	printf("Socket successfully created..\n");

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(dotted_ip);
	servaddr.sin_port = htons(port);

	// connect the client socket to server socket
	if (WSAConnect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
		err = WSAGetLastError();
        fprintf(stderr, "connect() failed, error %d\n", err);
        closesocket(sockfd);
        WSACleanup();
        return -1;
	}
    printf("Connected to the server at ip %s and port %hu.\n", dotted_ip, port);

	unsigned long mode = 0;
	ioctlsocket(sockfd, FIONBIO, &mode);

	return sockfd;
}

int main(int argc, char* argv[]){

	if(argc == 2 && (strcmp(argv[1], "-h")==0)){
		printhelp(argv[0]);
		exit(EXIT_SUCCESS);
	}
	// TODO parte di controllo argc, validazione e settaggio dell'input

	char *ip = "0.0.0.0"; // TODO recuperare i veri ip e porta
	unsigned short custom_port = -1;

	static WSADATA wsadata;
	int wsaerr = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (wsaerr) {
		fprintf(stderr, "WSAStartup() failed, error %d\n", wsaerr);
		return -1;
    }

	unsigned short port = custom_port == -1 ? DEF_TCP_PORT : custom_port;

	int sockfd = setup_socket_to_server(ip, port);
	handle_connection(sockfd);

	closesocket(sockfd);
	WSACleanup();

	return 0;
}
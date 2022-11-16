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


void handleConnection(int sockfd){ 
  int readBytes;
  char buff[BUF_SIZE];

  while ((readBytes = recv(sockfd, buff, BUF_SIZE, 0)) > 0) {
    if ((strncmp(buff, EXT_CMND, strlen(EXT_CMND))) == 0) {
      printf("client exit...\n");
      break;
    }
    else if ((strncmp(buff, CLK_CMND, strlen(CLK_CMND))) == 0) {
      printf("click received!\n");
      doDelayedLeftClick(0);
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

int main(int argc, char* argv[]){

  if(argc == 2 && (strcmp(argv[1], "-h")==0)){
		printHelp(argv[0]);
		exit(EXIT_SUCCESS);
	}

  if(argc == 1){
		printf("KITAMMUO");
    exit(EXIT_FAILURE);
  }
  // TODO parte di validazione e settaggio dell'input

  // TODO spostare in funzione TUTTA la parte di config di rete

  // TODO spostare in funzione la parte di config windows
  // BEGIN instructions needed to use unix socket on windows
  static WSADATA wsaData;
  int wsaerr = WSAStartup(MAKEWORD(2, 0), &wsaData);
  if (wsaerr)
    exit(EXIT_FAILURE);
  // ENDIN instructions needed to use unix socket on windows

  int sockfd;
  struct sockaddr_in servaddr;
 
  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == -1) {
      printf("socket creation failed...\n");
      exit(EXIT_FAILURE);
  }
  else
      printf("Socket successfully created..\n");
  char dottedIp[2];
  char port[2];
  // assign IP, PORT
  servaddr.sin_family = AF_INET;        
  servaddr.sin_addr.s_addr = inet_addr(dottedIp);
  servaddr.sin_port = htons(TCP_DEF_PORT);

  // connect the client socket to server socket
  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
      printf("connection with the server failed...\n");
      exit(EXIT_FAILURE);
  }
  printf("connected to the server at ip=%s and port=%s\n", dottedIp, port);

  handleConnection(sockfd);

  // close the socket
  close(sockfd);

  // BEGIN instructions needed to use unix socket on windows
  WSACleanup();
  // ENDIN instructions needed to use unix socket on windows
  
  return 0;
}
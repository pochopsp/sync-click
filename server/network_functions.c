#include "./network_functions.h"

#include <stdio.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int setupServerSocket(int pendingConnectionsQueueSize, char *ipAddress, uint16_t port){

    FILE *stdLogFile = stdout;

    struct sockaddr_in serverAddress;

	// TODO sostituire con una chiamata equivalente memset
	bzero(&serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    //our server socket listening port and ip
    serverAddress.sin_port = htons(port);                       
    serverAddress.sin_addr.s_addr = inet_addr(ipAddress);

    int serverSD;

    // TODO sostituire 0 con IPPROTO_TCP
    //creating a tcp socket to listen for client requests
    if ((serverSD = socket(PF_INET, SOCK_STREAM, 0)) < 0)
      perror("socket creation failed..."), exit(EXIT_FAILURE);
    fprintf(stdLogFile, "socket successfully created...");

    // bind socket to address
    if (bind(serverSD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
      perror("socket bind failed..."), exit(EXIT_FAILURE);
    fprintf(stdLogFile, "socket successfully binded...");

    // put socket in listen state
    if (listen(serverSD, pendingConnectionsQueueSize) < 0)
      perror("socket listen failed..."), exit(EXIT_FAILURE);
    fprintf(stdLogFile, "socket listening...");

    return serverSD;
}

void getLocalMachineIp(char *ipOutput){
  strcpy(ipOutput, "0.0.0.0\0");
}
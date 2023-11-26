#include "./network_functions.h"

#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int setup_server_socket(int max_pending_conn, char *ip, uint16_t port){

    FILE *stdLogFile = stdout;

    struct sockaddr_in serverAddress;

	  // TODO sostituire con una chiamata equivalente memset
	  bzero(&serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    //our server socket listening port and ip
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr(ip);

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
    if (listen(serverSD, max_pending_conn) < 0)
      perror("socket listen failed..."), exit(EXIT_FAILURE);
    fprintf(stdLogFile, "socket listening...");

    return serverSD;
}

bool get_interface_ip(char const * const interface_name, char * const ip_output){
  // TODO testare
  bool found = false;

  struct ifaddrs *ifap, *ifa;
  struct sockaddr_in *sa;
  char *addr;

  if(getifaddrs(&ifap) == -1) return false;

  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
      sa = (struct sockaddr_in *) ifa->ifa_addr;
      addr = inet_ntoa(sa->sin_addr);
      if(strcmp(interface_name, ifa->ifa_name) == 0){
        strcpy(ip_output, addr);
        found = true;
      }
    }
  }

  freeifaddrs(ifap);
  return found;
}

unsigned long socket_rtt(int socketDescriptor){
/* 		struct tcp_info tcpInfo;
		socklen_t tcpInfoSize = sizeof(ti);
		getsockopt(fd, IPPROTO_TCP, TCP_INFO, &tcpInfo, &tcpInfoSize);
		return tcpInfo.tcpi_rtt; */
    return 1;
}
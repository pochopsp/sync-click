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

    FILE *std_log_file = stdout;

    struct sockaddr_in server_address;

	  // zeroing sockaddr_in struct
	  memset(&server_address, 0, sizeof(struct sockaddr_in));

    server_address.sin_family = AF_INET;
    //our server socket listening port and ip
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);

    int server_sd;

    //creating a tcp socket to listen for client requests
    if ((server_sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
      perror("socket creation failed..."), exit(EXIT_FAILURE);
    fprintf(std_log_file, "socket successfully created...");

    // bind socket to address
    if (bind(server_sd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
      perror("socket bind failed..."), exit(EXIT_FAILURE);
    fprintf(std_log_file, "socket successfully binded...");

    // put socket in listen state
    if (listen(server_sd, max_pending_conn) < 0)
      perror("socket listen failed..."), exit(EXIT_FAILURE);
    fprintf(std_log_file, "socket listening...");

    return server_sd;
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
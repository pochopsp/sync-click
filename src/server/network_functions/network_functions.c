#include "./network_functions.h"

#include <stdio.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>

int setup_server_socket(int max_pending_conn, char *ip, uint16_t port){

  FILE *std_log_file = stdout;

  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(struct sockaddr_in));

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = inet_addr(ip);

  fprintf(std_log_file, "Starting socket setup:\n");

  int server_sd;
  if ((server_sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    perror("Socket creation failed"), exit(EXIT_FAILURE);
  fprintf(std_log_file, "- Socket successfully created.\n");

  if (bind(server_sd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    perror("Socket bind failed"), exit(EXIT_FAILURE);
  fprintf(std_log_file, "- Socket successfully binded.\n");

  if (listen(server_sd, max_pending_conn) < 0)
    perror("Socket listen failed"), exit(EXIT_FAILURE);
  fprintf(std_log_file, "- Socket listening at ip %s and port %u.\n\n", ip, (unsigned int)port);

  return server_sd;
}

bool get_interface_ip(char const * const interface_name, char * const ip_output){
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

unsigned long socket_rtt(int socket_descriptor){
  struct tcp_info tcpinfo;
  socklen_t tcpInfoSize = sizeof(tcpinfo);
  getsockopt(socket_descriptor, IPPROTO_TCP, TCP_INFO, &tcpinfo, &tcpInfoSize);
  return tcpinfo.tcpi_rtt;
}
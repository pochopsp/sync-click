#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <stdint.h>
#include <errno.h>


#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>

#include "../app_macros.h"
#include "./network_functions/network_functions.h"
#include "./client_handling/client_handling.h"
#include "../common/string_functions/string_functions.h"
#include "../common/network_constants.h"


// APPLICATION CONSTANTS
#define BUF_SIZE 1024
#define DEF_CLIENT_COUNT 2
#define MIN_CLIENT_COUNT 1
#define MAX_CLIENT_COUNT 10

// MESSAGES
#define UNK_MSSG "Unknown command. Use '"CLK_CMD"' to send click message to clients or '"EXT_CMD"' to exit.\n"

#define HLP_MSSG "Usage: %s interface_name [-p tcp_port] [-c clients_count]\n    interface_name is the network interface where the server socket will be opened\n    -p sets given tcp_port (default is %d) for the server socket\n    -c sets given clients_count (default is %d) as the number of clients that must connect before click events can be sent\n    -h prints this help message and exit\n"

// NETWORK CONSTANTS
#define MAX_PENDING_CLIENTS 10

static void signal_handler(int signum){
  printf("\nsignal %d received, exiting...\n", signum);
  exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]){

	// ------------- signal handling -------------

	/* if a client closes its socket then a write on that socket
	will result in a SIGPIPE, ignore it to keep the server running */
	signal(SIGPIPE, SIG_IGN);

    // same behaviour for these three signals
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGTSTP, signal_handler);


	// ------------- validate and process input -------------

	uint16_t port = DEF_TCP_PORT;
	uint8_t clients_count = DEF_CLIENT_COUNT;

	int c;
	while ((c = getopt(argc, argv, "hp:c:")) != -1){
		switch (c) {
			case 'h':
				printf(HLP_MSSG, argv[0], DEF_TCP_PORT, DEF_CLIENT_COUNT);
				exit(EXIT_SUCCESS);
			case 'p':
				if(!string_to_uint16(optarg, &port) || port < PORT_LWRBND_LIMIT){
					fprintf(stderr, "<tcp_port> must be a number between %d and %d.\n", PORT_LWRBND_LIMIT, PORT_UPPBND_LIMIT);
					return 1;
				}
				break;
			case 'c':
				if(!string_to_uint8(optarg, &clients_count) || (clients_count < MIN_CLIENT_COUNT || clients_count > MAX_CLIENT_COUNT)){
					fprintf(stderr, "<clients_count> must be a number between %d and %d.\n", MIN_CLIENT_COUNT, MAX_CLIENT_COUNT);
					return 2;
				}
				break;
			case '?':
				if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				return 3;
			default:
				abort ();
		}
	}

	if (argv[optind] == NULL) {
		printf("Mandatory argument <interface_name> is missing.\n");
		return 4;
	}

	if(strlen(argv[optind]) >= INTF_NAME_MAXLENGTH){
		fprintf(stderr, "<interface_name> must be shorter than %d characters.\n", INTF_NAME_MAXLENGTH);
		return 5;
	}
	char interface_name[INTF_NAME_MAXLENGTH];
	strcpy(interface_name, argv[optind]);

	char local_ip[IPV4_DOTNTN_LENGTH+1];
	if(!get_interface_ip(interface_name, local_ip)){
		fprintf(stderr, "Cannot retrieve ip for interface %s.\n", interface_name);
		return 6;
	}


	// ------------- initalize server socket -------------

	int server_sock_fd = setup_server_socket(MAX_PENDING_CLIENTS, local_ip, port);

	bool all_clients_connected = false;
	unsigned char connected_clients = 0;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

	printf("Waiting for all clients to be connected...\n\n");

	char conn_clients_msg[256];
	sprintf(conn_clients_msg, "%d/%d clients connected\n", connected_clients, clients_count);
	write(STDOUT_FILENO, conn_clients_msg, strlen(conn_clients_msg));


	// ------------- accept client connections -------------

	while(!all_clients_connected){

		struct sockaddr_in cli;
		int len = sizeof(cli);

		int client_sock_fd = accept(server_sock_fd, (struct sockaddr*)&cli, &len);
		if(client_sock_fd < 0){
			fprintf(stderr, "Socket accept failed for %s: %s (errno = %d)\n", inet_ntoa(cli.sin_addr), strerror(errno), errno);
			exit(EXIT_FAILURE);
		}

		++connected_clients;

		unsigned long client_rtt = socket_rtt(client_sock_fd);
		if(client_rtt > max_rtt) max_rtt = client_rtt;

		sprintf(conn_clients_msg, "%d/%d clients connected (accepted %s - rtt %lu)\n", connected_clients, clients_count, inet_ntoa(cli.sin_addr), client_rtt);
		write(STDOUT_FILENO, conn_clients_msg, strlen(conn_clients_msg));

		struct clientargs *args = malloc(sizeof(struct clientargs));
		args->client_sock_fd = client_sock_fd;
		args->client_rtt = client_rtt;
		args->mutex = &mutex;
		args->cond = &cond;

		pthread_t tid;
		// handle the new client connection
		pthread_create(&tid, NULL, client_thread_handler, (void *)args);
		pthread_detach(tid);

		if(connected_clients == clients_count){
			all_clients_connected = true;
			pthread_mutex_lock(&mutex);
			time_to_send_maxrtt = true;
			pthread_cond_broadcast(&cond);
    		pthread_mutex_unlock(&mutex);

			printf("\nSending MAX_RTT of %lu to all clients...\n\n", max_rtt);
		}

	}

	// close socket to prevent other clients to connect after clients_count has been reached
	close(server_sock_fd);


	// ------------- receive and process commands from STDIN -------------

	printf("\nReady to receive commands from command line...\n\n");


	char user_input[BUF_SIZE];

	while(true){
		scanf("%s", user_input);
		if(strcmp(user_input, EXT_CMD) == 0){
			printf("EXT_CMD received, exiting...\n");
			break;
		}
		else if(strcmp(user_input, CLK_CMD) == 0){
			printf("CLK_CMD received, sending click to clients...\n");
			pthread_mutex_lock(&mutex);
			pthread_cond_broadcast(&cond);
			pthread_mutex_unlock(&mutex);
		}
		else {
			printf("%s", UNK_MSSG);
		}
	}

	return 0;
}
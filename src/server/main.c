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


#include "../macros.h"
#include "./network_functions/network_functions.h"
#include "./client_handling/client_handling.h"
#include "../common/string_functions/string_functions.h"
#include "../common/network_constants.h"


// TODO implementare gestione segnali da terminale (ctrl+c ecc)

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

/* void sendClickMessageToClients(){
	// TODO fai in modo che i thread ALLO STESSO MOMENTO, mandino il messaggio ai client
	// per fare ciò, devono tutti aspettare su una condition variable e qui si fa in modo che si svegliano
	//write(connfd, CLK_CMD, strlen(CLK_CMD));
	// DOPO DI QUESTO DEVONO RIMETTERSI A DORMIRE
} */


int main(int argc, char* argv[]){

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

	int server_sock_fd = setup_server_socket(MAX_PENDING_CLIENTS, local_ip, port);

	bool all_clients_connected = false;
	unsigned char connected_clients = 0;
	unsigned long max_rtt = -1;

	while(!all_clients_connected){

		struct sockaddr_in cli;
		int len = sizeof(cli);

		int client_sock_fd = accept(server_sock_fd, (struct sockaddr*)&cli, &len);
		if(client_sock_fd < 0){
			fprintf(stderr, "Socket accept failed for %s: %s (errno = %d)\n", inet_ntoa(cli.sin_addr), strerror(errno), errno);
			exit(EXIT_FAILURE);
		}else
			printf("Server accepted the client %s\n", inet_ntoa(cli.sin_addr));

		unsigned long client_rtt = socket_rtt(client_sock_fd);
		if(client_rtt > max_rtt) max_rtt = client_rtt;

		struct clientargs *args = malloc(sizeof(struct clientargs));
		args->client_sock_fd = client_sock_fd;
		args->client_rtt = client_rtt;

		// TODO appena entrato nella clientHandlerFunction comunicare il client rtt usando il messaggio "MY_RTT_CMD <rtt>"
		pthread_t tid;
		// handle the new client connection
		//pthread_create(&tid, NULL, clientHandlerFunction, (void *)args);
		pthread_detach(tid);

		++connected_clients;
		if(connected_clients == clients_count)
			all_clients_connected = true;
	}

	// TODO 1) sveglia i thread che stanno aspettando sulla condition variable legata ad allClientsConnected
	// TODO 2) comunica ai client maxRTT (magari farla var globale?), usando il messaggio "MAX_RTT_CMD <rtt>"


	// ############################################################################################

	int pipefd[2];
	pipe(pipefd); // create the pipe

	pid_t new_pid = fork(); // duplicate the current process

	if(new_pid == -1)
		perror("cannot create child process"), exit(EXIT_FAILURE);

	if(new_pid != 0){ // I am the parent

		close(pipefd[0]); // close the read-end of the pipe, I'm not going to use it

		char user_input[BUF_SIZE];
		while(true){
			scanf("%s", user_input);
			if(strcmp(user_input, EXT_CMD) == 0){
				close(pipefd[1]);
				break;
			}
			else if(strcmp(user_input, CLK_CMD) == 0){
				write(pipefd[1], user_input, strlen(user_input));
			}
			else {
				printf("%s", UNK_MSSG);
			}
		}

		wait(NULL); // wait for the child process to exit before I do the same
		exit(EXIT_SUCCESS);

	}else{ // I am the child

		char from_parent[BUF_SIZE];
		close(pipefd[1]); // close the write-end of the pipe, I'm not going to use it
		int bytes_read;
		while(bytes_read = read(pipefd[0], from_parent, BUF_SIZE) > 0){ // read while EOF
			printf("sending click to clients...\n");
			//sendClickMessageToClients(connfd);
		}
		close(server_sock_fd);	// After ending close the server socket
		close(pipefd[0]); // close the read-end of the pipe
		exit(EXIT_SUCCESS);
	}
	return 0;
}
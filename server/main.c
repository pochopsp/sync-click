#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <stdint.h>


#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>


#include "../macros.h"
#include "./network_functions.h"
#include "../common/string_functions.h"


// TODO implementare gestione segnali da terminale (ctrl+c ecc)


//a structure we use to pass arguments to threads created for clients connections.
struct ClientThreadArgs{
	int client_sock_fd;
    unsigned long clientRTT;
};
typedef ClientThreadArgs ClientThreadArgs;

// APPLICATION CONSTANTS
#define BUF_SIZE 1024
#define DEF_CLIENT_COUNT 2

// MESSAGES
#define UNK_MSSG "Unknown command. Use '"CLK_CMD"' to send click message to clients or '"EXT_CMD"' to exit.\n"

// TODO adegua il messaggio per essere simile a quello del client
#define HLP_MSSG "Usage: %s [-p tcp_port] [-c clients_count]\n\twhen -p tcp_port is not specified, default port is %d\n\twhen -c clients_count is not specified, default clients count is %d\n"

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
				bool conversion_ok = string_to_uint16(optarg, &port);
				if(!conversion_ok || port < 1024){
					fprintf(stderr, "<tcp_port> must be a number between 1024 and 65535.\n");
					return 1;
				}
				break;
			case 'c':
				bool conversion_ok = string_to_uint8(optarg, &clients_count);
				if(!conversion_ok || (clients_count < 2 || clients_count > 10)){
					fprintf(stderr, "<clients_count> must be a number between 2 and 10.\n");
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

	char *local_ip = calloc(16, sizeof(char));
	memset(local_ip, '\0', 16);
	local_machine_ip(local_ip);

	int server_sock_fd = setup_server_socket(MAX_PENDING_CLIENTS, local_ip, port);
   	int server_sock_fd = 1;

	bool all_clients_connected = false;
	unsigned char connected_clients = 0;
	unsigned long max_rtt = -1;

	while(!all_clients_connected){

		struct sockaddr_in cli;
		int len = sizeof(cli);
		
		int client_sock_fd = accept(server_sock_fd, (struct sockaddr*)&cli, &len);
		if(client_sock_fd < 0){
			//TODO recuperare ip del client
			//perror("socket accept failed for %s", cli.ipAddress), exit(EXIT_FAILURE);
		}else
			printf("server accept the client...\n");

		unsigned long client_rtt = socket_rtt(client_sock_fd);
		if(client_rtt > max_rtt) max_rtt = client_rtt;

		ClientThreadArgs *args = malloc(sizeof(ClientThreadArgs));
		args->client_sock_fd = client_sock_fd;
		args->clientRTT = client_rtt;

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
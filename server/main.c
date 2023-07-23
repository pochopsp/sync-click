#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "../macros.h"
#include "./network_functions.h"


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
#define HLP_MSSG "Usage: %s [-p tcp_port] [-c clients_count]\n-when -p tcp_port is not specified, default port is %d\n-when -c clients_count is not specified, default clients count is %d\n"

// NETWORK CONSTANTS
#define MAX_PENDING_CLIENTS 10

/* void sendClickMessageToClients(){
	// TODO fai in modo che i thread ALLO STESSO MOMENTO, mandino il messaggio ai client
	// per fare ciò, devono tutti aspettare su una condition variable e qui si fa in modo che si svegliano
	//write(connfd, CLK_CMD, strlen(CLK_CMD));
	// DOPO DI QUESTO DEVONO RIMETTERSI A DORMIRE
} */


typedef void (*lambda)(void *dest_option_arg, char *src_option_arg);
	// option to lambda
	lambda option_lambdas[ASCII_COUNT] = { NULL };
	option_lambdas[optv[i].option_key]


int main(int argc, char* argv[]){

	// IO CHE SONO IL CHIAMANTE DI PROCESS ARGS.C, TENGO UNA MAPPA ASSOCIA AD OGNI OPTION KEY UNA LAMBDA DA ESEGUIRE, IN QUESTO MODO
	// PROCESSO TUTTO CON PROCESS ARGS, ITERO SULLE OPTION CHE MI SONO STATE RESTUITE ED ESEGUO LE MIE LAMBDA.


	// TODO usare getopts invece di lib custom


	// gestire anche questa opzione mediante la lib fatta ???
	if(argc == 2 && (strcmp(argv[1], "-h")==0)){
		printf(HLP_MSSG, argv[0], DEF_TCP_PORT, DEF_CLIENT_COUNT);
		exit(EXIT_SUCCESS);
	}
	// TODO recuperare anche il numero di client che bisogna collegare
  	unsigned char input_clients_count = -1;

	const unsigned char clients_count = input_clients_count > 2 ? input_clients_count : DEF_CLIENT_COUNT;

	// TODO validazione e settaggio parametri, esempi sotto:
	/*if(argc != 2){
      fprintf(stderr, "usage: %s <ip_address> <port>\n", argv[0]);
      return 1;
    }
    uint16_t port;
    if(stringToUint16(&port, argv[2]) < 0 || port <= 1023){
      fprintf(stderr, "<port> must be a number between 1024 and 65535.\n");
      return 2;
    } */

	unsigned short input_tcp_port = -1;

	const unsigned short PORT = input_tcp_port != -1 ? input_tcp_port : DEF_TCP_PORT;

	char *local_ip = calloc(16, sizeof(char));
	memset(local_ip, '\0', 16);
	local_machine_ip(local_ip);

	int server_sock_fd = setup_server_socket(MAX_PENDING_CLIENTS, local_ip, PORT);
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
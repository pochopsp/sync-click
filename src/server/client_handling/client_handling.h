#ifndef CLIENT_HANDLING_H
#define CLIENT_HANDLING_H

//a structure we use to pass arguments to threads created for clients connections.
struct clientargs{
	int client_sock_fd;
    unsigned long client_rtt;
};
typedef clientargs clientargs;

//connected clients threads' start routine
void* client_thread_handler(void *args);

#endif // CLIENT_HANDLING_H
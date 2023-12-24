#ifndef CLIENT_HANDLING_H
#define CLIENT_HANDLING_H

#include <pthread.h>
#include <stdbool.h>

extern bool time_to_send_maxrtt;
extern unsigned long max_rtt;

//a structure we use to pass arguments to threads created for clients connections.
struct clientargs{
	int client_sock_fd;
    unsigned long client_rtt;
    pthread_mutex_t *mutex;
	pthread_cond_t *cond;
};

//connected clients threads' start routine
void* client_thread_handler(void *args);

#endif // CLIENT_HANDLING_H
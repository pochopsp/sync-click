#include "./client_handling.h"
#include "../../macros.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <sys/socket.h>

bool time_to_send_maxrtt = false;
unsigned long max_rtt = 0;

void* client_thread_handler(void *args){

    struct clientargs *clientargs = args;
    int client_sock_fd = clientargs->client_sock_fd;
    unsigned long client_rtt = clientargs->client_rtt;
    pthread_mutex_t *m = clientargs->mutex;
    pthread_cond_t *c = clientargs->cond;
    free(clientargs); // free the memory previously allocated for the struct

    // 1st step -- send the rtt for its own connection to this client "MY_RTT_CMD <rtt>"
    char str_rtt[256] = {'\0'};
    sprintf(str_rtt, "%s %lu", MY_RTT_CMD, client_rtt);
    write(client_sock_fd, str_rtt, strlen(str_rtt));

    // 2nd step -- send the max rtt among all clients to this client "MAX_RTT_CMD <rtt>"
    pthread_mutex_lock(m);
    while (!time_to_send_maxrtt){
        pthread_cond_wait(c, m);
    }
    pthread_mutex_unlock(m);
    memset(str_rtt, 0, sizeof(str_rtt));
    sprintf(str_rtt, "%s %lu", MAX_RTT_CMD, max_rtt);
    write(client_sock_fd, str_rtt, strlen(str_rtt));

    // 3rd step -- loop to wait for click requests
    while(true){
        pthread_mutex_lock(m);
        pthread_cond_wait(c, m);
        write(client_sock_fd, CLK_CMD, strlen(CLK_CMD));
        pthread_mutex_unlock(m);
    }

    close(client_sock_fd);
    return NULL;
}
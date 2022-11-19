#ifndef NETWORK_FUNCTIONS.H
#define NETWORK_FUNCTIONS.H

#include <netdb.h>

int setupServerSocket(int pendingConnectionsQueueSize, char *ipAddress, uint16_t port);

/** @brief Retrieves the local machine IP address and
 *         puts it inside the buffer pointed by ipOutput.
 *
 *  The buffer pointed by ipOutput should have space for 16 characters
 *  (standard ipV4 length PLUS the terminating character).
 *  If a buffer with less space is provided, result is unpredictable.
 *
 *  @param ipOutput The pointer to the buffer to store the ip.
 *  @return void
 */
void getLocalMachineIp(char *ipOutput);

#endif
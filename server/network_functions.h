#ifndef NETWORK_FUNCTIONS.H
#define NETWORK_FUNCTIONS.H

#include <netdb.h>

// TODO aggiungere documentazione
int setupServerSocket(int pendingConnectionsQueueSize, char *ipAddress, uint16_t port);

/** @brief Retrieves the local machine IP address and
 *         puts it inside the buffer pointed by ipOutput.
 *
 *  The buffer should have space for 16 characters
 *  (standard ipV4 length PLUS the terminating character).
 *  If a buffer with less space is provided, result is unpredictable.
 *
 *  @param ipOutput The pointer to the buffer to store the ip into.
 *  @return Void.
 */
void getLocalMachineIp(char *ipOutput);

// TODO scrivere possibili errori, dettagli ecc, e verificare se millis o micros
/** @brief Retrieves the Round Trip Time for the opened/accepted (?) socket 
 *  specified by socketDescriptor.
 *
 *  @param socketDescriptor The descriptor of the socket we want to obtain RTT of.
 *  @return Unsigned long representing RTT in millis.
 */
unsigned long getSocketRTT(int socketDescriptor);

#endif
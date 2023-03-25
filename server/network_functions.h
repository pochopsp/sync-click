#ifndef NETWORK_FUNCTIONS_H
#define NETWORK_FUNCTIONS_H

#include <netdb.h>

// TODO aggiungere documentazione
int setup_server_socket(int max_pending_conn, char *ip, uint16_t port);

/** @brief Retrieves local IPv4 address and
 *         puts it in the buffer pointed by ip_output.
 *
 *  The buffer must have space for 16 characters
 *  (IPv4 Dot-Decimal notation length plus 1 for terminating character).
 *  If it has less space than needed, program behavior is unpredictable.
 *
 *  @param ip_output The pointer to the buffer to store the ip into.
 *  @return Void.
 */
void local_machine_ip(char *ip_output);

// TODO scrivere possibili errori, dettagli ecc, e verificare se millis o micros
/** @brief Retrieves the Round Trip Time for the opened/accepted (?) socket 
 *  specified by socket_descriptor.
 *
 *  @param socket_descriptor The descriptor of the socket we want to obtain RTT of.
 *  @return Unsigned long representing RTT in millis.
 */
unsigned long socket_rtt(int socket_descriptor);

#endif // NETWORK_FUNCTIONS_H
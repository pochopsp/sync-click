#ifndef NETWORK_FUNCTIONS_H
#define NETWORK_FUNCTIONS_H

#include <netdb.h>
#include <stdbool.h>

// TODO aggiungere documentazione
int setup_server_socket(int max_pending_conn, char *ip, uint16_t port);

/** @brief Retrieves local IPv4 address associated to the interface interface_name
 *  and puts it in the buffer pointed by ip_output.
 *
 *  If the interface_name is found, true is returned and the associated ip will be stored
 *  in the given buffer. Otherwise, false is returned and the buffer will remain untouched.
 *
 *  NOTE: The buffer must have space for 16 characters
 *  (IPv4 Dot-Decimal notation length plus 1 for terminating character).
 *  If it doesn't, program behavior is unpredictable.
 *
 *  @param interface_name The pointer to the string to read the interface name from.
 *  @param ip_output The pointer to the buffer to store the ip into.
 *  @return true if the ip was retrieved successfully, false in case of error.
 */
bool get_interface_ip(char const * const interface_name, char * const ip_output);

// TODO scrivere possibili errori, dettagli ecc, e verificare se millis o micros
/** @brief Retrieves the Round Trip Time for the opened/accepted (?) socket
 *  specified by socket_descriptor.
 *
 *  @param socket_descriptor The descriptor of the socket we want to obtain RTT of.
 *  @return Unsigned long representing RTT in millis.
 */
unsigned long socket_rtt(int socket_descriptor);

#endif // NETWORK_FUNCTIONS_H
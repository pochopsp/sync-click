#ifndef NETWORK_CONSTANTS_H
#define NETWORK_CONSTANTS_H

/** @brief Lower limit for TCP and UDP port number for private use, see
 * https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers#Dynamic,_private_or_ephemeral_ports
 */
extern int PORT_LWRBND_LIMIT;

/** @brief Upper limit for TCP and UDP port number for private use, see
 * https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers#Dynamic,_private_or_ephemeral_ports
 */
extern int PORT_UPPBND_LIMIT;

/** @brief IPv4 length in dot notation
 */
extern int IPV4_DOTNTN_LENGTH;

#endif // NETWORK_CONSTANTS_H
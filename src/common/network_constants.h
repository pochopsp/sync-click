#ifndef NETWORK_CONSTANTS_H
#define NETWORK_CONSTANTS_H

/** @brief Lower limit for TCP and UDP port number for private use, see
 * https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers#Dynamic,_private_or_ephemeral_ports
 */
extern const int PORT_LWRBND_LIMIT;

/** @brief Upper limit for TCP and UDP port number for private use, see
 * https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers#Dynamic,_private_or_ephemeral_ports
 */
extern const int PORT_UPPBND_LIMIT;

/** @brief IPv4 length in decimal dot notation
 */
extern const int IPV4_DOTNTN_LENGTH;

/** @brief Linux network interface name max length, see
 * https://elixir.bootlin.com/linux/v5.6/source/include/uapi/linux/if.h#L33
 */
extern const int INTF_NAME_MAXLENGTH;

#endif // NETWORK_CONSTANTS_H
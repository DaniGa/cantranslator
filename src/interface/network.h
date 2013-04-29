#ifndef _NETWORKUTIL_H_
#define _NETWORKUTIL_H_

#ifdef __USE_NETWORK__
#include "chipKITEthernet.h"
#endif // __USE_NETWORK__

#include "util/bytebuffer.h"

#define USE_DHCP

namespace openxc {
namespace interface {
namespace network {

/** Public: A container for an network connection with queues for both input and
 * output.
 *
 * server - An instance of Server which will allow connections from network
 *      clients.
 * macAddress - MAC address for network device. DEFAULT_MAC_ADRESS will use the
 *      hardware-specified address.
 * ipAddress - static IP address for the network device. If USE_DHCP is defined,
 *      this is ignored.
 *
 * sendQueue - A queue of bytes that need to be sent out over an IP network.
 * receiveQueue - A queue of bytes that have been received via an IP network but
 *      not yet processed.
 */
typedef struct {
    uint8_t ipAddress[4];
    uint8_t macAddress[6];

    // device to host
    QUEUE_TYPE(uint8_t) sendQueue;
    // host to device
    QUEUE_TYPE(uint8_t) receiveQueue;
#ifdef __USE_NETWORK__
    Server* server;
#endif // __USE_NETWORK__
} NetworkDevice;

/** Public: Perform platform-agnostic Network initialization.
 */
void initializeNetworkCommon(NetworkDevice* device);

/** Public:  Initializes the network interface with MAC and IP addresses, starts
 * listening for connections.
 */
void initializeNetwork(NetworkDevice* device);

/** Public: Processes the network send queue and sends its bytes to connected
 * network clients.
 */
void processNetworkSendQueue(NetworkDevice* device);

void readFromSocket(NetworkDevice* device, bool (*callback)(uint8_t*));

} // namespace network
} // namespace interface
} // namespace openxc

#endif

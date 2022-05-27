#ifndef RELIABLE_UDP_VIDEO_STREAM_CLIENT_HELPER_H
#define RELIABLE_UDP_VIDEO_STREAM_CLIENT_HELPER_H

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/reliable-udp-video-stream-client.h"

namespace ns3 {

/**
 * \ingroup reliableudpvideostreamclient
 * \brief Create a client application which waits for input UDP packets which are divided from frames in reliable way to gurantee the order of packets by retransmitting loss packets. */
class ReliableUdpVideoStreamClientHelper
{
public:
  /**
   * Create ReliabeUdpVideoStreamClient which will make life easier for people trying to set up simulatios with reliable-udp-video-stream-server.
   */
  ReliableUdpVideoStreamClientHelper ();
  
  /**
   * Create ReliabeUdpVideoStreamClient which will make life easier for people trying to set up simulatios with reliable-udp-video-stream-server.
   
   * \param ip The IP address of the remote UDP server
   * \param port The port number of the remote UDP server
   */
  ReliableUdpVideoStreamClientHelper (Address ip, uint16_t port);

  /**
   * Create ReliabeUdpVideoStreamClient which will make life easier for people trying to set up simulatios with reliable-udp-video-stream-server.
   
   * \param addr The address of the remote UDP server
   */
  ReliableUdpVideoStreamClientHelper (Address addr);

};

} // namespace ns3

#endif /* RELIABLE_UDP_VIDEO_STREAM_CLIENT_HELPER_H */

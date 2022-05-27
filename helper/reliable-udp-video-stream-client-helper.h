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
   * \brief Create ReliabeUdpVideoStreamClient which will make life easier for people trying to set up simulatios with reliable-udp-video-stream-server.
   */
  ReliableUdpVideoStreamClientHelper ();
  
  /**
   * \brief Create ReliabeUdpVideoStreamClient which will make life easier for people trying to set up simulatios with reliable-udp-video-stream-server.
   
   * \param ip The IP address of the remote UDP server
   * \param port The port number of the remote UDP server
   */
  ReliableUdpVideoStreamClientHelper (Address ip, uint16_t port);

  /**
   * \brief Create ReliabeUdpVideoStreamClient which will make life easier for people trying to set up simulatios with reliable-udp-video-stream-server.
   
   * \param addr The address of the remote UDP server
   */
  ReliableUdpVideoStreamClientHelper (Address addr);

  /**
   * \brief Record an attribute to be set in each Application after it is created.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   */
  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * \brief Create a ReliabeUdpVideoStreamClient application on each of the input container
   
   * \param c NodeContainer of the set of nodes on which a ReliableUdpVideoStreamClient application will be installed.
   * \return Container of Ptr to the applications installed.
   */
  ApplicationContainer Install (NodeContainer c); 

  /**
   * \brief Create a ReliableUdpVideoStreamClient application on each of the input container
   
   * \param node The node on which a ReliableUdpVideoStreamClient applicaton will be installed.
   * \return Container of Ptr to the applications installed.
   */
  ApplicationContainer Install (Ptr<Node> node); 

private:
  /**
   * \brief Install an ns3::ReliableUdpVideoStreamClient on the node configured with all the attributes set with SetAttribute.

   * \param node The node on whch an ReliableUdpVideoStreamClient will be installed.
   * \return Ptr to the application installed.
   */
  Ptr<Application> InstallPriv (Ptr<Node> node);
  ObjectFactory m_factory; //!< Object factory
};

} // namespace ns3

#endif /* RELIABLE_UDP_VIDEO_STREAM_CLIENT_HELPER_H */

#ifndef RELIABLE_UDP_SERVER_HELPER_H
#define RELIABLE_UDP_SERVER_HELPER_H

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "reliable-udp-server.h"

namespace ns3 {

/**
 * \ingroup reliableudpserver
 * \brief Create a reliable udp server application 
 */
class ReliableUdpServerHelper
{
public:
  /**
   * \brief Create ReliableUdpServer which will make life easier for 
   * people trying to set up simulations with reliable-udp-video-stream-server.
   */
  ReliableUdpServerHelper ();
  
  /**
   * \brief Create ReliableUdpClient which will make life easier for 
   * people trying to set up simulations with reliable-udp-video-stream-server.
   
   * \param port The port number on which we listen for incoming packets  
   */
  ReliableUdpServerHelper (uint16_t port);

  /**
   * \brief Record an attribute to be set in each Application after it is created.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   */
  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * \brief Create a ReliableUdpClient application on each of the input container
   
   * \param c NodeContainer of the set of nodes on which a ReliableUdpServer application will be installed.
   * \return Container of Ptr to the applications installed.
   */
  ApplicationContainer Install (NodeContainer c); 

  /**
   * \brief Create a ReliableUdpServer application on each of the input container
   
   * \param node The node on which a ReliableUdpServer applicaton will be installed.
   * \return Container of Ptr to the applications installed.
   */
  ApplicationContainer Install (Ptr<Node> node); 

private:
  /**
   * \brief Install an ns3::ReliableUdpServer on the node configured with all the attributes set with SetAttribute.

   * \param node The node on whch an ReliableUdpServer will be installed.
   * \return Ptr to the application installed.
   */
  Ptr<Application> InstallPriv (Ptr<Node> node);
  ObjectFactory m_factory; //!< Object factory
};

} // namespace ns3

#endif /* RELIABLE_UDP_SERVER_HELPER_H */

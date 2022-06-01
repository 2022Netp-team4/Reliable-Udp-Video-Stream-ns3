#ifndef RELIABLE_UDP_SERVER_H
#define RELIABLE_UDP_SERVER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/traced-callback.h"
#include "ns3/queue.h"
#include <map>


namespace ns3 {

class Socket;
class Packet;

/**
 * \ingroup applications
 * \defgroup reliableudpserver ReliableUdpServer
 */

/**
 * \ingroup reliableudpserver

 * \brief A UDP server, sends UDP packets to a client in a reliable way.
 
 * UDP packets are divided from frames and enqueued to particular queue when 
 * they are not a retransmitted one. The retransmitted packets are enqueued  
 * to the other particular queue for reliability. 
 */
class ReliableUdpServer : public Application
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  
  ReliableUdpServer();

  virtual ~ReliableUdpServer();


protected:
  virtual void DoDispose (void);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  /**
   * \brief Handle a packet reception. 
   * The packet the server receives is either an ack or a request to stop sending.  
   * If ack is received, remove corresponding packet from m_unAckedPackets.
   * If stop request is received, set m_sending flag false. 
   * This function is called by lower layers.
   * \param socket the socket which is a packet was received to.
   */
  void HandleRead (Ptr<Socket> socket);
 
  /**
   * \brief Generate packets and enqueue into m_TxQueue.
   * It is called periodically, and if there is room in the TxQueue, add more packets.
   * m_lastGeneratedSeqNum should be managed here. 
   */
  void GeneratePackets (void);

  /**
   * \brief It transmits the appropriate packet according to the situation.
   * If m_unAckedPackets is not empty, retransmit all packets in it. 
   * else transmit some packets in m_TxQueue, and move them to m_unAckedPackets
   * Note that new packets should not be sent if m_sending is false. 
  */
  void Send (void);

  Ptr<Socket> m_socket;  //!< Ipv4 Socket
  uint16_t m_port;       //!< Port on which we listen for incoming packets 

  uint32_t m_lastGeneratedSeqNum;  //!< Sequence # of recently generated packet 
  uint32_t m_lastSentSeqNum;       //!< Sequence # of recently sent packet 

  // Packets waiting to be transmitted
  Ptr<Queue<Packet> > m_TxQueue;  

  // Packets sent but not acked. This acts as a retransmission buffer. 
  // key: sequence #, value: packet 
  // Why map? It enables quick search and delete. 
  std::map<uint32_t, Packet> m_unAckedPackets;  

  bool m_sending; // !< Indicates whether to send new packets or not. 

  EventId m_generatePacketEvent;  //!< Event to call GeneratePackets() periodically 
  EventId m_sendEvent;            //!< Event to call Send() periodically 
};

} // namespace ns3

#endif /* RELIABLE_UDP_SERVER_H */

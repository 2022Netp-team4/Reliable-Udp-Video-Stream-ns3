#ifndef RELIABLE_UDP_HEADER_H
#define RELIABLE_UDP_HEADER_H

#include <stdint.h>
#include <string>
#include "ns3/header.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"

namespace ns3 {
/**
 * \ingroup reliableudp
 * \brief Packet header for reliable UDP packets
 */
class ReliableUdpHeader : public Header 
{
public:

  /**
   * \brief Constructor
   *
   * Creates a null header
   */
  ReliableUdpHeader ();
  ~ReliableUdpHeader ();

  /**
   * \param seqNum A sequence number that server sends to client 
   */
  void SetSeqNum (uint32_t seqNum);

  /**
   * \param ackNum An acknowledge number that client sends to server 
   */
  void SetAckNum (uint32_t ackNum);

  /**
   * \param signal Signal that client send to server to stop/resume sending.
   * ex) 0x00 for NO_SIGNAL, 0x01 for STOP, 0x02 for RESUME 
   */
  void SetSignal (uint8_t signal);

  void SetRetransmit (uint8_t isRetransmit);


  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  uint32_t m_seqNum;    //!< Sequence #
  uint32_t m_ackNum;    //!< Ack #
  uint8_t m_signal;     //!< Signal to stop/resume sending 
  uint8_t m_isRetransmit; //!< Indicates wheter retransmit or not 
};

} // namespace ns3

#endif /* RELIABLE_UDP_HEADER */

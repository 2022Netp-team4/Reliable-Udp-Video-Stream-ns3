#include <stdint.h>
#include "reliable-udp-header.h"
#include "ns3/udp-header.h"


namespace ns3 {

ReliableUdpHeader::ReliableUdpHeader() :
    m_seqNum(0),
    m_ackNum(0),
    m_signal(0),
    m_isRetransmit(0)
{}

ReliableUdpHeader::~ReliableUdpHeader(){
}

TypeId
ReliableUdpHeader::GetTypeId(void) {
    static TypeId tid = TypeId("ns3::ReliableUdpHeader")
    .SetParent<Header> ()
    .SetGroupName("Headers")
    .AddConstructor<ReliableUdpHeader>();
    return tid;
}

TypeId
ReliableUdpHeader::GetInstanceTypeId (void) const
{
    return GetTypeId ();
}

void
ReliableUdpHeader::Print (std::ostream &os) const
{
  os << "header length: " << GetSerializedSize ()     << " "
     << "AckNum "<< m_ackNum << "SeqNum" << m_seqNum
     << "Retransmit" << m_isRetransmit << "Signal" << m_signal
  ;
}

uint32_t
ReliableUdpHeader::GetSerializedSize (void) const
{
  return 18;
}

void
ReliableUdpHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU32 (m_seqNum);
  i.WriteHtonU32 (m_ackNum);
  i.WriteU8 (m_signal);
  i.WriteU8 (m_isRetransmit);
}

uint32_t
ReliableUdpHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_seqNum = i.ReadU32 ();
  m_ackNum = i.ReadU32 ();
  m_signal = i.ReadU8 ();
  m_isRetransmit = i.ReadU8 ();

  return GetSerializedSize ();
}

void 
ReliableUdpHeader::SetSeqNum (uint32_t seqNum){
  m_seqNum = seqNum;
}

void 
ReliableUdpHeader::SetAckNum (uint32_t ackNum){
  m_ackNum = ackNum;
}

void 
ReliableUdpHeader::SetSignal (uint8_t signal){
  m_signal = signal;
}

void 
ReliableUdpHeader::SetRetransmit (uint8_t isRetransmit){
  m_isRetransmit = isRetransmit;
}

uint32_t 
ReliableUdpHeader::GetSeqNum (){
  return m_seqNum;
}

uint32_t 
ReliableUdpHeader::GetAckNum (){
  return m_ackNum;
}

uint8_t 
ReliableUdpHeader::GetSignal (){
  return m_signal;
}

uint8_t 
ReliableUdpHeader::GetRetransmit (){
  return m_isRetransmit;
}

}


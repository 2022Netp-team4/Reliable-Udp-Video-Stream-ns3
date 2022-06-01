#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/drop-tail-queue.h"
#include "reliable-udp-client.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ReliableUdpClientApplication");
NS_OBJECT_ENSURE_REGISTERED (ReliableUdpClient);

TypeId ReliableUdpClient::GetTypeId (void) 
{
  static TypeId tid = TypeId ("ns3::ReliableUdpClient")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<ReliableUdpClient> ()
    .AddAttribute ("RemoteAddress",
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&ReliableUdpClient::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("RemotePort", "The destination port of the outbound packets",
                   UintegerValue (100),
                   MakeUintegerAccessor (&ReliableUdpClient::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
    ;
    return tid;
}

ReliableUdpClient::ReliableUdpClient ()
{
  m_receiving = true;
  m_socket = 0;
  m_lastArrangedSeq = 0;
  m_outOfOrderQueue = m_inOrderQueue = 0;
}

ReliableUdpClient::~ReliableUdpClient ()
{
}

void
ReliableUdpClient::SetRemote (Address ip, uint16_t port)
{
  m_peerAddress = ip;
  m_peerPort = port;
}

void 
ReliableUdpClient::SetRemote (Address addr)
{
  m_peerAddress = addr;
}

void 
ReliableUdpClient::DoDispose (void)
{
  Application::DoDispose ();
}

void
ReliableUdpClient::StartApplication (void)
{
  if (m_outOfOrderQueue == 0) 
    m_outOfOrderQueue = CreateObject<DropTailQueue<Packet> > ();
  if (m_inOrderQueue == 0)
    m_inOrderQueue = CreateObject<DropTailQueue<Packet> > ();

  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 9);
  if (m_socket == 0) {
    TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
    m_socket = Socket::CreateSocket (GetNode (), tid);

    if (Ipv4Address::IsMatchingType(m_peerAddress) == true) {
      if (m_socket->Bind (local) == -1) {
        NS_FATAL_ERROR ("Failed to bind socket");
      }
      m_socket->Connect (InetSocketAddress(Ipv4Address::ConvertFrom(m_peerAddress), m_peerPort));
    } else if (InetSocketAddress::IsMatchingType(m_peerAddress) == true) {
      if (m_socket->Bind (local) == -1) {
        NS_FATAL_ERROR ("Failed to bind socket");
      }
      m_socket->Connect (m_peerAddress);
    } else {
      NS_ASSERT_MSG (false, "Incompatible address type: " << m_peerAddress);
    }
  }

  m_socket->SetRecvCallback (MakeCallback (&ReliableUdpClient::HandleRead, this));
  m_outOfOrderQueue->SetMaxSize (QueueSize (ns3::BYTES, MAX_QUEUE_SIZE));
  m_rearrangePacketsEvent = Simulator::Schedule (
    MilliSeconds(10), 
    &ReliableUdpClient::RearrangePackets, this
  );
  m_consumePacketsEvent = Simulator::Schedule (
    MilliSeconds(33),
    &ReliableUdpClient::ConsumePackets, this
  );
}

void
ReliableUdpClient::StopApplication (void) 
{
  Simulator::Cancel (m_rearrangePacketsEvent);
  Simulator::Cancel (m_consumePacketsEvent);
}

void
ReliableUdpClient::HandleRead (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from))) {
    if (packet->GetSize () > 0) {
      // If packet arrives while incoming is blocked,
      // resend stop request 
      if (!m_receiving) {
        SendAck (0, 0x01);
        return;
      }
      ReliableUdpHeader recvHeader;
      packet->RemoveHeader (recvHeader);
      uint8_t isRetransmit = recvHeader.GetRetransmit();
      uint32_t seq = recvHeader.GetSeqNum();

      // As retransmit queue sorts packets using info in header, 
      // we should re-attach it. 
      packet->AddHeader (recvHeader);

      uint8_t signal = 0x00;
      // For retransmitted packets, push into retransmit queue 
      if (isRetransmit) {
        m_retransQueue.push(packet);
        std::cout << "ret " << seq << std::endl;
      // For regular packets, push into out-of-order queue 
      } else {
        if (false) {
          m_receiving = false;
          signal = 0x01;
        } else {
          m_outOfOrderQueue->Enqueue(packet);
          std::cout << "reg " << seq << std::endl;
        }
      }
      SendAck (recvHeader.GetSeqNum (), signal); 
    }
  }
}

void
ReliableUdpClient::RearrangePackets (void)
{
  // If enough room is made in out-of-order queue, 
  // require to resume sending 
  uint32_t queueSize = m_outOfOrderQueue->GetMaxSize().GetValue();
  uint32_t occupied = m_outOfOrderQueue->GetNBytes();
  if (!m_receiving && queueSize - occupied > 4096) {
    m_receiving = true;
    SendAck(0, 0x02);
  }

  // Check out-of-order queue if next packet exists 
  if (!m_outOfOrderQueue->IsEmpty()) {
    ReliableUdpHeader header;
    Ptr<Packet> p = m_outOfOrderQueue->Peek()->Copy();
    p->RemoveHeader (header);
    // In-order; schedule and return 
    if (header.GetSeqNum() == m_lastArrangedSeq + 1) {
      m_outOfOrderQueue->Dequeue();
      m_inOrderQueue->Enqueue(p);
      m_lastArrangedSeq++;

      m_rearrangePacketsEvent = Simulator::Schedule(
        MilliSeconds(10), 
        &ReliableUdpClient::RearrangePackets, this
      );
      return;
    } 
  }

  // Next packet is not at the front of out-of-order queue
  // Check retransmit queue if next packet exists 
  if (!m_retransQueue.empty()) {
    Ptr<Packet> firstRetransmitPacket = m_retransQueue.top()->Copy();
    ReliableUdpHeader rHeader;
    firstRetransmitPacket->RemoveHeader(rHeader);
    if (rHeader.GetSeqNum() == m_lastArrangedSeq + 1) {
      m_retransQueue.pop();
      m_inOrderQueue->Enqueue(firstRetransmitPacket);
      m_lastArrangedSeq++;
    }
  }

  // Repeat
  m_rearrangePacketsEvent = Simulator::Schedule (
    MilliSeconds(200), 
    &ReliableUdpClient::RearrangePackets, this
  );
}

void
ReliableUdpClient::ConsumePackets (void) 
{
  // Dequeue packets from in-order queue 
  uint32_t nPackets = m_inOrderQueue->GetNPackets();
  if (nPackets >= 200) 
    for (int i = 0; i < 200; i++) {
      m_inOrderQueue->Dequeue();
    }
  m_consumePacketsEvent = Simulator::Schedule (
    MilliSeconds(33),
    &ReliableUdpClient::ConsumePackets, this
  );
}

void
ReliableUdpClient::SendAck (uint32_t ackNum, uint8_t signal)
{
  ReliableUdpHeader ackHeader;
  ackHeader.SetAckNum (ackNum);
  ackHeader.SetSignal (signal);
  Ptr<Packet> headerOnlyPacket = Create<Packet> (ackHeader.GetSerializedSize());
  headerOnlyPacket->AddHeader (ackHeader);
  m_socket->Send (headerOnlyPacket);
}

}


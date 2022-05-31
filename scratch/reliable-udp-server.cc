#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/traced-callback.h"
#include "ns3/queue.h"
#include <map>

#include "reliable-udp-server.h"
#include "reliable-udp-header.h"

namespace ns3 {

    NS_LOG_COMPONENT_DEFINE ("ReliableUdpServerApplication");

    NS_OBJECT_ENSURE_REGISTERED (ReliableUdpServer);

    TypeId
    ReliableUdpServer::GetTypeId(void) {
        static TypeId tid = TypeId("ns3::ReliableUdpServer")
                .SetParent<Application>()
                .SetGroupName("Applications")
                .AddConstructor<ReliableUdp>()
                .AddAttribute("Port", "Port on which we listen for incoming packets.",
                              UintegerValue(9),
                              MakeUintegerAccessor(&ReliableUdpServer::m_port),
                              MakeUintegerChecker<uint16_t>());
        return tid;
    }

    ReliableUdpServer::ReliableUdpServer() {
        NS_LOG_FUNCTION(this);
        m_sending = true;
        m_lastGeneratedSeqNum = -1;
        m_lastGeneratedSeqNum = -1;
    }

    ReliableUdpServer::~ReliableUdpServer() {
        NS_LOG_FUNCTION(this);
        m_socket = 0;
    }

    void
    ReliableUdpServer::DoDispose(void) {
        NS_LOG_FUNCTION(this);
        Application::DoDispose();
    }

    void
    ReliableUdpServer::StartApplication(void) {
        NS_LOG_FUNCTION(this);

        if (m_socket == 0) {
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
            m_socket = Socket::CreateSocket(GetNode(), tid);
            InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), m_port);
            if (m_socket->Bind(local) == -1) {
                NS_FATAL_ERROR("Failed to bind socket");
            }
            if (addressUtils::IsMulticast(m_local)) {
                Ptr <UdpSocket> udpSocket = DynamicCast<UdpSocket>(m_socket);
                if (udpSocket) {
                    // equivalent to setsockopt (MCAST_JOIN_GROUP)
                    udpSocket->MulticastJoinGroup(0, m_local);
                } else {
                    NS_FATAL_ERROR("Error: Failed to join multicast group");
                }
            }
        }

        m_socket->SetRecvCallback(MakeCallback(&ReliableUdpServer::HandleRead, this));
    }

    void
    ReliableUdpServer::StopApplication() {
        NS_LOG_FUNCTION(this);

        if (m_socket != 0) {
            m_socket->Close();
            m_socket->SetRecvCallback(MakeNullCallback < void, Ptr < Socket > > ());
        }
    }

    void
    ReliableUdpServer::HandleRead(Ptr <Socket> socket) {
        NS_LOG_FUNCTION(this << socket)

        Ptr <Packet> packet;
        Address from;
        Address localAddress;
        ReliableUdpHeader header;

        while ((packet = socket->RecvFrom(from))) {
            socket->GetSockName(localAddress);
            packet->PeekHeader(header);
            if (InetSocketAddress::IsMatchingType(from)) {
                NS_LOG_INFO("At time " << Simulator::Now().GetSeconds()
                                       << "s server received " << packet->GetSize()
                                       << " bytes from " << InetSocketAddress::ConvertFrom(from).GetIpv4()
                                       << " port " << InetSocketAddress::ConvertFrom(from).GetPort()
                                       << " seq " << header.GetSeqNum()
                                       << " ack " << header.GetAckNum());
            }
            if (header.GetSignal()) {
                m_sending = false
            } else {
                m_unAckedPackets.erase(header.GetAckNum());
            }
        }
    }

    void
    ReliableUdpServer::GeneratePackets() {
        uint32_t packetSize = 1024;
        uint8_t dataBuffer[packetSize];
        Ptr <Packet> packet = Create<Packet>(dataBuffer, packetSize);

        ReliableUdpHeader *header = new Header();
        header->SetSeqNum(++m_lastGeneratedSeqNum);
        header->SetRetransmit(0);

        packet->AddHeader(header);
        m_TxQueue->Enqueue(packet);
    }

    void
    ReliableUdpServer::Send() {
        ReliableUdpHeader header;
        if (m_sending) {
            if ((!m_unAckedPackets.emmpty())) {
                for (Ptr <Packet> p: m_unAckedPackets.values()) {
                    m_socket->Send(p);
                }
                m_unAckedPackets.clear();
            } else {
                while (!m_TxQueue->IsEmpty()) {
                    Ptr <Packet> p = m_TxQueue->Peek();
                    p->PeekHeader(header);
                    m_socket->Send(p);
                    m_lastSentSeqNum = header.GetSeqNum()
                    m_TxQueue->Dequeue();
                    m_unAckedPackets[header.GetSeqNum()] = p;
                }
            }
        }
    }
}
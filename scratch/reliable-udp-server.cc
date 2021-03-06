#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/traced-callback.h"
#include "ns3/queue.h"
#include "ns3/socket.h"
#include "ns3/address-utils.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/drop-tail-queue.h"
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
                .AddConstructor<ReliableUdpServer>()
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
        m_TxQueue = 0;
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

        if (m_TxQueue == 0) 
            m_TxQueue = CreateObject<DropTailQueue<Packet> > ();
        if (m_socket == 0) {
            TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
            m_socket = Socket::CreateSocket(GetNode(), tid);
            InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), m_port);
            if (m_socket->Bind(local) == -1) {
                NS_FATAL_ERROR("Failed to bind socket");
            }
        }

        m_socket->SetRecvCallback(MakeCallback(&ReliableUdpServer::HandleRead, this));
        m_generatePacketEvent = Simulator::Schedule(
                MilliSeconds(10),
                &ReliableUdpServer::GeneratePackets, this
        );
        m_sendEvent = Simulator::Schedule(
                MilliSeconds(33),
                &ReliableUdpServer::Send, this
        );
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
        NS_LOG_FUNCTION(this << socket);

        Ptr<Packet> packet;
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
                m_sending = false;
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

        ReliableUdpHeader header;
        header.SetSeqNum(++m_lastGeneratedSeqNum);
        header.SetRetransmit(0);

        packet->AddHeader(header);
        m_TxQueue->Enqueue(packet);

        m_generatePacketEvent = Simulator::Schedule(
                MilliSeconds(10),
                &ReliableUdpServer::GeneratePackets, this
        );
    }

    void
    ReliableUdpServer::Send() {
        InetSocketAddress client ("10.1.1.1", 9);
        ReliableUdpHeader header;
        if (m_sending) {
            if ((!m_unAckedPackets.empty())) {
                for (std::map<uint32_t, Packet>::iterator it = m_unAckedPackets.begin(); 
                     it != m_unAckedPackets.end(); ++it) {
                    m_socket->SendTo(&(it->second), 0, client);
                }
                m_unAckedPackets.clear();
            } else {
                while (!m_TxQueue->IsEmpty()) {
                    Ptr <Packet> p = m_TxQueue->Peek()->Copy();
                    p->PeekHeader(header);
                    m_socket->SendTo(p, 0, client);
                    m_lastSentSeqNum = header.GetSeqNum();
                    m_TxQueue->Dequeue();
                    m_unAckedPackets[header.GetSeqNum()] = *p;
                }
            }
        }
        m_sendEvent = Simulator::Schedule(
            MilliSeconds(33),
            &ReliableUdpServer::Send, this
        );
    }
}


#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/traced-callback.h"
#include "ns3/queue.h"

#include "reliable-udp-server.h

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
                              MakeUintegerAccessor(&UdpEchoServer::m_port),
                              MakeUintegerChecker<uint16_t>());
        return tid;
    }
}
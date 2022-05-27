#ifndef RELIABLE_UDP_VIDEO_STREAM_CLIENT_H
#define RELIABLE_UDP_VIDEO_STREAM_CLIENT_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/traced-callback.h"

namespace ns3 {
/**
 * \ingroup applications
 * \defgroup reliableudpvideostreamclient ReliableUdpVideoStreamClient
 */

/**
 * \ingroup reliableudpvideostreamclient

 * \brief A UDP client, receives UDP packets from a remote server in a reliable way.
 
 * UDP packets are divided from frames and enqueued to particular queue when * they are not a retransmitted one. The retransmitted packets are enqueued  * to the other particular queue for reliability. 
 */
class ReliableUdpVideoStreamClient : public Application
{

};
} // namespace ns3
#endif /* RELIABLE_UDP_VIDEO_STREAM_CLIENT_H */

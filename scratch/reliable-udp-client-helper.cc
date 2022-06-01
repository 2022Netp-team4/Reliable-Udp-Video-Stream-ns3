#include "reliable-udp-client-helper.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

ReliableUdpClientHelper::ReliableUdpClientHelper() {
  m_factory.SetTypeId (ReliableUdpClientHelper::GetTypeId());
}

ReliableUdpClientHelper::ReliableUdpClientHelper(Address address, uint16_t port) {
  m_factory.SetTypeId (ReliableUdpClientHelper::GetTypeId());
  SetAttribute ("RemoteAddress", AddressValue (address));
  SetAttribute ("RemortPort", UintegerValue (port));
}

void
ReliableUdpClientHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
ReliableUdpClientHelper::Install (NodeContainer c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End(); ++i) {
    Ptr<Node> node = *i;
    Ptr<ReliableUdpClient> client = m_factory.Create<ReliableUdpClient> ();
    node->AddApplication (client);
    apps.Add (client);
  }
  return apps;
}

}
